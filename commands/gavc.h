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

#include <gavcquery.h>
#include <iostreamsholder.h>
#include <notificationsfile.h>
#include <properties.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/program_options.hpp>

#include <filesystem>
#include <list>

#include <commands/base_errors.h>

#include "QueryContext.h"
#include "QueryOperation.h"

namespace piel::cmd {

    namespace fs = std::filesystem;
    namespace pl = piel::lib;

    class GAVC: public QueryOperation, public piel::lib::IOstreamsHolder
    {
    public:
        using paths_list        = std::list<fs::path>;
        using query_results     = std::map<fs::path,std::pair<std::string,std::string> >;
        using PropertiesHandler = std::function<void(const std::string&, const fs::path&, const pl::Properties&)>;

        using ptree             = boost::property_tree::ptree;
        using ptree_value       = boost::property_tree::ptree::value_type;

        GAVC(  const QueryContext *context
             , const QueryOperationParameters *params
             , const std::string& output_file = std::string()
             , const std::string& notifications_file = std::string());

        virtual ~GAVC() = default;

        void operator()();

        void set_path_to_download(const fs::path& path);
        fs::path get_path_to_download() const;
        paths_list get_list_of_actual_files() const;
        query_results get_query_results() const;
        paths_list get_list_of_queued_files() const;
        std::vector<std::string> get_versions() const;

        std::vector<std::string> get_versions_to_process() const;
        void process_versions(const std::vector<std::string>&);
        void process_version(const std::string&);

        void set_cache_mode(bool value, PropertiesHandler object_properties_handler);
        static std::string get_classifier_file_name(const std::string& query_name, const std::string& ver, const std::string& classifier);

        void notify_gavc_version(const std::string& version);

    protected:

        void on_object     (const ptree_value& obj, const std::string& version, const std::string& classifier);
        void on_aql_object (const ptree_value& obj, const std::string& version, const std::string& classifier);

        piel::lib::Properties create_object_properties(
                const ptree_value& obj,
                const std::string& server_object_id, const std::string& object_classifier);

        std::map<std::string,std::string> get_server_checksums(const ptree& obj_tree, const std::string& section) const;
        void download_file(const fs::path& object_path, const std::string& object_id, const std::string& download_uri) const;
        void delete_file(const std::string& download_uri) const;

        std::optional<std::string> object_classifier_if_needed_object_id(const std::string& query_classifier, const std::string& server_object_id) const;

        void handle_actual_object(const std::string& object_id, const std::string& object_path);

    private:
        fs::path path_to_download_;

        bool cache_mode_;
        PropertiesHandler object_properties_handler_;

        paths_list list_of_actual_files_;
        query_results query_results_;
        paths_list list_of_queued_files_;
        std::vector<std::string> versions_;

        std::string output_file_;
        pl::NotificationsFile notifications_file_;
        bool force_offline_;
    };

} // namespace piel::cmd
