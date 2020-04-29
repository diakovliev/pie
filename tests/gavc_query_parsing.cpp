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

#define BOOST_TEST_MODULE GavcQueryParsing
#include <boost/test/unit_test.hpp>

#include <gavcquery.h>

using namespace art::lib;

std::string server_url = "http://server/art";
std::string server_repository = "test-repo-local";
std::string maven_metadata_url = "http://server/art/test-repo-local/adk/trunk/adk/maven-metadata.xml";

bool check_op(const std::vector<gavc::OpType> &ops, int index, gavc::Ops op, const std::string& context_str)
{
    return op == ops[index].first && context_str == ops[index].second;
}

BOOST_AUTO_TEST_CASE(FullQuery_GAVCE)
{
    std::string q_body = "adk.trunk:adk:62:ADK32419p_Explorer3040hd_OCAP_ATSC_SA_pKey@zip";

    std::optional<GavcQuery> op = GavcQuery::parse(q_body);
    BOOST_CHECK(op);
    GavcQuery q = *op;

    BOOST_CHECK_EQUAL("adk.trunk",                                  q.group());
    BOOST_CHECK_EQUAL("adk",                                        q.name());
    BOOST_CHECK_EQUAL("62",                                         q.version());
    BOOST_CHECK_EQUAL("ADK32419p_Explorer3040hd_OCAP_ATSC_SA_pKey", q.classifier());
    BOOST_CHECK_EQUAL("zip",                                        q.extension());

    BOOST_CHECK_EQUAL(q_body,               q.to_string());
    BOOST_CHECK_EQUAL(maven_metadata_url,   q.format_maven_metadata_url(server_url, server_repository));

    std::optional<std::vector<gavc::OpType> > ops_ = q.query_version_ops();
    BOOST_CHECK(ops_);
    std::vector<gavc::OpType> ops = *ops_;

    BOOST_CHECK_EQUAL(1, ops.size());
    BOOST_CHECK(check_op(ops, 0, gavc::Op_const, "62"));
}

BOOST_AUTO_TEST_CASE(Query_GAVC)
{
    std::string q_body = "adk.trunk:adk:62:ADK32419p_Explorer3040hd_OCAP_ATSC_SA_pKey";

    std::optional<GavcQuery> op = GavcQuery::parse(q_body);
    BOOST_CHECK(op);
    GavcQuery q = *op;

    BOOST_CHECK_EQUAL("adk.trunk",                                  q.group());
    BOOST_CHECK_EQUAL("adk",                                        q.name());
    BOOST_CHECK_EQUAL("62",                                         q.version());
    BOOST_CHECK_EQUAL("ADK32419p_Explorer3040hd_OCAP_ATSC_SA_pKey", q.classifier());
    BOOST_CHECK_EQUAL("",                                           q.extension());

    BOOST_CHECK_EQUAL(q_body,               q.to_string());
    BOOST_CHECK_EQUAL(maven_metadata_url,   q.format_maven_metadata_url(server_url, server_repository));

    std::optional<std::vector<gavc::OpType> > ops_ = q.query_version_ops();
    BOOST_CHECK(ops_);
    std::vector<gavc::OpType> ops = *ops_;

    BOOST_CHECK_EQUAL(1, ops.size());
    BOOST_CHECK(check_op(ops, 0, gavc::Op_const, "62"));
}

BOOST_AUTO_TEST_CASE(Query_GAV)
{
    std::string q_body = "adk.trunk:adk:62";

    std::optional<GavcQuery> op = GavcQuery::parse(q_body);
    BOOST_CHECK(op);
    GavcQuery q = *op;

    BOOST_CHECK_EQUAL("adk.trunk",  q.group());
    BOOST_CHECK_EQUAL("adk",        q.name());
    BOOST_CHECK_EQUAL("62",         q.version());
    BOOST_CHECK_EQUAL("",           q.classifier());
    BOOST_CHECK_EQUAL("",           q.extension());

    BOOST_CHECK_EQUAL(q_body,               q.to_string());
    BOOST_CHECK_EQUAL(maven_metadata_url,   q.format_maven_metadata_url(server_url, server_repository));

    std::optional<std::vector<gavc::OpType> > ops_ = q.query_version_ops();
    BOOST_CHECK(ops_);
    std::vector<gavc::OpType> ops = *ops_;

    BOOST_CHECK_EQUAL(1, ops.size());
    BOOST_CHECK(check_op(ops, 0, gavc::Op_const, "62"));
}

BOOST_AUTO_TEST_CASE(Query_GA)
{
    std::string q_body = "adk.trunk:adk";

    std::optional<GavcQuery> op = GavcQuery::parse(q_body);
    BOOST_CHECK(op);
    GavcQuery q = *op;

    BOOST_CHECK_EQUAL("adk.trunk",  q.group());
    BOOST_CHECK_EQUAL("adk/trunk",  q.group_path());
    BOOST_CHECK_EQUAL("adk",        q.name());
    BOOST_CHECK_EQUAL("",           q.version());
    BOOST_CHECK_EQUAL("",           q.classifier());
    BOOST_CHECK_EQUAL("",           q.extension());

    BOOST_CHECK_EQUAL(q_body,               q.to_string());
    BOOST_CHECK_EQUAL(maven_metadata_url,   q.format_maven_metadata_url(server_url, server_repository));

    std::optional<std::vector<gavc::OpType> > ops_ = q.query_version_ops();
    BOOST_CHECK(ops_);
    std::vector<gavc::OpType> ops = *ops_;

    BOOST_CHECK_EQUAL(1, ops.size());
    BOOST_CHECK(check_op(ops, 0, gavc::Op_all, "*"));
}

BOOST_AUTO_TEST_CASE(FailQuery_G)
{
    std::string q_body = "adk.trunk";

    std::optional<GavcQuery> op = GavcQuery::parse(q_body);
    BOOST_CHECK(!op);
}

BOOST_AUTO_TEST_CASE(FailEmptyQuery)
{
    std::optional<GavcQuery> op = GavcQuery::parse("");
    BOOST_CHECK(!op);
}

// Version
BOOST_AUTO_TEST_CASE(VersionParts)
{
    std::string q_body = "adk.trunk:adk:1.1.+.*.123.-.+.1.3.*";

    std::optional<GavcQuery> op = GavcQuery::parse(q_body);
    BOOST_CHECK(op);
    GavcQuery q = *op;

    BOOST_CHECK_EQUAL("adk.trunk",              q.group());
    BOOST_CHECK_EQUAL("adk",                    q.name());
    BOOST_CHECK_EQUAL("1.1.+.*.123.-.+.1.3.*",  q.version());
    BOOST_CHECK_EQUAL("",                       q.classifier());
    BOOST_CHECK_EQUAL("",                       q.extension());

    BOOST_CHECK_EQUAL(q_body,               q.to_string());
    BOOST_CHECK_EQUAL(maven_metadata_url,   q.format_maven_metadata_url(server_url, server_repository));

    std::optional<std::vector<gavc::OpType> > ops_ = q.query_version_ops();
    BOOST_CHECK(ops_);
    std::vector<gavc::OpType> ops = *ops_;

    BOOST_CHECK_EQUAL(10, ops.size());
    BOOST_CHECK(check_op(ops, 0, gavc::Op_const,    "1.1."));
    BOOST_CHECK(check_op(ops, 1, gavc::Op_latest,   "+"));
    BOOST_CHECK(check_op(ops, 2, gavc::Op_const,    "."));
    BOOST_CHECK(check_op(ops, 3, gavc::Op_all,      "*"));
    BOOST_CHECK(check_op(ops, 4, gavc::Op_const,    ".123."));
    BOOST_CHECK(check_op(ops, 5, gavc::Op_oldest,   "-"));
    BOOST_CHECK(check_op(ops, 6, gavc::Op_const,    "."));
    BOOST_CHECK(check_op(ops, 7, gavc::Op_latest,   "+"));
    BOOST_CHECK(check_op(ops, 8, gavc::Op_const,    ".1.3."));
    BOOST_CHECK(check_op(ops, 9, gavc::Op_all,      "*"));
}

BOOST_AUTO_TEST_CASE(VersionParts_not_allow_ops_sequences)
{
    std::string q_body = "adk.trunk:adk:++++";

    std::optional<GavcQuery> op = GavcQuery::parse(q_body);
    BOOST_CHECK(!op);

    q_body = "adk.trunk:adk:+***";

    op = GavcQuery::parse(q_body);
    BOOST_CHECK(!op);

    q_body = "adk.trunk:adk:+***+-.sadjl++";

    op = GavcQuery::parse(q_body);
    BOOST_CHECK(!op);

    q_body = "adk.trunk:adk:+*";

    op = GavcQuery::parse(q_body);
    BOOST_CHECK(!op);

    q_body = "adk.trunk:adk:*";

    op = GavcQuery::parse(q_body);
    BOOST_CHECK(op);
}

BOOST_AUTO_TEST_CASE(CheckForExactVersion)
{
    std::optional<GavcQuery> op = GavcQuery::parse("test.test:test:1");
    BOOST_CHECK(op);
    BOOST_CHECK(op->is_exact_version_query());

    op = GavcQuery::parse("test.test:test:1.1.1");
    BOOST_CHECK(op);
    BOOST_CHECK(op->is_exact_version_query());
    BOOST_CHECK(op->is_single_version_query());

    op = GavcQuery::parse("test.test:test:*");
    BOOST_CHECK(op);
    BOOST_CHECK(!op->is_exact_version_query());
    BOOST_CHECK(!op->is_single_version_query());

    op = GavcQuery::parse("test.test:test:1.*");
    BOOST_CHECK(op);
    BOOST_CHECK(!op->is_exact_version_query());
    BOOST_CHECK(!op->is_single_version_query());

    op = GavcQuery::parse("test.test:test:1.*.1");
    BOOST_CHECK(op);
    BOOST_CHECK(!op->is_exact_version_query());
    BOOST_CHECK(!op->is_single_version_query());

    op = GavcQuery::parse("test.test:test:+");
    BOOST_CHECK(op);
    BOOST_CHECK(!op->is_exact_version_query());
    BOOST_CHECK(op->is_single_version_query());

    op = GavcQuery::parse("test.test:test:1.+");
    BOOST_CHECK(op);
    BOOST_CHECK(!op->is_exact_version_query());
    BOOST_CHECK(op->is_single_version_query());

    op = GavcQuery::parse("test.test:test:1.+.1");
    BOOST_CHECK(op);
    BOOST_CHECK(!op->is_exact_version_query());
    BOOST_CHECK(op->is_single_version_query());

    op = GavcQuery::parse("test.test:test:-");
    BOOST_CHECK(op);
    BOOST_CHECK(!op->is_exact_version_query());
    BOOST_CHECK(op->is_single_version_query());

    op = GavcQuery::parse("test.test:test:1.-");
    BOOST_CHECK(op);
    BOOST_CHECK(!op->is_exact_version_query());
    BOOST_CHECK(op->is_single_version_query());

    op = GavcQuery::parse("test.test:test:1.-.1");
    BOOST_CHECK(op);
    BOOST_CHECK(!op->is_exact_version_query());
    BOOST_CHECK(op->is_single_version_query());
}

BOOST_AUTO_TEST_CASE(VersionsRange)
{
    using namespace art::lib::gavc;

    auto p = GavcQuery::query_version_data("*[123,321]");
    BOOST_CHECK(p.first);
    BOOST_CHECK(p.second);

    BOOST_CHECK_EQUAL("123",    p.second->left);
    BOOST_CHECK_EQUAL("321",    p.second->right);

    BOOST_CHECK(p.second->flags & (RangeFlags_include_left|RangeFlags_include_right));

    p = GavcQuery::query_version_data("*(123,321]");
    BOOST_CHECK(p.first);
    BOOST_CHECK(p.second);

    BOOST_CHECK_EQUAL("123",    p.second->left);
    BOOST_CHECK_EQUAL("321",    p.second->right);

    BOOST_CHECK(!(p.second->flags & (RangeFlags_include_left)));
    BOOST_CHECK(p.second->flags & RangeFlags_include_right);

    p = GavcQuery::query_version_data("*[123,321)");
    BOOST_CHECK(p.first);
    BOOST_CHECK(p.second);

    BOOST_CHECK_EQUAL("123",    p.second->left);
    BOOST_CHECK_EQUAL("321",    p.second->right);

    BOOST_CHECK(p.second->flags & (RangeFlags_include_left));
    BOOST_CHECK(!(p.second->flags & RangeFlags_include_right));

    p = GavcQuery::query_version_data("*(123,321)");
    BOOST_CHECK(p.first);
    BOOST_CHECK(p.second);

    BOOST_CHECK_EQUAL("123",    p.second->left);
    BOOST_CHECK_EQUAL("321",    p.second->right);

    BOOST_CHECK(!(p.second->flags & RangeFlags_include_left));
    BOOST_CHECK(!(p.second->flags & RangeFlags_include_right));
}
