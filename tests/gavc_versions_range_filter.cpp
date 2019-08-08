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

#define BOOST_TEST_MODULE GavcVersionsFilter
#include <boost/test/unit_test.hpp>

#include <gavcquery.h>
#include <gavcversionsrangefilter.h>
#include <gavcversionsmatcher.h>

using namespace art::lib;

void init_versions(std::vector<std::string>& versions)
{
    versions.push_back("16.3.123");
    versions.push_back("16.3.532");
    versions.push_back("16.6.123");
    versions.push_back("16.6.124");
    versions.push_back("16.6.125");
    versions.push_back("16.6.533");
    versions.push_back("16.8.123");
    versions.push_back("16.8.536");
}

bool contains_value(const std::vector<std::string>& vector, const std::string& value)
{
    return std::find(vector.begin(), vector.end(), value) != vector.end();
}

std::vector<std::string> filter_according_to_query(const std::string& gavc, const std::vector<std::string>& versions) {
    boost::optional<GavcQuery> q = GavcQuery::parse(gavc);
    BOOST_CHECK(q);
    return q->filter(versions);
}

BOOST_AUTO_TEST_CASE(NonExistingBorders)
{
    std::vector<std::string> versions;
    init_versions(versions);

    std::vector<std::string> filtered = filter_according_to_query("test:test:16.6.*[16.6.122,16.6.534]", versions);

    BOOST_CHECK_EQUAL(4,            filtered.size());
    BOOST_CHECK_EQUAL("16.6.123",   filtered[0]);
    BOOST_CHECK_EQUAL("16.6.124",   filtered[1]);
    BOOST_CHECK_EQUAL("16.6.125",   filtered[2]);
    BOOST_CHECK_EQUAL("16.6.533",   filtered[3]);
}

BOOST_AUTO_TEST_CASE(IncludeBorders)
{
    std::vector<std::string> versions;
    init_versions(versions);

    std::vector<std::string> filtered = filter_according_to_query("test:test:16.6.*[16.6.-,16.6.+]", versions);

    BOOST_CHECK_EQUAL(4,            filtered.size());
    BOOST_CHECK_EQUAL("16.6.123",   filtered[0]);
    BOOST_CHECK_EQUAL("16.6.124",   filtered[1]);
    BOOST_CHECK_EQUAL("16.6.125",   filtered[2]);
    BOOST_CHECK_EQUAL("16.6.533",   filtered[3]);

    filtered = filter_according_to_query("test:test:16.6.*[16.6.123,16.6.533]", versions);

    BOOST_CHECK_EQUAL(4,            filtered.size());
    BOOST_CHECK_EQUAL("16.6.123",   filtered[0]);
    BOOST_CHECK_EQUAL("16.6.124",   filtered[1]);
    BOOST_CHECK_EQUAL("16.6.125",   filtered[2]);
    BOOST_CHECK_EQUAL("16.6.533",   filtered[3]);
}

BOOST_AUTO_TEST_CASE(ExcludeLeftBorder)
{
    std::vector<std::string> versions;
    init_versions(versions);

    std::vector<std::string> filtered = filter_according_to_query("test:test:16.6.*(16.6.-,16.6.+]", versions);

    BOOST_CHECK_EQUAL(3,            filtered.size());
    BOOST_CHECK_EQUAL("16.6.124",   filtered[0]);
    BOOST_CHECK_EQUAL("16.6.125",   filtered[1]);
    BOOST_CHECK_EQUAL("16.6.533",   filtered[2]);
}

BOOST_AUTO_TEST_CASE(ExcludeRightBorder)
{
    std::vector<std::string> versions;
    init_versions(versions);

    std::vector<std::string> filtered = filter_according_to_query("test:test:16.6.*[16.6.-,16.6.+)", versions);

    BOOST_CHECK_EQUAL(3,            filtered.size());
    BOOST_CHECK_EQUAL("16.6.123",   filtered[0]);
    BOOST_CHECK_EQUAL("16.6.124",   filtered[1]);
    BOOST_CHECK_EQUAL("16.6.125",   filtered[2]);
}
