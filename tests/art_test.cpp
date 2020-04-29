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

#define BOOST_TEST_MODULE GavcQueryParsing
#include <boost/test/unit_test.hpp>

#include <logging.h>
#include <artdeployartifacthandlers.h>
#include <artcreatedirectoryhandlers.h>
#include <artaqlhandlers.h>
#include <artgavchandlers.h>
#include <gavcquery.h>

#include <boost/bind.hpp>
#include <boost/optional.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost_property_tree_ext.hpp>

using namespace art::lib;
namespace pt = boost::property_tree;

std::string create_gavc_uri(
        const std::string& server_url,
        const std::string& server_repository,
        const art::lib::GavcQuery& q,
        const std::string& version_to_query,
        const std::string& classifier)
{
    std::string url = server_url;
    url.append("/api/search/gavc");
    url.append("?r=").append(server_repository);
    url.append("&g=").append(q.group());
    url.append("&a=").append(q.name());
    if (!version_to_query.empty()) {
        url.append("&v=").append(version_to_query);
    }
    if (!classifier.empty()) {
        url.append("&c=").append(classifier);
    }
    return url;
}

BOOST_AUTO_TEST_CASE(art_gavc_test)
{
    LOGT << "GAVC test" << ELOG;

    auto q = art::lib::GavcQuery::parse("charter_27_1_2.dev.charter.tchwb11.default.third_parties.linux.mipsel:graphite2-1.2.4:70:package");

    const std::string url                       = "https://artifactory.developonbox.ru/artifactory";
    const std::string server_api_access_token   = getenv("GAVC_SERVER_API_ACCESS_TOKEN");
    const std::string repo                      = "bin-release-local";

    art::lib::ArtGavcHandlers gavc_handlers(server_api_access_token);

    std::string uri = create_gavc_uri(url, repo, *q, q->version(), q->classifier());

    piel::lib::CurlEasyClient<art::lib::ArtGavcHandlers> gavc_client(uri, &gavc_handlers);

    if (!gavc_client.perform())
    {
        LOGE << gavc_client.curl_error().presentation() << ELOG;
    }
}

#if 0

GAVC:
  "results" : [ {
    "repo" : "bin-release-local",
    "path" : "/charter_27_1_2/dev/charter/tchwb11/default/third_parties/linux/mipsel/graphite2-1.2.4/70/graphite2-1.2.4-70-package.tbz2",
    "created" : "2020-04-14T20:54:34.117+04:00",
    "createdBy" : "build",
    "lastModified" : "2020-04-14T20:54:34.090+04:00",
    "modifiedBy" : "build",
    "lastUpdated" : "2020-04-14T20:54:34.090+04:00",
    "properties" : {
    },
    "downloadUri" : "https://artifactory.developonbox.ru/artifactory/bin-release-local/charter_27_1_2/dev/charter/tchwb11/default/third_parties/linux/mipsel/graphite2-1.2.4/70/graphite2-1.2.4-70-package.tbz2",
    "mimeType" : "application/octet-stream",
    "size" : "780843",
    "checksums" : {
      "sha1" : "2ccf5feaf567787f7fd865a3ad4ecb3fed428186",
      "md5" : "8e0703e97c3212da1e7218fb57c72370",
      "sha256" : "5a9b74deb565bb1148702250e92aab920f430af8156cd5abdcbd8ba098046de4"
    },
    "originalChecksums" : {
      "sha1" : "2ccf5feaf567787f7fd865a3ad4ecb3fed428186",
      "md5" : "8e0703e97c3212da1e7218fb57c72370",
      "sha256" : "5a9b74deb565bb1148702250e92aab920f430af8156cd5abdcbd8ba098046de4"
    },
    "uri" : "https://artifactory.developonbox.ru/artifactory/api/storage/bin-release-local/charter_27_1_2/dev/charter/tchwb11/default/third_parties/linux/mipsel/graphite2-1.2.4/70/graphite2-1.2.4-70-package.tbz2"
  } ]

AQL:
"results" : [ {
  "repo" : "bin-release-local",
  "path" : "charter_27_1_2/dev/charter/tchwb11/default/third_parties/linux/mipsel/graphite2-1.2.4/70",
  "name" : "graphite2-1.2.4-70-package.tbz2",
  "type" : "file",
  "size" : 780843,
  "created" : "2020-04-14T20:54:34.117+04:00",
  "created_by" : "build",
  "modified" : "2020-04-14T20:54:34.090+04:00",
  "modified_by" : "build",
  "updated" : "2020-04-14T20:54:34.090+04:00",
  "depth" : 11,
  "actual_md5" : "8e0703e97c3212da1e7218fb57c72370",
  "actual_sha1" : "2ccf5feaf567787f7fd865a3ad4ecb3fed428186",
  "original_md5" : "8e0703e97c3212da1e7218fb57c72370",
  "original_sha1" : "2ccf5feaf567787f7fd865a3ad4ecb3fed428186",
  "repo_path_checksum" : "4676aec1de3fbd147cbbba887d3e9246afcfc6cd",
  "sha256" : "5a9b74deb565bb1148702250e92aab920f430af8156cd5abdcbd8ba098046de4",
  "virtual_repos" : [ "bin-release", "repo" ]
} ],

#endif

void on_aql_object(const pt::ptree::value_type& obj, const std::string& url, pt::ptree* results)
{
    boost::optional<std::string> repo = pt::find_value(obj.second, pt::FindPropertyHelper("repo"));
    LOGI << "repo: " << *repo << ELOG;

    boost::optional<std::string> name = pt::find_value(obj.second, pt::FindPropertyHelper("name"));
    LOGI << "name: " << *name << ELOG;

    boost::optional<std::string> path = pt::find_value(obj.second, pt::FindPropertyHelper("path"));
    LOGI << "path: " << *path << ELOG;

    boost::optional<std::string> md5 = pt::find_value(obj.second, pt::FindPropertyHelper("actual_md5"));
    LOGI << "md5: " << *md5 << ELOG;

    boost::optional<std::string> sha1 = pt::find_value(obj.second, pt::FindPropertyHelper("actual_sha1"));
    LOGI << "sha1: " << *sha1 << ELOG;

    boost::optional<std::string> sha256 = pt::find_value(obj.second, pt::FindPropertyHelper("sha256"));
    LOGI << "sha256: " << *sha256 << ELOG;

    std::string path2 = "/" + *path + "/" + *name;
    LOGI << "path2: " << path2 << ELOG;

    std::string downloadUri = url + "/" + *repo + path2;
    LOGI << "downloadUri: " << downloadUri << ELOG;

    std::string uri = url + "/api/storage/" + *repo + path2;
    LOGI << "uri: " << uri << ELOG;

    pt::ptree item;

    item.put("repo", *repo);
    item.put("path", path2);
    item.put("downloadUri", downloadUri);
    item.put("uri", uri);

    pt::ptree checksums;
    checksums.put("sha1", *sha1);
    checksums.put("md5", *md5);
    checksums.put("sha256", *sha256);
    item.add_child("checksums", checksums);

    pt::ptree originalChecksums;
    originalChecksums.put("sha1", *sha1);
    originalChecksums.put("md5", *md5);
    originalChecksums.put("sha256", *sha256);
    item.add_child("originalChecksums", originalChecksums);

    std::ostringstream oss;
    pt::write_json(oss, item);

    LOGI << "item: " << oss.str() << ELOG;

    results->push_back(std::make_pair("", item));
}

BOOST_AUTO_TEST_CASE(art_aql_test)
{
    LOGT << "AQL test" << ELOG;

    auto q = art::lib::GavcQuery::parse("charter_27_1_2.dev.charter.tchwb11.default.third_parties.linux.mipsel:graphite2-1.2.4:70:package");

    LOGI << "aql path: " << q->to_aql_path() << ELOG;
    LOGI << "aql name: " << q->to_aql_name() << ELOG;

    const std::string url                       = "https://artifactory.developonbox.ru/artifactory";
    const std::string server_api_access_token   = getenv("GAVC_SERVER_API_ACCESS_TOKEN");
    const std::string repo                      = "bin-release-local";

    LOGI << "aql: " << q->to_aql(repo) << ELOG;

    art::lib::ArtAqlHandlers aql_handlers(server_api_access_token, q->to_aql(repo));
    aql_handlers.set_url(url);

    piel::lib::CurlEasyClient<art::lib::ArtAqlHandlers> aql_client(aql_handlers.gen_uri(), &aql_handlers, aql_handlers.method());

    if (!aql_client.perform())
    {
        LOGE << aql_client.curl_error().presentation() << ELOG;
    }

    pt::ptree root;
    pt::read_json(aql_handlers.responce_stream(), root);

    pt::ptree root2;
    pt::ptree results;

    pt::each(root.get_child("results"), boost::bind(&on_aql_object, _1, url, &results));

    root2.add_child("results", results);

    std::ostringstream oss;
    pt::write_json(oss, root2);

    LOGI << "oss: " << oss.str() << ELOG;
}
