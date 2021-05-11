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

#include "GavcUtils.h"

#include <fstream>

#include <cassert>
#include <logging.h>
#include <artbaseconstants.h>
#include <checksumsdigestbuilder.hpp>

namespace utils::gavc {

    namespace al = ::art::lib;


    std::vector<std::string> parse_classifiers_spec(const std::string& classifiers_spec) {
        LOGT << "Parse classifiers spec: " << classifiers_spec << ELOG;

        auto result = ::utils::string::split2vec(',', classifiers_spec);
        if(result.size() == 0) {
            result.emplace_back();
        }

        LOGT << "Classifiers count: " << result.size() << ELOG;

        return result;
    }


    std::string classifier_significant_part(const std::string& classifier) {
        return ::utils::string::split('.', classifier).first;
    }


    std::string classifier_from_object_id(const std::string& server_object_id) {

        std::string object_classifier;

        auto server_object_id_parts = ::utils::string::split2vec('-', server_object_id);

        if (server_object_id_parts.size() > 2)
        {
            object_classifier = classifier_significant_part(server_object_id_parts[server_object_id_parts.size()-1]);
        }

        return object_classifier;
    }


    bool validate_local_file(const fs::path& object_path, const pl::Properties& server_checksums)
    {
        bool local_file_is_actual = fs::exists(object_path);
        if(!local_file_is_actual)
            return local_file_is_actual;

        std::ifstream is(object_path.generic_string());

        auto str_digests = pl::ChecksumsDigestBuilder().str_digests_for(is);

        auto verify_sum = [&](auto remote_sum_name, auto local_sum_name) {
            if (server_checksums.contains(remote_sum_name))
            {
                LOGT << remote_sum_name
                     << " from server: "
                     << server_checksums.get(remote_sum_name, "")
                     << " local: "
                     << str_digests[local_sum_name]
                     << ELOG;

                local_file_is_actual &= server_checksums.get(remote_sum_name, "") == str_digests[local_sum_name];
            }
        };

        verify_sum(al::ArtBaseConstants::checksums_sha256,  pl::Sha256::t::name());
        verify_sum(al::ArtBaseConstants::checksums_sha1,    pl::Sha::t::name());
        verify_sum(al::ArtBaseConstants::checksums_md5,     pl::Md5::t::name());

        return local_file_is_actual;
    }


    std::string get_default_cache_path() {
        static const std::string default_cache_path = "/.pie/gavc/cache";
        static const char *home_c_str = ::getenv("HOME");
        return std::string(home_c_str) + default_cache_path;
    }

} // utils::gavc
