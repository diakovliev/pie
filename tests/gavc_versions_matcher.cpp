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

#define BOOST_TEST_MODULE GavcVersionsMatcher
#include <boost/test/unit_test.hpp>

#include <gavcquery.h>
#include <gavcversionsmatcher.h>

using namespace art::lib;

struct SignificantParts_3digits {
    std::string query;
    int         groups_count;
    std::string version;
    std::string d0;
    std::string d1;
    std::string d2;
};

//std::vector<SignificantParts_3digits> sp_3d;
void init_3d_versions(std::vector<SignificantParts_3digits>& sp_3d)
{
    sp_3d.push_back( { "test:test:+.-.*", 3, "0.0.1", "0", "0", "1" } );
    sp_3d.push_back( { "test:test:*.+.-", 3, "0.1.0", "0", "1", "0" } );
    sp_3d.push_back( { "test:test:-.+.*", 3, "1.0.0", "1", "0", "0" } );
    sp_3d.push_back( { "test:test:*.-.*", 3, "1.0.1", "1", "0", "1" } );
    sp_3d.push_back( { "test:test:*.+.*", 3, "1.1.1", "1", "1", "1" } );
    sp_3d.push_back( { "test:test:+.*.-", 3, "1.1.0", "1", "1", "0" } );
    sp_3d.push_back( { "test:test:-.*.+", 3, "111.11.0", "111", "11", "0" } );

    sp_3d.push_back( { "test:test:+.-", 2, "111.11", "111", "11", "" } );
    sp_3d.push_back( { "test:test:-",   1, "111", "111", "", "" } );
}

BOOST_AUTO_TEST_CASE(VersionsMatcher_significant_parts_0)
{
    std::optional<GavcQuery> op = GavcQuery::parse("test:test:12.6.*.123.+.*");

    BOOST_CHECK(op);

    GavcQuery q = *op;

    GavcVersionsMatcher marcher = *(q.query_version_ops());
    std::vector<std::string> sparts = marcher.significant_parts("12.6.ods.123.4453.987");

    BOOST_CHECK_EQUAL(3,            sparts.size());
    BOOST_CHECK_EQUAL("ods",        sparts[0]);
    BOOST_CHECK_EQUAL("4453",       sparts[1]);
    BOOST_CHECK_EQUAL("987",        sparts[2]);
}

BOOST_AUTO_TEST_CASE(VersionsMatcher_significant_parts_1)
{
    std::vector<SignificantParts_3digits> sp_3d;
    init_3d_versions(sp_3d);
    for (std::vector<SignificantParts_3digits>::const_iterator i = sp_3d.begin(), end = sp_3d.end(); i != end; ++i)
    {
        std::optional<GavcQuery> op = GavcQuery::parse(i->query);

        BOOST_CHECK(op);

        GavcQuery q = *op;

        GavcVersionsMatcher marcher = *(q.query_version_ops());

        std::vector<std::string> sparts = marcher.significant_parts(i->version);
                                 BOOST_CHECK_EQUAL(i->groups_count,  sparts.size());
        if (i->groups_count > 0) BOOST_CHECK_EQUAL(i->d0,            sparts[0]);
        if (i->groups_count > 1) BOOST_CHECK_EQUAL(i->d1,            sparts[1]);
        if (i->groups_count > 2) BOOST_CHECK_EQUAL(i->d2,            sparts[2]);
    }
}
