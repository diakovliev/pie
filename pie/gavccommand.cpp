/*
 * Copyright (c) 2017, Dmytro Iakovliev <email>
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
 * THIS SOFTWARE IS PROVIDED BY Dmytro Iakovliev <email> ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Dmytro Iakovliev <email> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <iostream>
#include <gavccommand.h>
#include <artbasedownloadhandlers.h>
#include <artgavchandlers.h>

#include <boost/bind.hpp>
#include <boost/log/trivial.hpp>
#include <boost/program_options.hpp>
#include <boost_property_tree_ext.hpp>

namespace pt = boost::property_tree;
namespace po = boost::program_options;

GavcCommand::GavcCommand(Application *app, int argc, char **argv)
    : ICommand(app)
    , _argc(argc)
    , _argv(argv)
    , _server_url()
    , _server_api_access_token()
    , _server_repository()
    , _query()
{
}

GavcCommand::~GavcCommand()
{
}

bool GavcCommand::parse_arguments()
{
    po::options_description desc("Query options");

    std::string query_str;
    
    desc.add_options()
        ("token,t",         po::value<std::string>(&_server_api_access_token)->required(),  "Token to access server remote api (required).")
        ("server,s",        po::value<std::string>(&_server_url)->required(),               "Server url (required).")
        ("repository,r",    po::value<std::string>(&_server_repository)->required(),        "Server repository (required).")
        ("query,q",         po::value<std::string>(&query_str)->required(),                 "Query.")
        ;

    if (show_help(desc, _argc, _argv)) {
        return false;
    }

    po::variables_map vm;
    po::parsed_options parsed = po::command_line_parser(_argc, _argv).options(desc).allow_unregistered().run();
    po::store(parsed, vm);
    po::notify(vm);

    // Check if all requared arguments are set
    if (!( vm.count("token")
        && vm.count("server")
        && vm.count("repository")
        && vm.count("query")
    )) {
        std::cerr << "Please specify all required arguments." << std::endl;
        std::cout << desc;
        return false;
    }

    // Parce query
    boost::optional<art::lib::GavcQuery> parsed_query = art::lib::GavcQuery::parse(query_str);
    if (!parsed_query)
    {
        std::cerr << "Wrong GAVC query specified: " << query_str << "!" << std::endl;
        return false;
    }

    _query = *parsed_query;

    return true;
}

std::string GavcCommand::create_url() const
{
    std::string url = _server_url;
    url.append("/api/search/gavc");
    url.append("?r=").append(_server_repository);
    url.append("&g=").append(_query.group());
    url.append("&a=").append(_query.name());
    if (!_query.version().empty()) {
        url.append("&v=").append(_query.version());
    }
    if (!_query.classifier().empty()) {
        url.append("&c=").append(_query.classifier());
    }
    return url;
}

struct FindPropertyHelper
{
    static bool find_object_property(const std::string& prop_name, pt::ptree::value_type prop)
    {
        return prop.first == prop_name;
    }
};

struct BeforeOutputCallback: public art::lib::ArtBaseApiHandlers::IBeforeCallback
{
    virtual void callback(art::lib::ArtBaseApiHandlers *handlers)
    {
        std::string output_filename = handlers->headers()["X-Artifactory-Filename"];
        BOOST_LOG_TRIVIAL(trace) << "Artifactory filename: " << output_filename;

        _dest = boost::shared_ptr<std::ofstream>(new std::ofstream(output_filename.c_str()));

        dynamic_cast<art::lib::ArtBaseDownloadHandlers*>(handlers)->set_destination(_dest.get());
    }
private:
    boost::shared_ptr<std::ofstream> _dest;
};

void GavcCommand::on_object(pt::ptree::value_type obj)
{
    boost::optional<pt::ptree::value_type> op = pt::find(obj.second,
        boost::bind(&FindPropertyHelper::find_object_property, "downloadUri", _1));

    if (!op)
    {
        return;
    }

    pt::ptree::value_type p = *op;

    BOOST_LOG_TRIVIAL(trace) << std::string(p.first) << ": " << std::string(p.second.data());

    art::lib::ArtBaseDownloadHandlers downloadHandlers(_server_api_access_token);
    
    BeforeOutputCallback before_output;
    downloadHandlers.set_before_output_callback(&before_output);

    std::string downloadUri = std::string(p.second.data());
    piel::lib::CurlEasyClient<art::lib::ArtBaseDownloadHandlers> downloadClient(downloadUri, &downloadHandlers);
    downloadClient.perform();
}

/*virtual*/ int GavcCommand::perform()
{
    int result = -1;
    if (!parse_arguments()) {
        return result;
    }

    art::lib::ArtGavcHandlers apiHandlers(_server_api_access_token);
    piel::lib::CurlEasyClient<art::lib::ArtGavcHandlers> client(create_url(), &apiHandlers);
    client.perform();

    // Create a root
    pt::ptree root;

    // Load the json file in this ptree
    pt::read_json(apiHandlers.responce_stream(), root);
    pt::each(root.get_child("results"), boost::bind(&GavcCommand::on_object, this, _1));
    
    result = 0;

    return result;
}
