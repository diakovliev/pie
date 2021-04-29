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

#include "QueryCache.h"

namespace piel::cmd {

    namespace fs = std::filesystem;


    QueryCache::QueryCache(const QueryContext* context, fs::path cache_root, GAVC* gavc)
        : IArtifactCache(cache_root)
        , context_(context)
        , gavc_(gavc)
    {
        if (gavc_) {
            gavc_->set_cache_mode(true, [this](const auto& version, const auto& object_path, const auto& props){
                CacheObject::write_object_properties(object_path, props);
            });
        }
    }


    const QueryContext *QueryCache::context() const {
        return context_;
    }


    std::vector<std::string> QueryCache::versions() const {
        auto result = context()->query().filtered(IArtifactCache::versions());
        if (gavc_) {
            for(auto version: result) {
                gavc_->notify_gavc_version(version);
            }
        }
        return result;
    }


    /*virtual*/ fs::path QueryCache::metadata_path() const {
        return context_->get_maven_metadata_path();
    }


    std::vector<CacheObject> QueryCache::cache_version_objects(const std::string& version) const {
        std::vector<CacheObject> result;
        result.reserve(10);

        if (!gavc_) {
            return version_objects([](const auto&){return true;}, version);
        }

        gavc_->set_path_to_download(version_root(version));
        gavc_->process_version(version);

        auto actual_files = gavc_->get_list_of_actual_files();

        return version_objects([&actual_files](const auto& o)->bool{
            return actual_files.end() != std::find(actual_files.begin(), actual_files.end(), o.object());
        }, version);
    }


    std::vector<std::string> QueryCache::get_gavc_versions() const {
        if (!gavc_)
            return std::vector<std::string>();

        return gavc_->get_versions();
    }

} // namespace piel::cmd
