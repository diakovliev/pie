/*
 * Copyright (c) 2018, diakovliev
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

#include "gavc.h"

#include <commands/base_errors.h>

#include "GavcUtils.h"

namespace piel::cmd {

    class GAVCCache: public QueryOperation, public piel::lib::IOstreamsHolder
    {
    public:
        GAVCCache( const QueryContext *context
             , const QueryOperationParameters *params
             , const std::string& cache_path = std::string()
             , const std::string& output_file = std::string()
             , const std::string& notifications_file = std::string()
             , bool force_offline = false);

        virtual ~GAVCCache() = default;

        void operator()();

        void set_path_to_download(const std::filesystem::path& path);
        std::filesystem::path get_path_to_download() const;

        void copy_file_list(GAVC::paths_list &file_list);
        std::vector<std::string> get_versions() const;

        GAVC::query_results get_query_results() const;
        GAVC::paths_list get_list_of_queued_files() const;

    protected:
        void perform();
        static std::string now_string();
        bool is_force_offline() const;

    private:
        std::filesystem::path path_to_download_;
        std::string output_file_;
        std::string cache_path_;
        std::string notifications_file_;
        bool force_offline_;

        GAVC::query_results query_results_;
        GAVC::paths_list list_of_queued_files_;

        std::vector<std::string> versions_;
    };

} // namespace piel::cmd
