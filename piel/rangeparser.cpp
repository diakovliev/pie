/*
 * Copyright (c) 2018, diakovliev
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
 * THIS SOFTWARE IS PROVIDED BY diakovliev ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL diakovliev BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <rangeparser.h>
#include <logging.h>
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/algorithm/string/trim.hpp>

typedef piel::lib::refs::Range Range;
BOOST_FUSION_ADAPT_STRUCT(
    Range,
    (std::string, first)
    (std::string, second)
)

namespace piel { namespace lib {

struct RangeSpecConstants {
    static const char sepa;
};

const char RangeSpecConstants::sepa = ':';

namespace range_spec {

    namespace qi = boost::spirit::qi;

    template<typename Iterator>
    struct grammar: qi::grammar<Iterator, Range()> {
        grammar(): grammar::base_type(_data)
        {
            using qi::char_;
            using qi::as_string;

            _first      = as_string[ +( char_ - RangeSpecConstants::sepa) ];
            _second     = as_string[ +( char_ ) ];
            _data       = -_first > RangeSpecConstants::sepa > -_second;
        }

    private:
        qi::rule<Iterator, std::string()>   _first;
        qi::rule<Iterator, std::string()>   _second;
        qi::rule<Iterator, Range()>         _data;
    };

} // namespace range_spec


RangeParser::RangeParser()
{

}

RangeParser::~RangeParser()
{

}

/*static*/ refs::Range RangeParser::parse(const std::string& spec)
{
    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    Range result;

    if (!spec.empty())
    {
        range_spec::grammar<std::string::const_iterator> range_spec_grammar;
        qi::phrase_parse( spec.begin(), spec.end(), range_spec_grammar, ascii::space, result);
        LOGT << "range { first: "<< result.first << " second: " << result.second << "}" << ELOG;
    }

    return result;
}

} } // namespace piel::lib
