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

#include <string>
#include <gavcquery.h>

namespace piel::cmd {

    namespace al = art::lib;

    class QueryContext {
    public:
        QueryContext(const std::string& server_api_access_token
             , const std::string& server_url
             , const std::string& server_repository
             , const al::GavcQuery& query)
            : server_api_access_token_(server_api_access_token)
            , server_url_(server_url)
            , server_repository_(server_repository)
            , query_(query)
        {
        }

        std::string token() const {
            return server_api_access_token_;
        }

        std::string server_url() const {
            return server_url_;
        }

        std::string repository() const {
            return server_repository_;
        }

        const al::GavcQuery& query() const {
            return query_;
        }

        std::string get_maven_metadata_path() const {
            return query_.format_maven_metadata_path(server_repository_);
        }

        std::string get_maven_metadata_url() const {
            return query_.format_maven_metadata_url(server_url_, server_repository_);
        }

        std::string get_version_url(const std::string& version) const {
            return query_.format_version_url(server_url_, server_repository_, version);
        }

    private:
        QueryContext() = delete;
        QueryContext(const QueryContext&) = delete;
        QueryContext(QueryContext&&) = delete;
        QueryContext operator=(const QueryContext&) = delete;

        std::string server_api_access_token_;
        std::string server_url_;
        std::string server_repository_;
        al::GavcQuery query_;
    };

}
