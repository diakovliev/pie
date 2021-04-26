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

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <functional>
#include <optional>
#include <algorithm>

#include <gavc.h>
#include "gavcconstants.h"
#include <artbaseconstants.h>
#include <artbasedownloadhandlers.h>
#include <artgavchandlers.h>
#include <artaqlhandlers.h>
#include <logging.h>
#include <mavenmetadata.h>
#include <artbaseapihandlers.h>

#include <commands/SleepFor.hpp>
#include <commands/Retrier.hpp>

#include <boost_property_tree_ext.hpp>
#include <boost/algorithm/string.hpp>

namespace al = art::lib;
namespace pl = piel::lib;
namespace fs = std::filesystem;
namespace pt = boost::property_tree;
namespace po = boost::program_options;

namespace piel { namespace cmd {

GAVC::GAVC(const std::string& server_api_access_token
           , const std::string& server_url
           , const std::string& server_repository
           , const art::lib::GavcQuery& query
           , const bool have_to_download_results
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
    , cache_mode_(false)
    , object_properties_handler_()
    , list_of_actual_files_()
    , query_results_()
    , list_of_queued_files_()
    , versions_()
    , output_file_(output_file)
    , max_attempts_(max_attempts)
    , retry_timeout_s_(retry_timeout_s)
    , notifications_file_(notifications_file)
    , force_offline_(force_offline)
{
}

GAVC::~GAVC()
{
}

struct BeforeOutputCallback: public al::ArtBaseApiHandlers::IBeforeCallback
{
    BeforeOutputCallback(const fs::path& object_path): dest_(), object_path_(object_path) {}
    virtual ~BeforeOutputCallback() {}

    virtual bool callback(al::ArtBaseApiHandlers *handlers)
    {
        LOGT << "Output path: " << object_path_.generic_string() << ELOG;

        dest_ = std::make_shared<std::ofstream>(object_path_.generic_string().c_str());

        dynamic_cast<al::ArtBaseDownloadHandlers*>(handlers)->set_destination(dest_.get());

        return true;
    }
private:
    std::shared_ptr<std::ofstream> dest_;
    fs::path object_path_;
};

struct OnBufferCallback
{
    OnBufferCallback(const GAVC *parent)
        : parent_(parent)
        , index_(0)
        , total_(0)
        , last_event_time_(0)
    {
    }

    void operator()(const al::ArtBaseDownloadHandlers::BufferInfo& bi)
    {
        static const char progress_ch[4] = { '-', '\\', '|', '/' };

        time_t curr_event_time_ = time(0);

        id_     = bi.id;
        total_ += bi.size;

        if (curr_event_time_ - last_event_time_ > 1)
        {
            parent_->cout() << progress_ch[index_] << " " << id_ << "\r";
            parent_->cout().flush();
            index_ = (index_ + 1) % sizeof(progress_ch);

            last_event_time_ = curr_event_time_;
        }
    }

private:
    const GAVC *parent_;
    unsigned char index_;
    size_t total_;
    std::string id_;
    time_t last_event_time_;
};

std::map<std::string,std::string> GAVC::get_server_checksums(const pt::ptree& obj_tree, const std::string& section) const
{
    std::map<std::string,std::string> result;

    auto checksums = obj_tree.get_child(section);

    auto get_sum = [&](auto sum_name) {
        auto op_sum = pt::find_value(checksums, pt::FindPropertyHelper(sum_name));
        if (op_sum)
        {
            LOGT << section << " " << sum_name << ":" << *op_sum << ELOG;
            result[sum_name] = *op_sum;
        }
    };

    get_sum(al::ArtBaseConstants::checksums_sha1);
    get_sum(al::ArtBaseConstants::checksums_sha256);
    get_sum(al::ArtBaseConstants::checksums_md5);

    return result;
}

/*static*/ bool GAVC::validate_local_file(const fs::path& object_path, const pl::Properties& server_checksums)
{
    bool local_file_is_actual = fs::exists(object_path);

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

void GAVC::download_file(const fs::path& object_path, const std::string& object_id, const std::string& download_uri) const
{
    LOGT << "Download file: " << object_path << " id: " << object_id << " uri: " << download_uri << ELOG;

    al::ArtBaseDownloadHandlers download_handlers(server_api_access_token_);

    BeforeOutputCallback before_output(object_path);
    download_handlers.set_id(object_id);
    download_handlers.set_before_output_callback(&before_output);

    pl::CurlEasyClient<al::ArtBaseDownloadHandlers> download_client(download_uri, &download_handlers);

    OnBufferCallback on_buffer(this);
    download_handlers.connect(on_buffer);

    try {
        download_client.perform(true);
    } catch (...) {
        std::throw_with_nested(std::runtime_error("Error on download file: " + object_path.generic_string() + " id: " + object_id + " uri: " + download_uri));
    }
}

void GAVC::delete_file(const std::string& delete_uri) const
{
    LOGT << "Delete remote file uri: " << delete_uri << ELOG;

    al::ArtGavcHandlers delete_handlers(server_api_access_token_);

    pl::CurlEasyClient<al::ArtGavcHandlers> delete_client(delete_uri, &delete_handlers, al::ArtBaseConstants::rest_api__delete_request);

    try {
        delete_client.perform(true);
    } catch (...) {
        std::throw_with_nested(std::runtime_error("Error on delete remote file uri: " + delete_uri));
    }
}

void GAVC::on_object(const pt::ptree::value_type& obj, const std::string& version, const std::string& query_classifier)
{
    LOGT << "on_object version: " << version << " query classifier: " << query_classifier << ELOG;

    auto op_download_uri = pt::find_value(obj.second, pt::FindPropertyHelper("downloadUri"));
    if (!op_download_uri)
    {
        LOGF << "Can't find downloadUri property!" << ELOG;
        return;
    }

    auto op_path = pt::find_value(obj.second, pt::FindPropertyHelper("path"));
    if (!op_path)
    {
        LOGF << "Can't find path property!" << ELOG;
        return;
    }

    fs::path path        = output_file_.empty()         ?   *op_path     : output_file_;
    fs::path object_path = path_to_download_.empty()    ?   path         : path_to_download_ / path.filename()   ;

    LOGT << "object path: "     << object_path                  << ELOG;

    std::string object_id       = object_path.filename().string();

    LOGT << "object id: "       << object_id                    << ELOG;

    std::string object_classifier = GAVCConstants::empty_classifier;
    LOGD << "Extract classifier from the object id." << ELOG;

    std::vector<std::string> query_classifier_parts;
    boost::split(query_classifier_parts, query_classifier, boost::is_any_of("."));

    std::vector<std::string> object_id_parts;
    boost::split(object_id_parts, object_id, boost::is_any_of("-"));

    if (object_id_parts.size() > 2)
    {
        object_classifier = object_id_parts[object_id_parts.size()-1];
        if (object_classifier.find('.') != std::string::npos)
        {
            object_classifier = object_classifier.substr(0, object_classifier.find('.'));
        }
    }

    LOGT << "object classifier: " << object_classifier << ELOG;
    LOGT << "query classifier: " << query_classifier_parts[0] << ELOG;

    if (object_classifier != GAVCConstants::empty_classifier &&
        query_classifier  != GAVCConstants::empty_classifier &&
        !query_classifier_parts[0].empty() &&
        object_classifier != query_classifier_parts[0]) {
        LOGD << "Skip object because query classifier: "    << query_classifier_parts[0]
             << " is not equal to object classifier: "      << object_classifier << ELOG;
        return;
    }

    auto server_checksums      = get_server_checksums(obj.second, "checksums");
    //auto original_checksums    = get_server_checksums(obj.second, "originalChecksums");

    cout() << "? " << object_id << "\r";
    cout().flush();

    bool local_file_is_actual   = (cache_mode_) ? validate_local_file(object_path, pl::Properties::from_map(server_checksums)) : false;
    bool do_download            = !local_file_is_actual;

    if (have_to_download_results_ && do_download)
    {
        cout() << "- " << object_id << "\r";
        cout().flush();

        if (!object_path.parent_path().string().empty()) {
            fs::create_directories(object_path.parent_path());
        }

        download_file(object_path, object_id, *op_download_uri);

        list_of_actual_files_.push_back(object_path);

        if (cache_mode_) {
            auto props = pl::Properties::from_map(server_checksums);

            props.set(GAVCConstants::object_id_property, object_id);
            props.set(GAVCConstants::object_classifier_property, object_classifier);

            if (object_properties_handler_) {
                object_properties_handler_(object_path, props);
            }

            cout() << "c " << object_id << std::endl;
        } else {
            cout() << "+ " << object_id << std::endl;
        }
    }
    else if (local_file_is_actual)
    {
        list_of_actual_files_.push_back(object_path);

        if (cache_mode_) {
            cout() << "c " << object_id << std::endl;
        } else {
            cout() << "+ " << object_id << std::endl;
        }
    }
    else
    {
        cout() << "- " << object_id << std::endl;
    }

    if (have_to_delete_results_)
    {
        delete_file(*op_download_uri);

        cout() << "x " << *op_download_uri << std::endl;
    }

    LOGT << "Add query result. { object path: " << object_path
            << " classifier: " << object_classifier
            << " version: " << version << " }" << ELOG;

    query_results_.insert(std::make_pair(object_path, std::make_pair(object_classifier, version)));
    list_of_queued_files_.push_back(object_path.string());
}

void GAVC::on_aql_object(const pt::ptree::value_type& obj, const std::string& version, const std::string& query_classifier)
{
    pt::ptree item;
    pt::ptree checksums;
    pt::ptree originalChecksums;

    auto get_req_value = [&](auto name) {
        auto value = pt::find_value(obj.second, pt::FindPropertyHelper(name));
        if (!value) {
            LOGF << "Can't find " << name << " property in aql results!" << ELOG;
        }

        return value;
    };

    auto get_sum = [&](auto sum_name) {
        auto op_sum = pt::find_value(obj.second, pt::FindPropertyHelper(sum_name));
        if (op_sum) {
            auto sum = *op_sum;
            LOGT << "aql " << sum_name + ": " << sum << ELOG;
            checksums.put(sum_name, sum);
            originalChecksums.put(sum_name, sum);
        }
    };


    auto repo = get_req_value(al::ArtBaseConstants::aql_repo);
    auto name = get_req_value(al::ArtBaseConstants::aql_name);
    auto path = get_req_value(al::ArtBaseConstants::aql_path);

    std::string path2 = "/" + *path + "/" + *name;
    LOGT << "aql path2: " << path2 << ELOG;

    std::string downloadUri = server_url_ + "/" + *repo + path2;
    LOGT << "aql downloadUri: " << downloadUri << ELOG;

    std::string uri = server_url_ + "/api/storage/" + *repo + path2;
    LOGT << "aql uri: " << uri << ELOG;

    item.put("repo", *repo);
    item.put("path", path2);
    item.put("downloadUri", downloadUri);
    item.put("uri", uri);

    get_sum(al::ArtBaseConstants::aql_md5);
    get_sum(al::ArtBaseConstants::aql_sha1);
    get_sum(al::ArtBaseConstants::aql_sha256);

    item.add_child("checksums", checksums);
    item.add_child("originalChecksums", originalChecksums);

    on_object(std::make_pair("", item), version, query_classifier);
}

std::string GAVC::get_maven_metadata_path() const
{
    return query_.format_maven_metadata_path(server_repository_);
}

std::vector<std::string> GAVC::get_versions_to_process() const
{
    // Get maven metadata
    al::ArtGavcHandlers download_metadata_handlers(server_api_access_token_);
    pl::CurlEasyClient<al::ArtGavcHandlers> get_metadata_client(
        query_.format_maven_metadata_url(server_url_, server_repository_), &download_metadata_handlers);

    try {
    get_metadata_client.perform(true);
    } catch (...) {
        std::throw_with_nested(std::runtime_error("Error on getting maven metadata!"));
    }

    // Try to parse server response.
    std::optional<al::MavenMetadata> metadata_op = std::nullopt;
    try
    {
        metadata_op = al::MavenMetadata::parse(download_metadata_handlers.responce_stream());
    }
    catch (...)
    {
        std::throw_with_nested(std::runtime_error("Parsing maven metadata error!"));
    }

    if (!metadata_op)
    {
        throw std::runtime_error("No maven metadata to process!");
    }

    auto metadata = *metadata_op;

    return metadata.versions_for(query_);
}

void GAVC::process_version(const std::string& version)
{
    LOGT << "Version: " << version << ELOG;

    GAVC::notify_gavc_version(version);

    cout() << "Version: "       << version  << std::endl;
    cout() << "Mode: online"                << std::endl;

    std::string classifier_spec = query_.classifier();

    LOGT << "Classifiers spec: " << classifier_spec << ELOG;

    std::vector<std::string> classifiers;

    boost::split(classifiers, classifier_spec, boost::is_any_of(","));

    std::for_each(classifiers.begin(), classifiers.end(), [&] (auto& classifier) {
        LOGT << "Classifier: " << classifier << ELOG;

        al::GavcQuery q = query_;
        q.set_version(version);
        q.set_classifier(classifier);

        al::ArtAqlHandlers aql_handlers(server_api_access_token_, q.to_aql(server_repository_));
        aql_handlers.set_url(server_url_);

        pl::CurlEasyClient<art::lib::ArtAqlHandlers> client(aql_handlers.gen_uri(), &aql_handlers, aql_handlers.method());
        try {
            client.perform(true);
        } catch (...) {
            std::throw_with_nested(std::runtime_error("Unable to get info associated with version: " + version + "!"));
        }

        pt::ptree root;
        pt::read_json(aql_handlers.responce_stream(), root);

        pt::each(root.get_child("results"),
            [&](auto& result) {
                on_aql_object(result, version, classifier);
            },
            [&]() {
                if (!classifier.empty())
                {
                    throw std::runtime_error("No requested classifier: " + classifier + " in artifact with version: " + version + "!");
                }
            }
        );

    });

    if (have_to_delete_versions_) {
        LOGT << "Delete version: " << version << ELOG;
        cout() << "X " << version << std::endl;

        delete_file(query_.format_version_url(server_url_, server_repository_, version));
    }
}

void GAVC::process_versions(const std::vector<std::string>& versions_to_process)
{
    std::for_each(versions_to_process.begin(), versions_to_process.end(), [this](auto version) {
        LOGT << "Version: " << version << ELOG;
        process_version(version);
    });
}


void GAVC::operator()()
{
    Retrier<SleepFor<> >(
        [this] (auto attempt, auto max) -> bool {
            std::vector<std::string> versions_to_process;

            LOGT << "GAVC query attempt: " << attempt << " from: " << max << ELOG;
            versions_to_process = get_versions_to_process();

            if (versions_to_process.empty()) {
                throw std::runtime_error("No any version to process!");
            }

            process_versions(versions_to_process);
            return true;
        },
        std::max(1u, max_attempts_),
        std::max(5u, retry_timeout_s_)
    )(
        [](auto attempt, auto max, const auto& e) {
            LOGT << "GAVC query attempt: " << attempt << " from: " << max << " error: " << errors::format_exceptions_stack(e) << ELOG;
        }
    );
}

void GAVC::set_path_to_download(const std::filesystem::path& path)
{
    path_to_download_ = path;
}

std::filesystem::path GAVC::get_path_to_download() const
{
    return path_to_download_;
}

GAVC::paths_list GAVC::get_list_of_actual_files() const
{
    return list_of_actual_files_;
}

GAVC::query_results GAVC::get_query_results() const
{
    return query_results_;
}

GAVC::paths_list GAVC::get_list_of_queued_files() const
{
    return list_of_queued_files_;
}

void GAVC::set_cache_mode(bool value, PropertiesHandler object_properties_handler)
{
    cache_mode_                 = value;
    object_properties_handler_  = object_properties_handler;
}

void GAVC::notify_gavc_version(const std::string& version)
{
    al::GavcQuery q = query_;
    q.set_version(version);

    LOGT << "notify version: " << version << " query: " << q.to_string() << ELOG;

    notifications_file_.notify("VERSION", version);
    notifications_file_.notify("GAVC", q.to_string());

    versions_.push_back(version);
}

std::vector<std::string> GAVC::get_versions() const
{
    return versions_;
}

} } // namespace piel::cmd
