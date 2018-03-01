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
#include <cstdlib>
#include <pushcommand.h>
#include <push.h>
#include <artbaseapideployartifacthandlers.h>

#include <artbasedownloadhandlers.h>

#include <artgavchandlers.h>
#include <logging.h>
#include <mavenmetadata.h>

#include <boost/bind.hpp>
#include <boost_property_tree_ext.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

namespace pie { namespace app {

namespace pt = boost::property_tree;
namespace po = boost::program_options;

PushCommand::PushCommand(Application *app, int argc, char **argv)
    : ICommand(app)
    , argc_(argc)
    , argv_(argv)
    , server_url_()
    , server_api_access_token_()
    , server_repository_()
    , query_()
    , have_to_download_results_(false)
    , classifier_vector_()
{
}

PushCommand::~PushCommand()
{
}

bool PushCommand::get_from_env(po::variables_map& vm,
                               const std::string& opt_name,
                               const std::string& env_var,
                               std::string& var)
{
    if (!vm.count(opt_name)) {
        const char *value = ::getenv(env_var.c_str());
        if (value)
        {
            LOGT << "Got " << env_var << " environment variable. Value: " << value << "." << ELOG;
            var = std::string(value);
            return true;
        }
        else
        {
            return false;
        }
    }
    return true;
}

void PushCommand::show_command_help_message(const po::options_description& desc)
{
    std::cerr << "Usage: gavc <gavc query> [options]" << std::endl;
    std::cout << "Note: Use specific version in the <gavc query>" << std::endl;
    std::cout << desc;
}

bool PushCommand::parse_arguments()
{
    po::options_description desc("Push options");
    desc.add_options()
        ("token,t",         po::value<std::string>(&server_api_access_token_),  "Token to access server remote api (required). Can be set using GAVC_SERVER_API_ACCESS_TOKEN environment variable.")
        ("server,s",        po::value<std::string>(&server_url_),               "Server url (required). Can be set using GAVC_SERVER_URL environment variable.")
        ("repository,r",    po::value<std::string>(&server_repository_),        "Server repository (required). Can be set using GAVC_SERVER_REPOSITORY environment variable.")
        ("filelist,f",      po::value<std::string>(&classifier_vector_str_)->required(),    "List of files to upload (required). Use as: file1,file2,...")
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

    boost::optional<art::lib::GavcQuery> parsed_query = art::lib::GavcQuery::parse(query_str);
    if (!parsed_query)
    {
        std::cout << "Wrong gavc query: " << query_str << "!" << std::endl;
        show_command_help_message(desc);
        return false;
    }

    //[bool pie::app::PushCommand::parse_arguments()] query_:test_dir:dir2:4
    query_ = *parsed_query;

    try
    {
        int version;
        version = boost::lexical_cast<int>(query_.version());
    }
    catch(boost::bad_lexical_cast& e)
    {
        std::cout << "'" << query_.version() << "' Version must be numeric value" << std::endl;
        return false;
    }

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

    // Gen file list
    LOGT << "classifier_vector_str_:" << classifier_vector_str_ << ELOG;
    boost::split(classifier_vector_, classifier_vector_str_, boost::is_any_of(","));

    for (StringVector::const_iterator it = classifier_vector_.begin(), end = classifier_vector_.end(); it != end; ++it) {
        LOGT << "classifier_vector_:" << (*it) << ELOG;
    }

    if (!get_env_flag) {
        show_command_help_message(desc);
        return false;
    }

    //have_to_download_results_ = vm.count("download");

    return true;
}

std::string PushCommand::create_url(const std::string& version_to_query) const
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

struct BeforeOutputCallback: public art::lib::ArtBaseApiHandlers::IBeforeCallback
{
    BeforeOutputCallback(const boost::filesystem::path& object_path): dest_(), object_path_(object_path) {}
    virtual ~BeforeOutputCallback() {}

    virtual bool callback(art::lib::ArtBaseApiHandlers *handlers)
    {
        LOGT << "Output path: " << object_path_.generic_string() << ELOG;

        dest_ = boost::shared_ptr<std::ofstream>(new std::ofstream(object_path_.generic_string().c_str()));

        dynamic_cast<art::lib::ArtBaseDownloadHandlers*>(handlers)->set_destination(dest_.get());

        return true;
    }
private:
    boost::shared_ptr<std::ofstream> dest_;
    boost::filesystem::path object_path_;
};

std::map<std::string,std::string> PushCommand::get_server_checksums(const pt::ptree& obj_tree, const std::string& section) const
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

void PushCommand::on_object(pt::ptree::value_type obj)
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

        boost::filesystem::path path(*op_path);
        LOGT << "path: " << path.generic_string() << ELOG;
        LOGT << "filename: " << path.filename()   << ELOG;

        boost::filesystem::path object_path(path.filename());
        LOGT << "object path: " << object_path.generic_string() << ELOG;

        std::map<std::string,std::string> server_checksums      = get_server_checksums(obj.second, "checksums");
        //std::map<std::string,std::string> original_checksums    = get_server_checksums(obj.second, "originalChecksums");

        bool do_download = true;
        if (boost::filesystem::exists(object_path))
        {
            std::ifstream is(object_path.generic_string().c_str());

            piel::lib::ChecksumsDigestBuilder digest_builder;
            piel::lib::ChecksumsDigestBuilder::StrDigests str_digests =
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

            art::lib::ArtBaseDownloadHandlers download_handlers(server_api_access_token_);

            BeforeOutputCallback before_output(object_path);
            download_handlers.set_id(object_path.filename().c_str());
            download_handlers.set_before_output_callback(&before_output);

            piel::lib::CurlEasyClient<art::lib::ArtBaseDownloadHandlers> download_client(download_uri, &download_handlers);

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

/*virtual*/ int PushCommand::perform()
{
    int result = -1;

    if (!parse_arguments()) {
        return result;
    }

    for (StringVector::const_iterator it = classifier_vector_.begin(), end = classifier_vector_.end(); it != end; ++it) {
        LOGT << "classifier_vector_:" << (*it) << ELOG;

        art::lib::ArtBaseApiDeployArtifactHandlers deploy_handlers(server_api_access_token_);
        deploy_handlers.set_url(server_url_);
        deploy_handlers.set_repo(server_repository_);
        deploy_handlers.set_path(query_.group());
        deploy_handlers.set_group(query_.name());
        deploy_handlers.set_version(query_.version());
        deploy_handlers.set_classifier((*it));
        deploy_handlers.file((*it));

        LOGT << "--" << __LINE__ << "--" << ELOG;

        piel::lib::CurlEasyClient<art::lib::ArtBaseApiDeployArtifactHandlers> art1_client(deploy_handlers.gen_uri(), &deploy_handlers);
        LOGT << "--" << __LINE__ << "--" << ELOG;

        std::cout << "upload to here: " << deploy_handlers.gen_uri() << std::endl;

        if (!art1_client.perform())
        {
            LOGE << "Error on downloading file attempt!"        << ELOG;
            LOGE << art1_client.curl_error().presentation() << ELOG;
        }
        LOGT << "--" << __LINE__ << "--" << ELOG;

    }
    return result;

    // Get maven metadata
    art::lib::ArtGavcHandlers download_metadata_handlers(server_api_access_token_);
    piel::lib::CurlEasyClient<art::lib::ArtGavcHandlers> get_metadata_client(
        query_.format_maven_metadata_url(server_url_, server_repository_), &download_metadata_handlers);

    if (!get_metadata_client.perform())
    {
        LOGE << "Error on requesting maven metadata."           << ELOG;
        LOGE << get_metadata_client.curl_error().presentation() << ELOG;
        return result;
    }

    // Try to parse server response.
    boost::optional<art::lib::MavenMetadata> metadata_op = boost::none;
    try
    {
        metadata_op = art::lib::MavenMetadata::parse(download_metadata_handlers.responce_stream());
    }
    catch (...)
    {
        LOGE << "Error on parsing maven metadata. Server response has non expected format." << ELOG;
    }

    if (!metadata_op) {
        LOGE << "Can't retrieve maven metadata!" << ELOG;
        return result;
    }

    art::lib::MavenMetadata metadata = *metadata_op;

    std::vector<std::string> versions_to_process = metadata.versions_for(query_);

    for (std::vector<std::string>::const_iterator i = versions_to_process.begin(), end = versions_to_process.end(); i != end; ++i)
    {
        LOGT << "Process version: " << *i << ELOG;

        art::lib::ArtGavcHandlers api_handlers(server_api_access_token_);
        piel::lib::CurlEasyClient<art::lib::ArtGavcHandlers> client(create_url(*i), &api_handlers);

        if (!client.perform())
        {
            LOGE << "Error on processing version: " << *i << "!"    << ELOG;
            LOGE << client.curl_error().presentation()              << ELOG;
            return result;
        }

        // Create a root
        pt::ptree root;

        // Load the json file into this ptree
        pt::read_json(api_handlers.responce_stream(), root);
        pt::each(root.get_child("results"), boost::bind(&PushCommand::on_object, this, _1));
    }

    result = 0;

    return result;
}

} } // namespace pie::app
