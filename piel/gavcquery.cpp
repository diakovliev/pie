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

#include <gavcquery.h>
#include <json_format.hpp>

#include <logging.h>
#include <gavcconstants.h>

#include <algorithm>
#include <functional>

#include <gavcversionsfilter.h>
#include <gavcversionsrangefilter.h>

namespace art { namespace lib {

GavcQuery::GavcQuery()
    : data_()
{
}

GavcQuery::~GavcQuery()
{
}

std::optional<GavcQuery> GavcQuery::parse(const std::string& gavc_str)
{
    if (gavc_str.empty())
        return std::nullopt;

    GavcQuery result;

    auto data = parsers::gavc::parse_query(gavc_str);
    if (!data) {
        LOGE << "Can't parse GAVC query: " << gavc_str << "!" << ELOG;
        return std::nullopt;
    }

    result.data_ = *data;

    auto ops = result.query_version_ops();
    if (!ops) {
        return std::nullopt;
    }

    return result;
}

std::optional<std::vector<gavc::OpType> > GavcQuery::query_version_ops() const
{
    return query_version_ops(data_.version);
}

std::pair<
    std::optional<std::vector<gavc::OpType> >,
    std::optional<gavc::gavc_versions_range_data>
> GavcQuery::query_version_data() const {
    return query_version_data(data_.version);
}

std::optional<gavc::gavc_versions_range_data> GavcQuery::query_versions_range() const
{
    return query_versions_range(data_.version);
}


/* static */
std::pair<
    std::optional<std::vector<gavc::OpType> >,
    std::optional<gavc::gavc_versions_range_data>
> GavcQuery::query_version_data(const std::string& version) {

    if (version.empty()) {
        std::vector<gavc::OpType> ops;
        ops.push_back(gavc::OpType(gavc::Op_all, GavcConstants::all_versions));
        return std::make_pair(ops, std::nullopt);
    }

    auto version_data = parsers::gavc::parse_version(version);
    if (!version_data) {
        LOGE << "version query: " << version << " has wrong syntax!" << ELOG;
        return std::make_pair(std::nullopt, std::nullopt);
    }

    return *version_data;
}


/* static */ std::optional<std::vector<gavc::OpType> > GavcQuery::query_version_ops(const std::string& version)
{
    return query_version_data(version).first;
}

/* static */ std::optional<gavc::gavc_versions_range_data> GavcQuery::query_versions_range(const std::string& version)
{
    return query_version_data(version).second;
}


bool GavcQuery::is_exact_version_query() const
{
    return is_exact_version_query(data_.version);
}

/* static */ bool GavcQuery::is_exact_version_query(const std::string& version)
{
    auto query_version_data = GavcQuery::query_version_data(version);

    auto range = query_version_data.second;
    if (range) return false;

    auto pops = query_version_data.first;
    // The empty version is equvalent of '*'
    if (!pops) return false;

    return std::find_if(pops->begin(), pops->end(), [](auto& op) { return op.first > gavc::Op_const; }) == pops->end();
}

bool GavcQuery::is_single_version_query() const
{
    return is_single_version_query(data_.version);
}

/* static */ bool GavcQuery::is_single_version_query(const std::string& version)
{
    auto query_version_data = GavcQuery::query_version_data(version);

    auto range = query_version_data.second;
    if (range) return false;

    auto pops = query_version_data.first;
    // The empty version is equvalent of '*'
    if (!pops) return false;

    return std::find_if(pops->begin(), pops->end(), [](auto& op) { return op.first == gavc::Op_all; }) == pops->end();
}

std::string GavcQuery::to_string() const
{
    std::ostringstream result;

    if (!group().empty())
    {
        result << group();
    }
    if (!name().empty())
    {
        result << GavcConstants::delimiter;
        result << name();
    }
    if (!version().empty())
    {
        result << GavcConstants::delimiter;
        result << version();
    }
    if (!classifier().empty())
    {
        result << GavcConstants::delimiter;
        result << classifier();
    }
    if (!extension().empty())
    {
        result << GavcConstants::extension_prefix;
        result << extension();
    }

    LOGT << result.str() << ELOG;

    return result.str();
}


std::string GavcQuery::group_path() const
{
    std::string group_path = group();

    std::replace(group_path.begin(), group_path.end(), GavcConstants::group_delimiter, GavcConstants::path_delimiter);

    return group_path;
}


std::string GavcQuery::format_maven_metadata_url(const std::string& server_url, const std::string& repository) const
{
    LOGT << "Build url for maven metadata. server_url: " << server_url << " repository: " << repository << ELOG;

    std::ostringstream result;
    result << server_url;
    result << format_maven_metadata_path(repository);
    result << GavcConstants::path_delimiter;
    result << GavcConstants::maven_metadata_filename;

    LOGT << "Maven metadata url: " << result.str() << ELOG;
    return result.str();
}


std::string GavcQuery::format_version_url(const std::string& server_url, const std::string& repository, const std::string& version) const
{
    LOGT << "Build url for version. server_url: " << server_url << " repository: " << repository << " version: " << version << ELOG;

    std::ostringstream result;
    result << server_url;
    result << format_maven_metadata_path(repository);
    result << GavcConstants::path_delimiter;
    result << version;

    LOGT << "Version url: " << result.str() << ELOG;
    return result.str();
}


std::string GavcQuery::format_maven_metadata_path(const std::string& repository) const
{
    LOGT << "Build path for maven metadata. repository: " << repository << ELOG;

    std::ostringstream result;
    result << GavcConstants::path_delimiter;
    result << repository;
    result << GavcConstants::path_delimiter;
    result << group_path();
    result << GavcConstants::path_delimiter;
    result << name();

    LOGT << "Cache path to metadata url: " << result.str() << ELOG;
    return result.str();
}


std::vector<std::string> GavcQuery::filtered(const std::vector<std::string>& versions) const {
    std::vector<std::string> result = versions;

    auto data = query_version_data();
    if (data.first && data.second) {
        auto ops = data.first;
        auto range = data.second;

        GavcVersionsRangeFilter filter(*ops, range->left, range->right, range->flags);

        return filter.filtered(result);
    } else if (data.first) {
        auto ops = data.first;

        if (!ops) {
            LOGT << "Unable to get query operations list." << ELOG;
            return result;
        }

        GavcVersionsFilter filter(*ops);
        return filter.filtered(result);
    }

    return result;
}

std::string GavcQuery::to_aql_path() const
{
    std::string os;
    os.append(group_path());
    os.append(1, GavcConstants::path_delimiter);
    os.append(name());
    os.append(1, GavcConstants::path_delimiter);
    os.append("*");
    return os;
}

std::string GavcQuery::to_aql_name(bool pom) const
{
    bool append_star = classifier().empty() | extension().empty();
    std::string os;
    os.append(name());
    os.append("-");
    os.append(version());
    if (!classifier().empty())
    {
        os.append("-");
        os.append(classifier());
    }
    if (!extension().empty())
    {
        os.append(".");
        os.append(extension());
    }
    if (append_star && pom)
    {
        os.append(".*");
    }
    else if (append_star && !pom)
    {
        os.append("-*");
    }
    return os;
}

using json_list  = json_format::list<std::ostringstream>;
using json_scope = json_format::scope<std::ostringstream>;

std::string GavcQuery::to_aql_cond(const std::string& repo, bool pom) const
{
    std::ostringstream oss;

    {
        json_scope jand(oss);
        jand.setb<json_list>("$and", [&](json_list& list) {

            list.addb<json_scope>([&](json_scope& jrepo) {
                jrepo.sets("repo", repo);
            });

            list.addb<json_scope>([&](json_scope& path) {
                path.setb<json_scope>("path", [&](auto& match) {
                    match.sets("$match", to_aql_path());
                });
            });

            list.addb<json_scope>([&](json_scope& name) {
                name.setb<json_scope>("name", [&](auto& match) {
                    match.sets("$match", to_aql_name(pom));
                });
            });

        });
    }

    std::string aql_cond = oss.str();
    LOGT << "aql cond: " << aql_cond << ELOG;

    return aql_cond;
}

class aql_find: public json_scope {
public:
    aql_find(std::ostringstream& oss)
        : json_scope(oss, "items.find({", "})") {}
};

std::string GavcQuery::to_aql(const std::string& repo) const
{
    std::ostringstream oss;

    {
        aql_find find(oss);

        find.setb<json_list>("$or", [&](auto& list) {
            // Raw output
            list << to_aql_cond(repo, false);
            list << json_format::symbols::next;
            list << to_aql_cond(repo, true);
        });
    }

    oss << ".include(" << json_format::string("*") << ")";

    std::string aql_query = oss.str();
    LOGT << "aql query: " << aql_query << ELOG;

    return aql_query;
}

} } // namespace art::lib
