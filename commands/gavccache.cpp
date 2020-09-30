/*
 * Copyright (c) 2017-2018
 *
 *  Dmytro Iakovliev daemondzk@gmail.com
 *  Oleksii Kogutenko https://github.com/oleksii-kogutenko
 *
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

#include <ctime>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <vector>
#include <gavc.h>
#include <gavccache.h>
#include "gavcconstants.h"
#include <gavcversionsfilter.h>
#include <gavcversionsrangefilter.h>
#include <gavcversionscomparator.h>
#include <artbaseconstants.h>
#include <artbasedownloadhandlers.h>
#include <artgavchandlers.h>
#include <logging.h>
#include <mavenmetadata.h>
#include <artbaseapihandlers.h>
#include <properties.h>

#include <boost_property_tree_ext.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>

namespace al = art::lib;
namespace pl = piel::lib;
namespace fs = std::filesystem;
namespace pt = boost::property_tree;
namespace po = boost::program_options;

namespace piel { namespace cmd {

namespace utils {

std::string get_default_cache_path() {
    static const std::string default_cache_path = "/.pie/gavc/cache";
    static const char *home_c_str = ::getenv("HOME");
    return std::string(home_c_str) + default_cache_path;
}

}//namespace utils

GAVCCache::GAVCCache(const std::string& server_api_access_token
           , const std::string& server_url
           , const std::string& server_repository
           , const art::lib::GavcQuery& query
           , const bool have_to_download_results
           , const std::string &cache_path
           , const std::string& output_file
           , const std::string& notifications_file
           , unsigned int max_attempts
           , unsigned int retry_timeout_s
           , bool force_offline
           , bool have_to_delete_results
           , bool have_to_delete_versions)
    : pl::IOstreamsHolder()
    , server_url_(server_url)
    , server_api_access_token_(server_api_access_token)
    , server_repository_(server_repository)
    , query_(query)
    , path_to_download_()
    , have_to_download_results_(have_to_download_results)
    , have_to_delete_results_(have_to_delete_results)
    , have_to_delete_versions_(have_to_delete_versions)
    , output_file_(output_file)
    , cache_path_(cache_path)
    , max_attempts_(max_attempts)
    , retry_timeout_s_(retry_timeout_s)
    , notifications_file_(notifications_file)
    , force_offline_(force_offline)
    , query_results_()
    , list_of_queued_files_()
    , versions_()
{
}

GAVCCache::~GAVCCache()
{
}

std::vector<std::string> GAVCCache::get_cached_versions(const std::string& path) const
{
    std::vector<std::string> result;

    if(!fs::is_directory(path)) {
        throw piel::cmd::errors::cache_folder_does_not_exist(path);
    }

    for(auto entry : boost::make_iterator_range(fs::directory_iterator(path), {})){
        LOGT << entry.path().filename().c_str() << ELOG;
        if(fs::is_directory(entry)) {
            result.push_back(entry.path().filename().c_str());
        }
    }

    if (result.empty()) {
        LOGT << "Nothing in cache for query." << ELOG;
        return result;
    }

    return query_.filter(result);
}

std::vector<std::string> GAVCCache::get_cached_classifiers_list(const std::string& artifacts_cache) {
    std::vector<std::string> result;

    for(auto entry : boost::make_iterator_range(fs::directory_iterator(artifacts_cache), {})){
        LOGT << entry.path().filename().c_str() << ELOG;
        if(!fs::is_regular_file(entry.path())) {
            continue;
        }

        if (!boost::algorithm::ends_with(entry.path().filename().string(), GAVCConstants::properties_ext)) {
            continue;
        }

        std::ifstream is(entry.path().string());
        pl::Properties props = pl::Properties::load(is);

        std::string classifier = props.get(GAVCConstants::object_classifier_property, "");
        if (classifier.empty())
            continue;

        LOGT << "Cached classifier: " << classifier << ELOG;
        result.push_back(classifier);
    }

    return result;
}

std::string GAVCCache::find_file_for_classifier(const std::string& artifacts_cache, const std::string& classifier)
{
    std::string result;

    for(auto entry : boost::make_iterator_range(fs::directory_iterator(artifacts_cache), {})){
        LOGT << entry.path().filename().c_str() << ELOG;
        if(!fs::is_regular_file(entry.path())) {
            continue;
        }

        if (!boost::algorithm::ends_with(entry.path().filename().string(), GAVCConstants::properties_ext)) {
            continue;
        }

        std::ifstream is(entry.path().string());
        pl::Properties props = pl::Properties::load(is);

        if(props.get(GAVCConstants::object_classifier_property, "") == classifier) {
            result = artifacts_cache + fs::path::preferred_separator + props.get(GAVCConstants::object_id_property, "");
            break;
        }
    }

    return result;
}

/*static*/ std::string GAVCCache::now_string()
{
    std::ostringstream buffer;
    std::time_t tm = std::time(nullptr);
    buffer << std::put_time(std::localtime(&tm), GAVCConstants::last_access_time_format.c_str());
    return buffer.str();
}

/*static*/ void GAVCCache::update_last_access_time(const fs::path& cache_object_path)
{
    pl::Properties props = GAVC::load_object_properties(cache_object_path);
    std::ostringstream buffer;
    buffer << now_string();
    props.set(GAVCConstants::last_access_time_property, buffer.str());
    GAVC::store_object_properties(cache_object_path, props);
}

/*static*/ std::tm GAVCCache::get_last_access_time(const fs::path& cache_object_path)
{
    pl::Properties props = GAVC::load_object_properties(cache_object_path);
    std::tm t = {};
    std::istringstream ss(props.get(GAVCConstants::last_access_time_property, now_string()));
    ss >> std::get_time(&t, GAVCConstants::last_access_time_format.c_str());

    t.tm_isdst = 1;

    return t;
}

GAVC::paths_list GAVCCache::get_cached_files_list(const std::vector<std::string>& versions_to_process, const std::string& path)
{
    std::string classifier_spec = query_.classifier();
    std::string query_name      = query_.name();

    std::vector<std::string> classifiers;

    boost::split(classifiers, classifier_spec, boost::is_any_of(","));
    bool have_errors = false;

    GAVC::paths_list results;

    for (auto ver = versions_to_process.begin(), end = versions_to_process.end(); ver != end; ++ver)
    {
        LOGT << "Version: " << *ver << ELOG;

        std::string artifact_cache_path = path + fs::path::preferred_separator + *ver;

        LOGT << "Version cache: " << artifact_cache_path << ELOG;

        if (classifiers.size() == 1 && classifiers[0].empty()) {
            classifiers = get_cached_classifiers_list(artifact_cache_path);
        }

        for (auto it = classifiers.begin(), cend = classifiers.end(); it != cend; ++it)
        {
            std::vector<std::string> craw_parts;
            boost::split(craw_parts, *it, boost::is_any_of("."));

            std::string c = craw_parts[0];

            if (c.empty()) {
                LOGT << "empty classifier " << ELOG;
                continue;
            }

            std::string file_path               = find_file_for_classifier(artifact_cache_path, c);
            std::string classifier_file_name    = fs::path(file_path).filename().string();

            LOGT << "Classifier: " << c << " path: " << file_path << ELOG;

            if (!fs::is_regular_file(file_path)) {
                LOGT << "No file: " << file_path << " for classifier: " << c << ELOG;

                have_errors = true;
                break;
            }

            pl::Properties props    = GAVC::load_object_properties(file_path);
            bool is_valid           = GAVC::validate_local_file(file_path, props);

            if (!is_valid) {
                LOGT << "File: " << file_path  << " for classifier: " << c << " is not valid!" << ELOG;

                have_errors = true;
                break;
            }

            if (std::find(results.begin(), results.end(), file_path) == results.end()) {
                results.push_back(file_path);
            }
        }
    }

    if (have_errors)
        return GAVC::paths_list();

    return results;
}

/*static*/ std::string GAVCCache::cache_properties_file(const std::string& cache_path)
{
    return cache_path + fs::path::preferred_separator + GAVCConstants::cache_properties_filename;
}

/*static*/ void GAVCCache::init(const std::string& cache_path)
{
    if (!fs::is_directory(cache_path) || !fs::exists(cache_path)) {
        LOGT << "Try to remove existing file which name is conflict with cache path! cache path: " << cache_path << ELOG;
        try {
            fs::remove_all(cache_path);
        } catch (...) {
            LOGE << "Unable to remove existing file which name is conflict with cache path! cache path: " << cache_path << ELOG;
            throw errors::cache_folder_does_not_exist(cache_path);
        }
    }

    if (!fs::exists(cache_path)) {
        LOGT << "Try to create cache directory at cache path: " << cache_path << ELOG;
        try {
            fs::create_directories(cache_path);
        } catch (...) {
            LOGE << "Unable to create cache directory at cache path: " << cache_path << ELOG;
            throw errors::cache_folder_does_not_exist(cache_path);
        }
    }

    LOGT << "Create cache properties..." << ELOG;
    pl::Properties props = pl::Properties();

    LOGT << "Set cache version to: " << GAVCConstants::cache_version;
    props.set(GAVCConstants::cache_version_property, GAVCConstants::cache_version);

    auto cache_properties_filepath = cache_properties_file(cache_path);

    LOGT << "Write cache properties file: " << cache_properties_filepath;
    std::ofstream os(cache_properties_filepath);
    props.store(os);
}

/*static*/ bool GAVCCache::validate(const std::string& cache_path)
{
    LOGT << "Try to read cache properties..." << ELOG;

    std::string properties_file = cache_properties_file(cache_path);

    if (!fs::is_regular_file(properties_file)) {
        LOGT << "No cache properties file! Cache is invalid!" << ELOG;
        return false;
    }

    std::ifstream is(properties_file);
    pl::Properties props = pl::Properties::load(is);

    std::string properties_version = props.get(GAVCConstants::cache_version_property, "");

    bool ret = GAVCConstants::cache_version == properties_version;

    LOGT
        << "Validate cache version. Expected version: "     << GAVCConstants::cache_version
        << " version from properties: "                     << properties_version
        << " cache version validation result: "             << ret
        << ELOG;

    return ret;
}

bool GAVCCache::is_force_offline() const
{
    bool offline            = force_offline_;
    if (!offline) {
        const char *offline_str = ::getenv("PIE_GAVC_FORCE_OFFLINE");
        if (offline_str) {
            offline = std::string(offline_str) == "1" || std::string(offline_str) == "true";
        }
    }
    LOGT << "Force offline mode: " << offline << ELOG;
    return offline;
}

void GAVCCache::perform()
{
    if (!validate(cache_path_)) {
        try {
            fs::remove_all(cache_path_);
        } catch (...) {}
        init(cache_path_);
    }

    piel::cmd::GAVC gavc(
         server_api_access_token_,
         server_url_,
         server_repository_,
         query_,
         have_to_download_results_,
         "",
         notifications_file_,
         max_attempts_,
         retry_timeout_s_,
         false,
         have_to_delete_results_,
         have_to_delete_versions_);

    gavc.setup_iostreams(&cout(), &cerr(), &cin());
    gavc.set_cache_mode(true);

    LOGT << " gavc.get_maven_metadata_path:"    << gavc.get_maven_metadata_path()   << ELOG;

    std::string mm_path = cache_path_ + gavc.get_maven_metadata_path();

    LOGT << " cache_path_:"                     << cache_path_                      << ELOG;
    LOGT << " mm_path:"                         << mm_path                          << ELOG;

    std::vector<std::string> versions_to_process_remote;
    std::vector<std::string> versions_to_process_cache;
    std::vector<std::string> versions_to_process;

    bool force_offline      = is_force_offline();
    bool offline            = force_offline;
    bool have_cached_files  = false;

    GAVC::paths_list cached_files_list;

    if (!offline) {
        try {
            versions_to_process_remote  = gavc.get_versions_to_process();
        }
        catch (piel::cmd::errors::no_server_maven_metadata& e) {
            // No server metadata. Force use cache.
            offline                     = true;
        }
    }

    LOGT << "force_offline: " << force_offline << " offline: " << offline << ELOG;

    if (offline) {

        try {
            versions_to_process_cache   = query_.filter(get_cached_versions(mm_path));
            // Initial cache files list
            cached_files_list           = get_cached_files_list(versions_to_process_cache, mm_path);
            have_cached_files           = !cached_files_list.empty();

            LOGT << "have_cached_files: " << have_cached_files << ELOG;
        } catch (errors::cache_folder_does_not_exist& e) {
            // Don't see anything in cache.
        }

        for (auto ver = versions_to_process_cache.begin(), end = versions_to_process_cache.end(); ver != end; ++ver) {
            cout() << "Version: " << *ver << std::endl;
            if (force_offline) {
                cout() << "Mode: force offline"     << std::endl;
            } else {
                cout() << "Mode: offline"           << std::endl;
            }

            gavc.notify_gavc_version(*ver);
        }

        for (GAVC::paths_list::iterator f = cached_files_list.begin(), end = cached_files_list.end(); f != end; ++f) {

            fs::path object_name        = fs::path(*f).filename();

            LOGT << *f << "->" << object_name << ELOG;

            cout() << "c " << object_name.string() << std::endl;
        }

        versions_to_process = versions_to_process_cache;

    } else {

        for (auto i = versions_to_process_remote.begin(), end = versions_to_process_remote.end(); i != end; ++i) {
            LOGT << "Version: " << *i << ELOG;

            std::string path = mm_path + "/" + *i;

            gavc.set_path_to_download(path);
            gavc.process_version(*i);

            // Get actial files list
            cached_files_list = gavc.get_list_of_actual_files();
            have_cached_files = !cached_files_list.empty();
        }

        versions_to_process = versions_to_process_remote;
    }

    if (versions_to_process.empty() && !have_cached_files) {
        LOGT << "Raise exception!" << ELOG;
        throw errors::cant_find_version_for_query();
    }

    versions_ = gavc.get_versions();

    if (have_to_download_results_) {

        if (offline && !have_cached_files) {
            throw errors::cache_no_cache_for_query(query_.to_string());
        }

        for (GAVC::paths_list::iterator f = cached_files_list.begin(), end = cached_files_list.end(); f != end; ++f) {

            fs::path path        = output_file_.empty()         ?   fs::path(*f).filename().string()    : output_file_                          ;
            fs::path object_path = path_to_download_.empty()    ?   path                                : path_to_download_ / path.filename()   ;

            LOGT << *f << "->" << object_path << ELOG;

            fs::copy_file(*f, object_path, fs::copy_options::overwrite_existing);
            update_last_access_time(*f);

            cout() << "+ " << object_path.filename().string() << std::endl;

            pl::Properties props = GAVC::load_object_properties(*f);

            std::string object_classifier = props.get(GAVCConstants::object_classifier_property, "");
            std::string version = fs::path(*f).parent_path().filename().string();

            query_results_.insert(std::make_pair(object_path, std::make_pair(object_classifier, version)));
            list_of_queued_files_.push_back(object_path.string());

            if (have_to_delete_results_) {
                cout() << "cx " << object_path.filename().string() << std::endl;
                // TODO:
            }
        }
    }

    if (have_to_delete_versions_) {
        // TODO:
    }
}

void GAVCCache::operator()()
{
    unsigned int        attempt = 0;
    const unsigned int  max_attempts  = std::max(1u, max_attempts_);
    const unsigned int  retry_timeout = std::max(5u, retry_timeout_s_);
    while(true) {
        try {
            LOGT << "GAVCCache query attempt: " << attempt << " from: " << max_attempts << " start." << ELOG;
            perform();
            LOGT << "GAVCCache query attempt: " << attempt << " success." <<  ELOG;
            break;
        } catch (errors::cache_folder_does_not_exist&) {
            break;
        } catch (errors::cant_find_version_for_query&) {
            throw;
        } catch (...) {
            LOGT << "GAVCCache query attempt: " << attempt << " from: " << max_attempts << " failed." <<  ELOG;
            if (attempt++ >= max_attempts) {
                LOGT << "GAVCCache rethrow." << ELOG;
                throw;
            } else {
                LOGT << "GAVCCache sleep: " << retry_timeout << "s." <<  ELOG;
                sleep(retry_timeout);
            }
        }
    }
}

void GAVCCache::set_path_to_download(const std::filesystem::path& path)
{
    path_to_download_ = path;
}

std::filesystem::path GAVCCache::get_path_to_download() const
{
    return path_to_download_;
}

GAVC::query_results GAVCCache::get_query_results() const
{
    return query_results_;
}

GAVC::paths_list GAVCCache::get_list_of_queued_files() const
{
    return list_of_queued_files_;
}

std::vector<std::string> GAVCCache::get_versions() const
{
    return versions_;
}

} } // namespace piel::cmd
