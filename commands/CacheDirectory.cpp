/*
 * Copyright (c) 2021, diakovliev
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

#include <set>
#include <fstream>

#include "CacheDirectory.h"

#include <properties.h>
#include <logging.h>

#include <std_filesystem_ext.hpp>

#include "gavcconstants.h"
#include "StringUtils.h"

namespace piel::cmd {

    CacheDirectory::CacheDirectory(fs::path cache_root)
        : cache_root_(std::move(cache_root))
    {
    }


    std::string CacheDirectory::cache_properties_file() const
    {
        return cache_root_ / GAVCConstants::cache_properties_filename;
    }


    void CacheDirectory::init() const
    {
        if (!fs::is_directory(cache_root_) || !fs::exists(cache_root_)) {
            LOGT << "Try to remove existing file which name is conflict with cache path! cache path: " << cache_root_ << ELOG;
            try {
                fs::remove_all(cache_root_);
            } catch (...) {
                throw std::runtime_error("Unable to delete existing file whose name conflicts with cache path! Cache path: " + cache_root_.string());
            }
        }

        if (!fs::exists(cache_root_)) {
            LOGT << "Try to create cache directory at cache path: " << cache_root_ << ELOG;
            try {
                fs::create_directories(cache_root_);
            } catch (...) {
                throw std::runtime_error("Unable to create cache directory at cache path: " + cache_root_.string());
            }
        }

        LOGT << "Create cache properties..." << ELOG;
        pl::Properties props = pl::Properties();

        LOGT << "Set cache version to: " << GAVCConstants::cache_version;
        props.set(GAVCConstants::cache_version_property, GAVCConstants::cache_version);

        auto cache_properties_filepath = cache_properties_file();

        LOGT << "Write cache properties file: " << cache_properties_filepath;
        std::ofstream os(cache_properties_filepath);
        props.store(os);
    }


    bool CacheDirectory::validate() const
    {
        LOGT << "Try to read cache properties..." << ELOG;

        std::string properties_file = cache_properties_file();

        if (!fs::is_regular_file(properties_file)) {
            LOGT << "No cache properties file! Cache is invalid!" << ELOG;
            return false;
        }

        std::ifstream is(properties_file);
        pl::Properties props = pl::Properties::load(is);

        std::string properties_version = props.get(GAVCConstants::cache_version_property, "");

        bool ret = GAVCConstants::cache_version == properties_version;

        LOGT
            << "Validate cache version. Expected version: "     << GAVCConstants::cache_version
            << " version from properties: "                     << properties_version
            << " cache version validation result: "             << ret
            << ELOG;

        return ret;
    }


    void CacheDirectory::remove() const {
        fs::remove_all(cache_root_);
    }


    void CacheDirectory::reinit() const {
        if (!validate()) {
            try {
                remove();
            } catch (...) {}
            init();
        }
    }


    std::vector<NonQueryCache> CacheDirectory::artifacts() const {
        std::vector<NonQueryCache> result;

        std::set<fs::path> artifacts_paths;

        LOGT << "Look for artifacts caches in: " << cache_root_ << ELOG;

        for(auto& entry: fs::recursive_directory_iterator(cache_root_)) {
            if(fs::is_directory(entry)) {
                LOGT << "Entry: " << entry << " is a directory. Skip." << ELOG;
                continue;
            }

            auto object = entry.path();

            if(object.parent_path() == cache_root_) {
                LOGT << "Entry: " << object << " is the cache root directory. Skip." << ELOG;
                continue;
            }

            if(CacheObject::is_object_properties(object)) {
                LOGT << "Entry: " << object << " is a properties file. Skip." << ELOG;
                continue;
            }

            if(!CacheObject::is_object(object)) {
                LOGT << "Entry: " << object << " is not an object. Skip." << ELOG;
                continue;
            }

            auto properties_path = CacheObject::properties_file(object);
            LOGT << "Object properties path: " << properties_path << ELOG;

            auto artifact_root = properties_path.parent_path().parent_path().string();
            LOGT << "Found artifact root: " << artifact_root << ELOG;

            std::string artifact_metadata_path(artifact_root.begin()+cache_root_.string().size()+1, artifact_root.end());
            LOGT << "Artifact metadata path: " << artifact_metadata_path << ELOG;

            artifacts_paths.insert(std::move(artifact_metadata_path));
        }

        for(auto artifact_path: artifacts_paths) {

            LOGT << "Create non queue cache for artifact path: " << artifact_path << ELOG;

            result.emplace_back(cache_root_, artifact_path);
        }

        return result;
    }

}//namespace piel::cmd
