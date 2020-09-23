/*
 * Copyright (c) 2020, Dmytro Iakovliev daemondzk@gmail.com
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

#define BOOST_TEST_MODULE Properties

#include <boost/test/unit_test.hpp>
#include <parsers/properties.h>

BOOST_AUTO_TEST_CASE(parse_comments) {
    auto res = parsers::properties::parse_property("# 123");
    BOOST_CHECK(!res);

    res = parsers::properties::parse_property(" # 123");
    BOOST_CHECK(!res);

    res = parsers::properties::parse_property("   # 123");
    BOOST_CHECK(!res);

    res = parsers::properties::parse_property("   #");
    BOOST_CHECK(!res);
}

BOOST_AUTO_TEST_CASE(parse_not_valid_key) {
    auto res = parsers::properties::parse_property("key#1 =  123");
    BOOST_CHECK(!res);

    res = parsers::properties::parse_property("key 1 =  123");
    BOOST_CHECK(!res);
}

BOOST_AUTO_TEST_CASE(parse_empty_string) {
    auto res = parsers::properties::parse_property("");
    BOOST_CHECK(!res);

    res = parsers::properties::parse_property("  ");
    BOOST_CHECK(!res);
}

BOOST_AUTO_TEST_CASE(parse_not_valid_string) {
    auto res = parsers::properties::parse_property("This is regular string");
    BOOST_CHECK(!res);

    res = parsers::properties::parse_property("  for test!");
    BOOST_CHECK(!res);
}

void check_parser(const std::pair<std::string, std::string>& expected, const std::string& input) {
    auto res = parsers::properties::parse_property(input);

    BOOST_CHECK(res.has_value());
    if (res) {
        BOOST_CHECK_EQUAL(expected.first, res->first);
        BOOST_CHECK_EQUAL(expected.second, res->second);
    }
}

BOOST_AUTO_TEST_CASE(parse_strip_key_value) {
    check_parser(std::make_pair("key", "value"), "key=value");
    check_parser(std::make_pair("key", " value"), " key= value");
    check_parser(std::make_pair("key", " value "), " key = value ");
    check_parser(std::make_pair("key", "   value "), "  key =   value ");
    check_parser(std::make_pair("key", "  value "), "  key  =  value ");
}
