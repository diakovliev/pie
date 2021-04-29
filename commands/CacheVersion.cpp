/*
 * Copyright (c) 2018, 2021, diakovliev
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY diakovliev ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL diakovliev BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <algorithm>

#include "CacheVersion.h"
#include "CacheObject.h"
#include "QueryCache.h"

#include <properties.h>
#include <logging.h>

#include "GavcUtils.h"

namespace piel::cmd {

    namespace pl = piel::lib;

    CacheVersion::CacheVersion(const IArtifactCache* parent, std::string version, fs::path root)
        : parent_(parent)
        , version_(std::move(version))
        , root_(std::move(root))
    {
    }


    std::string CacheVersion::version() const {
        return version_;
    }


    fs::path CacheVersion::root() const {
        return root_;
    }


    const IArtifactCache* CacheVersion::parent() const {
        return parent_;
    }


    std::vector<std::string> CacheVersion::get_requested_classifiers() const {

        std::vector<std::string> requested_classifiers;

        const QueryCache* qcache = dynamic_cast<const QueryCache*>(parent());
        if(qcache) {
            requested_classifiers = ::utils::gavc::parse_classifiers_spec(qcache->context()->query().classifier());
        } else {
            requested_classifiers.emplace_back();
        }

        return requested_classifiers;
    }


    std::vector<CacheObject> CacheVersion::objects(CacheObject::Filter objects_filter) const {
        LOGT << "Request for objects cached in: " << root_ << " cache directory."<< ELOG;

        std::vector<CacheObject> result;
        if(!fs::is_directory(root_)) {
            LOGT << root_ << " is not a directory. Return empty list." << ELOG;
            return result;
        }

        result.reserve(10);

        std::vector<std::string> requested_classifiers = get_requested_classifiers();

        for(auto& entry: fs::directory_iterator(root_)) {
            if(fs::is_directory(entry)) {
                LOGT << "Entry: " << entry << " is a directory." << ELOG;
                continue;
            }

            auto object = entry.path();

            if(CacheObject::is_object_properties(object)) {
                LOGT << "Entry: " << object << " is a properties file." << ELOG;
                continue;
            }

            if(!CacheObject::is_object(object)) {
                LOGT << "Entry: " << object << " is not an object." << ELOG;
                continue;
            }

            LOGT << "Try to apply objects filter to object: " << object << ELOG;

            CacheObject cache_object(this, object);

            if (!objects_filter(cache_object)) {
                LOGT << "Object: " << object << " rejected by objects filter." << ELOG;
                continue;
            }

            LOGT << "Validate object: " << object << ELOG;
            if (!cache_object.is_valid()) {
                LOGT << "Object: " << object << " is not valid." << ELOG;
                continue;
            }

            if(requested_classifiers.size() == 1 && requested_classifiers[0].empty()) {
                LOGT << "Requested empty classifier. Add object: " << object << " to results." << ELOG;
                result.push_back(std::move(cache_object));
                continue;
            }

            auto cache_object_classifier = cache_object.classifier();

            LOGT << "Cached object: " << object << " classifier: " << cache_object_classifier << ELOG;

            auto ret = std::find_if(requested_classifiers.begin(), requested_classifiers.end(), [cache_object_classifier](const auto& rc) {
                return cache_object_classifier == ::utils::gavc::classifier_significant_part(rc);
            });

            if(ret == requested_classifiers.end()) {
                LOGT << "Object classifier: " << cache_object_classifier << " is no in requested classifiers list." << ELOG;
                continue;
            }

            LOGT << "Add object: " << object << " to results." << ELOG;
            result.push_back(std::move(cache_object));
        }

        return result;
    }

} // namespace piel::cmd
