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

#include <iterator>

#include <std_filesystem_ext.hpp>

#include "IArtifactCache.h"

#include "CacheVersion.h"

namespace piel::cmd {

    namespace fs = std::filesystem;

    struct CacheHelper {
        template<class Version>
        static Version to_Version(const fs::path& path, const IArtifactCache*);
    };


    template<>
    std::string CacheHelper::to_Version<std::string>(const fs::path& path, const IArtifactCache*) {
        return path.filename().string();
    }


    template<>
    fs::path CacheHelper::to_Version<fs::path>(const fs::path& path, const IArtifactCache*) {
        return path;
    }


    template<>
    CacheVersion CacheHelper::to_Version<CacheVersion>(const fs::path& path, const IArtifactCache* parent) {
        return CacheVersion(parent, path.filename().string(), path);
    }


    IArtifactCache::IArtifactCache(fs::path cache_root)
        : cache_root_(std::move(cache_root))
    {
    }


    fs::path IArtifactCache::cache_root() const {
        return cache_root_;
    }


    fs::path IArtifactCache::version_root(const std::string& version) const {
        return artifact_path() / fs::path(version);
    }


    fs::path IArtifactCache::artifact_path() const {
        auto mm_path = metadata_path().string();
        if (fs::path(mm_path).is_absolute()) {
            mm_path = std::string(mm_path.begin()+1, mm_path.end());
        }
        return cache_root() / fs::path(mm_path);
    }


    CacheVersion IArtifactCache::load_version(const std::string& version) const {
        return CacheHelper::to_Version<CacheVersion>(version_root(version), this);
    }


    std::vector<std::string> IArtifactCache::versions() const {
        std::vector<std::string> result;

        auto path = artifact_path();

        if(!fs::is_directory(path)) {
            return result;
        }

        result.reserve(10);

        for(auto& entry: fs::directory_iterator(path)) {
            if(!fs::is_directory(entry))
                continue;

            result.push_back(CacheHelper::to_Version<std::string>(entry.path(), this));
        }

        return result;
    }


    void IArtifactCache::delete_version(const std::string& version) const {
        auto vroot = version_root(version);

        fs::remove_directory_content(vroot);
        fs::remove(vroot);
    }


    std::vector<CacheObject> IArtifactCache::version_objects(CacheObject::Filter objects_filter, const std::string& version) const {
        auto vo = load_version(version);
        return vo.objects(objects_filter);
    }


    std::vector<CacheObject> IArtifactCache::objects(CacheObject::Filter objects_filter) const {
        std::vector<CacheObject> result;
        result.reserve(10);

        for(auto v: versions()) {
            auto vos = version_objects(objects_filter, v);
            result.insert(result.end(),
                    std::make_move_iterator(vos.begin()),
                    std::make_move_iterator(vos.end()));
        }

        return result;
    }

}//namespace piel::cmd
