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

#pragma once

#include <filesystem>

#include "CacheObject.h"

namespace piel::cmd {

    namespace fs = std::filesystem;

    class IArtifactCache {
    public:
        explicit IArtifactCache(fs::path cache_root);
        IArtifactCache(const IArtifactCache&) = default;

        virtual ~IArtifactCache() = default;

        fs::path cache_root() const;

        virtual std::vector<std::string> versions() const;

        void delete_version(const std::string& version) const;

        CacheVersion load_version(const std::string& version) const;

        std::vector<CacheObject> objects(CacheObject::Filter objects_filter = [](const auto&){return true;}) const;

    protected:
        virtual fs::path metadata_path() const = 0;

        fs::path artifact_path() const;

        std::vector<CacheObject> version_objects(CacheObject::Filter objects_filter, const std::string& version) const;
        fs::path version_root(const std::string& version) const;

    private:
        fs::path cache_root_;

    };

}//namespace piel::cmd
