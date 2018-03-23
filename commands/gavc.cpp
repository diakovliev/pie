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
#include <gavc.h>
#include <artbasedownloadhandlers.h>
#include <artgavchandlers.h>
#include <logging.h>
#include <mavenmetadata.h>
#include <artbaseapihandlers.h>

#include <boost/bind.hpp>
#include <boost_property_tree_ext.hpp>

namespace al = art::lib;
namespace pl = piel::lib;
namespace fs = boost::filesystem;
namespace pt = boost::property_tree;
namespace po = boost::program_options;

namespace piel { namespace cmd {

GAVC::GAVC(const std::string& server_api_access_token
           , const std::string& server_url
           , const std::string& server_repository
           , const art::lib::GavcQuery& query
           , const bool have_to_download_results)
    : server_url_(server_url)
    , server_api_access_token_(server_api_access_token)
    , server_repository_(server_repository)
    , query_(query)
    , path_to_download_()
    , have_to_download_results_(have_to_download_results)
    , list_of_downloaded_files_()
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

        dest_ = boost::shared_ptr<std::ofstream>(new std::ofstream(object_path_.generic_string().c_str()));

        dynamic_cast<al::ArtBaseDownloadHandlers*>(handlers)->set_destination(dest_.get());

        return true;
    }
private:
    boost::shared_ptr<std::ofstream> dest_;
    fs::path object_path_;
};

std::map<std::string,std::string> GAVC::get_server_checksums(const pt::ptree& obj_tree, const std::string& section) const
{
    std::map<std::string,std::string> result;

    pt::ptree checksums = obj_tree.get_child(section);

    boost::optional<std::string> op_sha1    = pt::find_value(checksums, pt::FindPropertyHelper("sha1"));
    if (op_sha1)
    {
        result["sha1"] = *op_sha1;
        LOGT << section << " sha1: " << *op_sha1 << ELOG;
    }

    boost::optional<std::string> op_sha256  = pt::find_value(checksums, pt::FindPropertyHelper("sha256"));
    if (op_sha256)
    {
        result["sha256"] = *op_sha256;
        LOGT << section << " sha256: " << *op_sha256 << ELOG;
    }

    boost::optional<std::string> op_md5     = pt::find_value(checksums, pt::FindPropertyHelper("md5"));
    if (op_md5)
    {
        result["md5"] = *op_md5;
        LOGT << section << " md5: " << *op_md5 << ELOG;
    }

    return result;
}

void GAVC::on_object(pt::ptree::value_type obj)
{
    boost::optional<std::string> op_download_uri = pt::find_value(obj.second, pt::FindPropertyHelper("downloadUri"));
    if (!op_download_uri)
    {
        LOGF << "Can't find downloadUri property!" << ELOG;
        return;
    }

    boost::optional<std::string> op_path = pt::find_value(obj.second, pt::FindPropertyHelper("path"));
    if (!op_path)
    {
        LOGF << "Can't find path property!" << ELOG;
        return;
    }

    std::string download_uri = *op_download_uri;
    LOGT << "download_uri: " << download_uri << ELOG;

    if (have_to_download_results_) {

        fs::path path(*op_path);
        LOGT << "path: " << path.generic_string() << ELOG;
        LOGT << "filename: " << path.filename()   << ELOG;

        fs::path object_path = (path_to_download_.empty()) ? path.filename() : path_to_download_ / path.filename();

        LOGT << "object path: " << object_path.generic_string() << ELOG;

        std::map<std::string,std::string> server_checksums      = get_server_checksums(obj.second, "checksums");
        //std::map<std::string,std::string> original_checksums    = get_server_checksums(obj.second, "originalChecksums");

        list_of_downloaded_files_.push_back(object_path);

        bool do_download = true;
        if (fs::exists(object_path))
        {
            std::ifstream is(object_path.generic_string().c_str());

            pl::ChecksumsDigestBuilder digest_builder;
            pl::ChecksumsDigestBuilder::StrDigests str_digests =
                    digest_builder.str_digests_for(is);

            LOGT   << "Sha256 server: "    << server_checksums["sha256"]
                    << " local: "     << str_digests[piel::lib::Sha256::t::name()] << ELOG;
            LOGT   << "Sha1 server: "      << server_checksums["sha1"]
                    << " local: "     << str_digests[piel::lib::Sha::t::name()] << ELOG;
            LOGT   << "Md5 server: "       << server_checksums["md5"]
                    << " local: "     << str_digests[piel::lib::Md5::t::name()] << ELOG;

            do_download = !(  server_checksums["sha256"] == str_digests[piel::lib::Sha256::t::name()]
                           && server_checksums["sha1"]   == str_digests[piel::lib::Sha::t::name()]
                           && server_checksums["md5"]    == str_digests[piel::lib::Md5::t::name()]
                           );
        }

        if (do_download)
        {
            LOGT << "Download/Update object." << ELOG;

            al::ArtBaseDownloadHandlers download_handlers(server_api_access_token_);

            BeforeOutputCallback before_output(object_path);
            download_handlers.set_id(object_path.filename().c_str());
            download_handlers.set_before_output_callback(&before_output);

            pl::CurlEasyClient<al::ArtBaseDownloadHandlers> download_client(download_uri, &download_handlers);

            std::cout << "Downloading file from: " << download_uri << std::endl;

            if (!download_client.perform())
            {
                LOGE << "Error on downloading file attempt!"        << ELOG;
                LOGE << download_client.curl_error().presentation() << ELOG;
            }
        }
        else
        {
            LOGT << "Object already exists." << ELOG;
        }

    } else {

        std::cout << "Download url: " << download_uri << std::endl;

    }
}

std::string GAVC::create_url(const std::string& version_to_query) const
{
    std::string url = server_url_;
    url.append("/api/search/gavc");
    url.append("?r=").append(server_repository_);
    url.append("&g=").append(query_.group());
    url.append("&a=").append(query_.name());
    if (!version_to_query.empty()) {
        url.append("&v=").append(version_to_query);
    }
    if (!query_.classifier().empty()) {
        url.append("&c=").append(query_.classifier());
    }
    return url;
}

void GAVC::operator()()
{
    // Get maven metadata
    al::ArtGavcHandlers download_metadata_handlers(server_api_access_token_);
    pl::CurlEasyClient<al::ArtGavcHandlers> get_metadata_client(
        query_.format_maven_metadata_url(server_url_, server_repository_), &download_metadata_handlers);

    if (!get_metadata_client.perform())
    {
        throw errors::fail_on_request_maven_metadata(get_metadata_client.curl_error().presentation());
    }

    // Try to parse server response.
    boost::optional<al::MavenMetadata> metadata_op = boost::none;
    try
    {
        metadata_op = al::MavenMetadata::parse(download_metadata_handlers.responce_stream());
    }
    catch (...)
    {
        throw errors::fail_to_parse_maven_metadata();
    }

    if (!metadata_op) {
        throw errors::cant_receive_metadata();
    }

    al::MavenMetadata metadata = *metadata_op;

    std::vector<std::string> versions_to_process = metadata.versions_for(query_);

    for (std::vector<std::string>::const_iterator i = versions_to_process.begin(), end = versions_to_process.end(); i != end; ++i)
    {
        LOGT << "Process version: " << *i << ELOG;

        al::ArtGavcHandlers api_handlers(server_api_access_token_);
        pl::CurlEasyClient<art::lib::ArtGavcHandlers> client(create_url(*i), &api_handlers);

        if (!client.perform())
        {
            throw errors::error_processing_version(client.curl_error().presentation(), *i);
        }

        // Create a root
        pt::ptree root;

        // Load the json file into this ptree
        pt::read_json(api_handlers.responce_stream(), root);
        pt::each(root.get_child("results"), boost::bind(&GAVC::on_object, this, _1));
    }

}

} } // namespace piel::cmd