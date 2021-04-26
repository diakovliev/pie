/*
 * Copyright (c) 2017, Dmytro Iakovliev daemondzk@gmail.com
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
 * THIS SOFTWARE IS PROVIDED BY Dmytro Iakovliev daemondzk@gmail.com ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Dmytro Iakovliev daemondzk@gmail.com BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <gavccommand.h>
#include <gavccache.h>
#include <logging.h>
#include <mavenmetadata.h>

#include <boost_property_tree_ext.hpp>

#include <commands/base_errors.h>

namespace pie { namespace app {

namespace pt = boost::property_tree;
namespace po = boost::program_options;

GavcCommand::GavcCommand(Application *app, int argc, char **argv)
    : ICommand(app)
    , argc_(argc)
    , argv_(argv)
    , server_url_()
    , server_api_access_token_()
    , server_repository_()
    , query_()
    , have_to_download_results_(false)
    , have_to_delete_results_(false)
    , have_to_delete_versions_(false)
    , output_file_()
    , cache_path_(piel::cmd::utils::get_default_cache_path())
    , disable_cache_(false)
    , notifications_file_()
    , max_attempts_(3)
    , retry_timeout_s_(5)
    , files_list_()
    , force_offline_(false)
{
}

GavcCommand::~GavcCommand()
{
}

void GavcCommand::show_command_help_message(const po::options_description& desc)
{
    std::cerr << "Usage: gavc <gavc query> [options]" << std::endl;
    std::cout << desc;
}

bool GavcCommand::parse_arguments()
{
    po::options_description desc("Query options");
    desc.add_options()
        ("token,t",         po::value<std::string>(&server_api_access_token_),  "Token to access server remote api (required). Can be set using GAVC_SERVER_API_ACCESS_TOKEN environment variable.")
        ("server,s",        po::value<std::string>(&server_url_),               "Server url (required). Can be set using GAVC_SERVER_URL environment variable.")
        ("repository,r",    po::value<std::string>(&server_repository_),        "Server repository (required). Can be set using GAVC_SERVER_REPOSITORY environment variable.")
        ("download,d",                                                          "Download query results.")
        ("delete,x",                                                            "Delete query results from the server (delete files). Have sence only in online mode.")
        ("delete-versions,X",                                                   "Delete query results from the server (delete whole versions). Have sence only in online mode.")
        ("output,o",        po::value<std::string>(&output_file_),              "Output file name. Be careful, it will cause unexpected behavoiur if the query result is set.")
        ("cache-path",      po::value<std::string>(&cache_path_),               (std::string("Cache path. Can be set using GAVC_CACHE environment variable. Default: ") + piel::cmd::utils::get_default_cache_path()).c_str())
        ("disable-cache",                                                       "Do not use local cache (enabled by default).")
        ("notifications,n", po::value<std::string>(&notifications_file_),       "If specified, PIE will generate notifications file with actions details.")
        ("max-attempts",    po::value<unsigned int>(&max_attempts_),            "Max attempts on IO errors.")
        ("retry-timeout",   po::value<unsigned int>(&retry_timeout_s_),         "Retry timeout on IO errors.")
        ("files-list",      po::value<std::string>(&files_list_),               "Generate queued files list.")
        ("force-offline,f",                                                     "Forcing offline mode.")
        ;

    if (show_help(desc, argc_, argv_)) {
        return false;
    }

    // second argument is query
    if (argc_ < 2) {
        show_command_help_message(desc);
        return false;
    }

    std::string query_str(argv_[1]);

    // Parce query
    LOGT << "query to perform: " << query_str << ELOG;

    std::optional<art::lib::GavcQuery> parsed_query = art::lib::GavcQuery::parse(query_str);
    if (!parsed_query)
    {
        std::cerr << "Wrong gavc query: " << query_str << "!" << std::endl;
        show_command_help_message(desc);
        return false;
    }

    query_ = *parsed_query;

    po::variables_map vm;
    po::parsed_options parsed = po::command_line_parser(argc_, argv_).options(desc).allow_unregistered().run();
    po::store(parsed, vm);
    try {
        po::notify(vm);
    } catch (...) {
        show_command_help_message(desc);
        return false;
    }

    bool get_env_flag = true;
    get_env_flag &= get_from_env(vm, "token",       "GAVC_SERVER_API_ACCESS_TOKEN", server_api_access_token_);
    get_env_flag &= get_from_env(vm, "server",      "GAVC_SERVER_URL",              server_url_);
    get_env_flag &= get_from_env(vm, "repository",  "GAVC_SERVER_REPOSITORY",       server_repository_);
                    get_from_env(vm, "cache",       "GAVC_CACHE",                   cache_path_);

    if (!get_env_flag) {
        show_command_help_message(desc);
        return false;
    }

    have_to_download_results_   = vm.count("download");
    have_to_delete_results_     = vm.count("delete");
    have_to_delete_versions_    = vm.count("delete-versions");
    disable_cache_              = vm.count("disable-cache");
    force_offline_              = vm.count("force-offline");

    return true;
}

void GavcCommand::write_files_list(const piel::cmd::GAVC::paths_list& files_list) const {
    if (!files_list_.empty()) {
        std::ofstream fs;
        fs.open(files_list_.c_str(), std::fstream::out|std::fstream::binary);
        std::for_each(files_list.begin(), files_list.end(), [&](auto i) {
            //std::cout << " >> " << i << std::endl;
            fs << i.c_str() << std::endl;
        });
        fs.close();
    }
}

/*virtual*/ int GavcCommand::perform()
{
    int result = -1;

    if (!parse_arguments()) {
        return result;
    }

    try {
        if (disable_cache_) {
            piel::cmd::GAVC gavc(server_api_access_token_,
                             server_url_,
                             server_repository_,
                             query_,
                             have_to_download_results_,
                             output_file_,
                             notifications_file_,
                             max_attempts_,
                             retry_timeout_s_,
                             force_offline_,
                             have_to_delete_results_,
                             have_to_delete_versions_);

            if (output_file_.empty()) {
                gavc.set_path_to_download(std::filesystem::current_path());
            }

            gavc();

            write_files_list(gavc.get_list_of_queued_files());
        }
        else {
            piel::cmd::GAVCCache gavccache(server_api_access_token_,
                             server_url_,
                             server_repository_,
                             query_,
                             have_to_download_results_,
                             cache_path_,
                             output_file_,
                             notifications_file_,
                             max_attempts_,
                             retry_timeout_s_,
                             force_offline_,
                             have_to_delete_results_,
                             have_to_delete_versions_);

            if (output_file_.empty()) {
                gavccache.set_path_to_download(std::filesystem::current_path());
            }

            gavccache();

            write_files_list(gavccache.get_list_of_queued_files());
        }

        result = 0;
    }
    catch (const std::exception& e) {
        std::cerr << piel::cmd::errors::format_exceptions_stack(e) << std::endl;
        return result;
    }

    return result;
}

} } // namespace pie::app
