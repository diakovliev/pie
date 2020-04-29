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

#define BOOST_TEST_MODULE GavcVersionsComparator
#include <boost/test/unit_test.hpp>

#include <gavcversionscomparator.h>

using namespace art::lib;

BOOST_AUTO_TEST_CASE(VersionsCompatator)
{
    std::optional<GavcQuery> op = GavcQuery::parse("test:test:*.*.*");

    BOOST_CHECK(op);

    GavcQuery q = *op;

    GavcVersionsComparator comparator = *(q.query_version_ops());

    // Basic numeric cases
    BOOST_CHECK(0 == comparator.compare("0.0.0", "0.0.0"));
    BOOST_CHECK(0 == comparator.compare("0.1.0", "0.1.0"));
    BOOST_CHECK(0 == comparator.compare("1.0.0", "1.0.0"));
    BOOST_CHECK(0 == comparator.compare("1.1.0", "1.1.0"));
    BOOST_CHECK(0 == comparator.compare("1.1.1", "1.1.1"));

    BOOST_CHECK(0  < comparator.compare("0.0.0", "0.0.1"));
    BOOST_CHECK(0  < comparator.compare("1.0.0", "1.0.1"));
    BOOST_CHECK(0  < comparator.compare("1.1.0", "1.1.1"));

    BOOST_CHECK(0  > comparator.compare("0.0.1", "0.0.0"));
    BOOST_CHECK(0  > comparator.compare("1.0.1", "1.0.0"));
    BOOST_CHECK(0  > comparator.compare("1.1.1", "1.1.0"));

    // Basic lexicographic cases
    BOOST_CHECK(0 == comparator.compare("a.b.c", "a.b.c"));
    BOOST_CHECK(0 == comparator.compare("0.a.0", "0.a.0"));
    BOOST_CHECK(0 == comparator.compare("b.0.0", "b.0.0"));
    BOOST_CHECK(0 == comparator.compare("a.b.0", "a.b.0"));
    BOOST_CHECK(0 == comparator.compare("c.a.b", "c.a.b"));

    BOOST_CHECK(0  < comparator.compare("0.0.0", "0.0.a"));
    BOOST_CHECK(0  < comparator.compare("a.0.0", "a.0.b"));
    BOOST_CHECK(0  < comparator.compare("a.b.0", "a.b.c"));

    BOOST_CHECK(0  > comparator.compare("0.0.a", "0.0.0"));
    BOOST_CHECK(0  > comparator.compare("a.0.b", "a.0.0"));
    BOOST_CHECK(0  > comparator.compare("a.b.c", "a.b.0"));

    // Mixed cases
    BOOST_CHECK(0  < comparator.compare("0.0.a", "0.0.a1"));
    BOOST_CHECK(0  > comparator.compare("0.0.a", "0.0.1a"));
    BOOST_CHECK(0  > comparator.compare("0.0.aaaa", "0.0.aa1aaaaaa"));
}
