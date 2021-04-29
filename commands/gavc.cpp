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

#include "GavcUtils.h"

namespace piel::cmd {

    namespace pt = boost::property_tree;

    namespace {

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

    }


    GAVC::GAVC( const QueryContext *context
         , const QueryOperationParameters *params
         , const std::string& output_file
         , const std::string& notifications_file)
        : QueryOperation(context, params)
        , pl::IOstreamsHolder()
        , path_to_download_()
        , cache_mode_(false)
        , object_properties_handler_()
        , list_of_actual_files_()
        , query_results_()
        , list_of_queued_files_()
        , versions_()
        , output_file_(output_file)
        , notifications_file_(notifications_file)
    {
    }


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


    void GAVC::download_file(const fs::path& object_path, const std::string& object_id, const std::string& download_uri) const
    {
        LOGT << "Download file: " << object_path << " id: " << object_id << " uri: " << download_uri << ELOG;

        al::ArtBaseDownloadHandlers download_handlers(context()->token());

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

        al::ArtGavcHandlers delete_handlers(context()->token());

        pl::CurlEasyClient<al::ArtGavcHandlers> delete_client(delete_uri, &delete_handlers, al::ArtBaseConstants::rest_api__delete_request);

        try {
            delete_client.perform(true);
        } catch (...) {
            std::throw_with_nested(std::runtime_error("Error on delete remote file uri: " + delete_uri));
        }
    }


    std::optional<std::string> GAVC::object_classifier_if_needed_object_id(const std::string& input_query_classifier, const std::string& server_object_id) const {

        auto query_classifier   = ::utils::gavc::classifier_significant_part(input_query_classifier);
        auto object_classifier  = ::utils::gavc::classifier_from_object_id(server_object_id);

        LOGT << "query classifier: "    << query_classifier     << ELOG;
        LOGT << "object classifier: "   << object_classifier    << ELOG;

        if (object_classifier != GAVCConstants::empty_classifier &&
            query_classifier  != GAVCConstants::empty_classifier &&
            !query_classifier.empty() &&
            object_classifier != query_classifier) {
            LOGD << "Skip the object because the query classifier: '"   << query_classifier << "'"
                 << " is not matches recieved object classifier: '"     << object_classifier << "'" << ELOG;
            return std::nullopt;
        }

        return object_classifier;
    }


    void GAVC::handle_actual_object(const std::string& object_id, const std::string& object_path) {

        list_of_actual_files_.push_back(object_path);

        if (cache_mode_) {
            cout() << "c " << object_id << std::endl;
        } else {
            cout() << "+ " << object_id << std::endl;
        }
    }


    pl::Properties GAVC::create_object_properties(const pt::ptree::value_type& obj, const std::string& server_object_id, const std::string& object_classifier) {

        auto server_checksums       = get_server_checksums(obj.second, "checksums");
        //auto original_checksums    = get_server_checksums(obj.second, "originalChecksums");

        auto object_properties      = pl::Properties::from_map(server_checksums);

        object_properties.set(GAVCConstants::object_id_property, server_object_id);
        object_properties.set(GAVCConstants::object_classifier_property, object_classifier);

        return object_properties;
    }


    void GAVC::on_object(const pt::ptree::value_type& obj, const std::string& version, const std::string& query_classifier)
    {
        LOGT << "on_object version: " << version << " query classifier: " << query_classifier << ELOG;

        auto op_download_uri = pt::find_value(obj.second, pt::FindPropertyHelper("downloadUri"));
        if (!op_download_uri)
        {
            throw std::runtime_error("GAVC query result format error! Can't find 'downloadUri' property!");
        }

        auto op_path = pt::find_value(obj.second, pt::FindPropertyHelper("path"));
        if (!op_path)
        {
            throw std::runtime_error("GAVC query result format error! Can't find 'path' property!");
        }

        fs::path    server_object_path  = *op_path;
        std::string server_object_id    = server_object_path.filename().string();

        LOGT << "server object path: "  << server_object_path << ELOG;
        LOGT << "server object id: "    << server_object_id << ELOG;

        auto object_classifier = object_classifier_if_needed_object_id(query_classifier, server_object_id);
        if (!object_classifier) {
            return;
        }

        fs::path path        = output_file_.empty()         ?   *op_path     : output_file_;
        fs::path object_path = path_to_download_.empty()    ?   path         : path_to_download_ / path.filename()   ;

        std::string object_id     = object_path.filename().string();

        LOGT << "object path: "   << object_path << ELOG;
        LOGT << "object id: "     << object_id   << ELOG;

        cout() << "? " << object_id << "\r";
        cout().flush();

        auto object_properties      = create_object_properties(obj, server_object_id, *object_classifier);

        bool local_file_is_actual   = (cache_mode_) ? ::utils::gavc::validate_local_file(object_path, object_properties) : false;
        bool do_download            = !local_file_is_actual;

        if (params()->have_to_download_results() && do_download)
        {
            cout() << "- " << object_id << "\r";
            cout().flush();

            if (!object_path.parent_path().string().empty()) {
                fs::create_directories(object_path.parent_path());
            }

            download_file(object_path, object_id, *op_download_uri);

            if (cache_mode_ && object_properties_handler_) {
                object_properties_handler_(version, object_path, object_properties);
            }

            handle_actual_object(object_id, object_path);
        }
        else if (local_file_is_actual)
        {
            handle_actual_object(object_id, object_path);
        }
        else
        {
            cout() << "- " << object_id << std::endl;
        }

        if (params()->have_to_delete_results())
        {
            delete_file(*op_download_uri);

            cout() << "x " << *op_download_uri << std::endl;
        }

        LOGT << "Add query result. { object path: " << object_path
                << " classifier: " << *object_classifier
                << " version: " << version << " }" << ELOG;

        query_results_.insert(std::make_pair(object_path, std::make_pair(*object_classifier, version)));
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
                throw std::runtime_error("AQL query result format error! Can't find required'" + name + "' property!");
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

        std::string downloadUri = context()->server_url() + "/" + *repo + path2;
        LOGT << "aql downloadUri: " << downloadUri << ELOG;

        std::string uri = context()->server_url() + "/api/storage/" + *repo + path2;
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


    std::vector<std::string> GAVC::get_versions_to_process() const
    {
        // Get maven metadata
        al::ArtGavcHandlers download_metadata_handlers(context()->token());
        pl::CurlEasyClient<al::ArtGavcHandlers> get_metadata_client(
            context()->get_maven_metadata_url(), &download_metadata_handlers);

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

        return metadata.versions_for(context()->query());
    }


    void GAVC::process_version(const std::string& version)
    {
        LOGT << "Version: " << version << ELOG;

        GAVC::notify_gavc_version(version);

        cout() << "Version: "       << version  << std::endl;
        cout() << "Mode: online"                << std::endl;

        std::string classifiers_spec = context()->query().classifier();

        LOGT << "Classifiers spec: " << classifiers_spec << ELOG;

        auto classifiers = ::utils::gavc::parse_classifiers_spec(classifiers_spec);

        for(auto classifier: classifiers) {
            LOGT << "Classifier: " << classifier << ELOG;

            al::GavcQuery q = context()->query();
            q.set_version(version);
            q.set_classifier(classifier);

            al::ArtAqlHandlers aql_handlers(context()->token(), q.to_aql(context()->repository()));
            aql_handlers.set_url(context()->server_url());

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
                    if (!classifier.empty()) {
                        throw std::runtime_error("No requested classifier: " + classifier + " in artifact with version: " + version + "!");
                    }
                }
            );

        }

        if (params()->have_to_delete_versions()) {
            LOGT << "Delete version: " << version << ELOG;
            cout() << "X " << version << std::endl;

            delete_file(context()->get_version_url(version));
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
            std::max(1u, params()->max_attempts()),
            std::max(5u, params()->retry_timeout_s())
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
        al::GavcQuery q = context()->query();
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

} // namespace piel::cmd
