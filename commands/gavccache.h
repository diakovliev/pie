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

#ifndef GAVCCACHE_H_
#define GAVCCACHE_H_

#include <gavc.h>

namespace piel { namespace cmd {

namespace errors {
    struct cache_folder_does_not_exist {
        cache_folder_does_not_exist(const std::string& folder)
            : folder(folder)
        {}
        std::string folder;
    };
    struct cache_no_cache_for_query {
        cache_no_cache_for_query(const std::string& query)
            : query(query)
        {}
        std::string query;
    };
    struct cache_no_file_for_classifier {
        cache_no_file_for_classifier(const std::string& classifier)
            : classifier(classifier)
        {}
        std::string classifier;
    };
    struct cache_not_valid_file {
        cache_not_valid_file(const std::string& classifier)
            : classifier(classifier)
        {}
        std::string classifier;
    };
}

class GAVCCache: public piel::lib::IOstreamsHolder
{
public:
    GAVCCache(  const std::string& server_api_access_token
         , const std::string& server_url
         , const std::string& server_repository
         , const art::lib::GavcQuery& query
         , const bool have_to_download_results
         , const std::string& cache_path = std::string()
         , const std::string& output_file = std::string()
         , const std::string& notifications_file = std::string()
         , unsigned int max_attempts = 3
         , unsigned int retry_timeout_s = 5
         , bool force_offline = false
         , bool have_to_delete_resuts = false);

    virtual ~GAVCCache();

    void operator()();

    void set_path_to_download(const boost::filesystem::path& path);
    boost::filesystem::path get_path_to_download() const;

    std::vector<std::string> get_cached_versions(const std::string &path) const;
    GAVC::paths_list get_cached_files_list(const std::vector<std::string> &versions_to_process, const std::string &path);
    void copy_file_list(GAVC::paths_list &file_list);

    std::vector<std::string> get_cached_classifiers_list(const std::string& artifacts_cache);
    std::string find_file_for_classifier(const std::string& artifacts_cache, const std::string& classifier);

    static void update_last_access_time(const boost::filesystem::path& cache_object_path);
    static std::tm get_last_access_time(const boost::filesystem::path& cache_object_path);

    static void init(const std::string& cache_path);
    static bool validate(const std::string& cache_path);

    GAVC::query_results get_query_results() const;
    GAVC::paths_list get_list_of_queued_files() const;

protected:
    void perform();
    static std::string now_string();
    bool is_force_offline() const;
    static std::string cache_properties_file(const std::string& cache_path);

private:
    std::string server_url_;
    std::string server_api_access_token_;
    std::string server_repository_;
    art::lib::GavcQuery query_;
    boost::filesystem::path path_to_download_;
    bool have_to_download_results_;
    bool have_to_delete_results_;
    std::string output_file_;
    std::string cache_path_;
    unsigned int max_attempts_;
    unsigned int retry_timeout_s_;
    std::string notifications_file_;
    bool force_offline_;

    GAVC::query_results query_results_;
    GAVC::paths_list list_of_queued_files_;
};

} } // namespace piel::cmd

#endif /* GAVC_H_ */
