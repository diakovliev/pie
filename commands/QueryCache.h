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

#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include "IArtifactCache.h"
#include "gavc.h"
#include "QueryContext.h"
#include "CacheVersion.h"

namespace piel::cmd {

    namespace fs = std::filesystem;
    namespace al = art::lib;

    class QueryCache: public IArtifactCache {
    public:
        QueryCache(const QueryContext* context, fs::path cache_root, GAVC* gavc);
        QueryCache(const QueryCache&) = default;
        virtual ~QueryCache() = default;

        const QueryContext *context() const;

        virtual std::vector<std::string> versions() const;
        std::vector<CacheObject> cache_version_objects(const std::string& version) const;

        std::vector<std::string> get_gavc_versions() const;

        virtual fs::path metadata_path() const;

    private:
        const QueryContext *context_;
        GAVC *gavc_;

    };

} // namespace piel::cmd
