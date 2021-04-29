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

    struct QueryOperationParameters {
        QueryOperationParameters() = default;
        virtual ~QueryOperationParameters() = default;

    private:
        QueryOperationParameters(const QueryOperationParameters&) = delete;
        QueryOperationParameters(QueryOperationParameters&&) = delete;
        QueryOperationParameters operator=(const QueryOperationParameters&) = delete;
    };

    class DownloadOperationParameters: public QueryOperationParameters {
    public:
        DownloadOperationParameters(const bool have_to_download_results
             , bool have_to_delete_results
             , bool have_to_delete_versions
             , unsigned int max_attempts
             , unsigned int retry_timeout_s)
            : QueryOperationParameters()
            , have_to_download_results_(have_to_download_results)
            , have_to_delete_results_(have_to_delete_results)
            , have_to_delete_versions_(have_to_delete_versions)
            , max_attempts_(max_attempts)
            , retry_timeout_s_(retry_timeout_s)
        {
        }
        virtual ~DownloadOperationParameters() = default;

        bool have_to_download_results() const {
            return have_to_download_results_;
        }

        bool have_to_delete_results() const {
            return have_to_delete_results_;
        }

        bool have_to_delete_versions() const {
            return have_to_delete_versions_;
        }

        unsigned int max_attempts() const {
            return max_attempts_;
        }

        unsigned int retry_timeout_s() const {
            return retry_timeout_s_;
        }

    private:
        DownloadOperationParameters() = delete;
        DownloadOperationParameters(const DownloadOperationParameters&) = delete;
        DownloadOperationParameters(DownloadOperationParameters&&) = delete;
        DownloadOperationParameters operator=(const DownloadOperationParameters&) = delete;

        bool have_to_download_results_;
        bool have_to_delete_results_;
        bool have_to_delete_versions_;

        unsigned int max_attempts_;
        unsigned int retry_timeout_s_;
    };


    class QueryOperation {
    public:
        QueryOperation(const QueryContext *context, const QueryOperationParameters* params)
            : context_(context)
            , params_(params)
        {
        }
        virtual ~QueryOperation() = default;

        const QueryContext* context() const {
            return context_;
        }

        const QueryOperationParameters* get_params() const {
            return params_;
        }

        template<class ParamsType = DownloadOperationParameters>
        const ParamsType* params() const {
            return static_cast<const ParamsType*>(get_params());
        }

    private:
        QueryOperation() = delete;
        QueryOperation(const QueryOperation&) = delete;
        QueryOperation(QueryOperation&&) = delete;
        QueryOperation operator=(const QueryOperation&) = delete;

        const QueryContext*             context_;
        const QueryOperationParameters* params_;
    };

}
