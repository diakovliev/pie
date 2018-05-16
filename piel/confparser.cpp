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

#include <confparser.h>

#include <boost_property_tree_ext.hpp>
#include <boost/format.hpp>
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>


// git: {
//   id: test;
//   remote: ssh://$ldap_user$@git.test.org:repository.git;
//   local: test;
//   branch: master;
//   revision: 123456;
// };
// art: {
//   id: test2;
//   remote: https:://artifactory.server/artifactory;
//   local: test2;
//   repo: bin-release-local;
//   gavc: test:test2:+;
//   version: 123;
//   classifiers: {
//      debug: dbg;
//      release: prd;
//   };
// };

namespace piel { namespace lib {

namespace pt = boost::property_tree;

namespace conf {

    namespace qi = boost::spirit::qi;
    namespace ph = boost::phoenix;
    namespace ascii = boost::spirit::ascii;

    namespace constants {
        static const char vector_begin          = '{';
        static const char vector_end            = '}';
        static const char pair_delimiter        = ':';
        static const char vector_delimiter      = ';';

        static const std::string variable_begin = "${";
        static const std::string variable_end   = "}";
    }


    template<typename Iterator>
    struct conf_grammar: qi::grammar<Iterator> {

        conf_grammar(): conf_grammar::base_type(conf_tree_)
        {
            using qi::char_;
            using qi::skip;
            using qi::_1;
            using qi::_2;
            using ph::ref;
            using ph::val;
            using ph::push_back;
            using ph::insert;

            std::map<std::string,std::string> m;

//            pair_ss_ = (first_ > constants::pair_delimiter > second_)[insert(val(m), std::make_pair(_1,_2))];
//            pair_sv = first_ > constants::pair_delimiter > constants::vector_begin > ;

            conf_tree_ =
                    pair_ss_ % constants::vector_delimiter;
        }
    private:
        qi::rule<Iterator, std::string()>  first_;
        qi::rule<Iterator, std::string()>  second_;

        qi::rule<Iterator> pair_;                                   // ! Abstact pair
        qi::rule<Iterator, std::pair<std::string,std::string>() > pair_ss_;
        qi::rule<Iterator> pair_sv_;

        qi::rule<Iterator> conf_tree_;    //!< Consumer is property_tree.
    };

}

ConfParser::ConfParser()
{

}

ConfParser::~ConfParser()
{

}

/*static*/ boost::optional<pt::ptree> ConfParser::parse(const std::string data)
{
    if (data.empty())
        return boost::none;

    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    pt::ptree                                       conf;
    conf::conf_grammar<std::string::const_iterator> grammar;

    try {
        qi::phrase_parse( data.begin(), data.end(), grammar, ascii::space, conf );
    } catch (...) {
        return boost::none;
    }

    return conf;
}

} } // namespace piel::lib
