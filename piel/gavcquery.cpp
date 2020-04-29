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

#include <logging.h>
#include <gavcconstants.h>

#include <algorithm>
#include <boost/format.hpp>
#include <boost/bind.hpp>
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/split.hpp>

#if 0
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost_property_tree_ext.hpp>
#endif

#include <gavcversionsfilter.h>
#include <gavcversionsrangefilter.h>

#if 0
namespace pt = boost::property_tree;
#endif

//! Versions based queries
//
//  '*' - all
//  '+' - latest
//  '-' - oldest
//
// prefix(+|-|*\.)+suffix
//  - calculation from left to right
//    (+|-|*\.)(+|-) == (+|-) (single element)
//    (+|-|*\.)* == * (set)
//
// Pairs conversion matrix:
//     -------------
//     | + | - | * |
// -----------------
// | + | + | - | + |
// -----------------
// | - | - | - | - |
// -----------------
// | * | + | - | * |
// -----------------

BOOST_FUSION_ADAPT_STRUCT(
    art::lib::gavc::gavc_data,
    (std::string, group)
    (std::string, name)
    (std::string, version)
    (std::string, classifier)
    (std::string, extension)
)

BOOST_FUSION_ADAPT_STRUCT(
    art::lib::gavc::gavc_versions_range_data,
    (std::string, left)
    (std::string, right)
)

namespace art { namespace lib {

namespace gavc {

    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    template<typename Iterator>
    struct gavc_version_ops_grammar: qi::grammar<Iterator, std::vector<OpType>(), ascii::space_type> {
        gavc_version_ops_grammar(): gavc_version_ops_grammar::base_type(_ops)
        {
            using qi::char_;
            using qi::as_string;

            _op      = ops;
            _const   = as_string[ +( char_ - _op - GavcConstants::left_include_braket - GavcConstants::left_exclude_braket ) ];
            _ops     = *( (_op >> !_op) | _const );
        }

    private:
        struct ops_: qi::symbols<char,OpType> {
            ops_()
            {
                add_sym(GavcConstants::all_versions, Op_all);
                add_sym(GavcConstants::latest_version, Op_latest);
                add_sym(GavcConstants::oldest_version, Op_oldest);
            }

            void add_sym(const std::string& sym, Ops op)
            {
                add(sym, OpType(op, sym));
            }

            void add_sym(char ch, Ops op)
            {
                std::string sym;
                sym.push_back(ch);
                add(sym, OpType(op, sym));
            }

        } ops;

        qi::rule<Iterator, OpType()>                                    _op;        //!< Operation.
        qi::rule<Iterator, std::string()>                               _const;     //!< Constanst.
        qi::rule<Iterator, std::vector<OpType>(), ascii::space_type>    _ops;
    };

    // req         :req         :opt              :opt           @opt
    // <group.spec>:<name-spec>[:][<version.spec>[:{[classifier][@][extension]}]]
    template<typename Iterator>
    struct gavc_grammar: qi::grammar<Iterator, gavc_data(), ascii::space_type> {
        gavc_grammar(): gavc_grammar::base_type(_gavc)
        {

            using qi::char_;
            using qi::skip;

            _body            = +( char_ - GavcConstants::delimiter );
            _group           = _body > skip[ GavcConstants::delimiter ];
            _name            = _body > -( skip[ GavcConstants::delimiter ] );
            _version         = _body > -( skip[ GavcConstants::delimiter ] );
            _classifier      = +( char_ - GavcConstants::extension_prefix ) > -( skip[ GavcConstants::extension_prefix ] );
            _extension       = +( char_ );

            _gavc   =  _group
                    >  _name
                    >  -_version
                    >  -_classifier
                    >  -_extension
                    ;

        }

    private:
        qi::rule<Iterator, std::string()>                   _body;          //!< Helper.
        qi::rule<Iterator, std::string()>                   _group;         //!< Consumer is gavc_data.group .
        qi::rule<Iterator, std::string()>                   _name;          //!< Consumer is gavc_data.name .
        qi::rule<Iterator, std::string()>                   _version;       //!< Consumer is gavc_data.version .
        qi::rule<Iterator, std::string()>                   _classifier;    //!< Consumer is gavc_data.classifier .
        qi::rule<Iterator, std::string()>                   _extension;     //!< Consumer is gavc_data.extension .
        qi::rule<Iterator, gavc_data(), ascii::space_type>  _gavc;          //!< Consumer is gavc_data.
    };

    // <[|(><left>,<right><]|(>
    template<typename Iterator>
    struct gavc_versions_range_grammar: qi::grammar<Iterator, gavc_versions_range_data()> {
        gavc_versions_range_grammar()
            : gavc_versions_range_grammar::base_type(_data)
            , _flags(RangeFlags_exclude_all)
        {
            using qi::char_;
            using qi::skip;
            using qi::lit;
            using qi::lexeme;
            using qi::as_string;

            _left_braket    = lexeme[
                                lit(GavcConstants::left_include_braket)
                                    [
                                        boost::bind(&gavc_versions_range_grammar::include_left, this)
                                    ]
                                | lit(GavcConstants::left_exclude_braket)
                              ];

            _right_braket   = lexeme[
                                lit(GavcConstants::right_include_braket)
                                    [
                                        boost::bind(&gavc_versions_range_grammar::include_right, this)
                                    ]
                                | lit(GavcConstants::right_exclude_braket)
                              ];

            _left      = as_string[ +( char_ - GavcConstants::range_separator ) ];
            _right     = as_string[ +( char_ - _right_braket )];
            _data      = _left_braket > _left > GavcConstants::range_separator > _right > _right_braket;
        }

        unsigned char flags() const {
            return _flags;
        }

        void include_left() {
            _flags |= RangeFlags_include_left;
        }

        void include_right() {
            _flags |= RangeFlags_include_right;
        }

    private:
        qi::rule<Iterator>                              _left_braket;
        qi::rule<Iterator>                              _right_braket;

        qi::rule<Iterator, std::string()>               _left;
        qi::rule<Iterator, std::string()>               _right;
        qi::rule<Iterator, gavc_versions_range_data()>  _data;

        unsigned char _flags;
    };

} // namespace gavc

GavcQuery::GavcQuery()
    : data_()
{
}

GavcQuery::~GavcQuery()
{
}

boost::optional<GavcQuery> GavcQuery::parse(const std::string& gavc_str)
{
    if (gavc_str.empty())
        return boost::none;

    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    GavcQuery                                       result;
    gavc::gavc_grammar<std::string::const_iterator> grammar;

    try {
        qi::phrase_parse( gavc_str.begin(), gavc_str.end(), grammar, ascii::space, result.data_ );
    } catch (...) {
        return boost::none;
    }

    LOGT << "group: "      << result.group()        << ELOG;
    LOGT << "name: "       << result.name()         << ELOG;
    LOGT << "version: "    << result.version()      << ELOG;
    LOGT << "classifier: " << result.classifier()   << ELOG;
    LOGT << "extension: "  << result.extension()    << ELOG;

    // Attempt to parse versions query
    boost::optional<std::vector<gavc::OpType> > ops = result.query_version_ops();
    if (!ops) {
        return boost::none;
    }

    return result;
}

boost::optional<std::vector<gavc::OpType> > GavcQuery::query_version_ops() const
{
    return query_version_ops(data_.version);
}

std::pair<
    boost::optional<std::vector<gavc::OpType> >,
    boost::optional<gavc::gavc_versions_range_data>
> GavcQuery::query_version_data() const {
    return query_version_data(data_.version);
}

boost::optional<gavc::gavc_versions_range_data> GavcQuery::query_versions_range() const
{
    return query_versions_range(data_.version);
}

/* static */
std::pair<
    boost::optional<std::vector<gavc::OpType> >,
    boost::optional<gavc::gavc_versions_range_data>
> GavcQuery::query_version_data(const std::string& version) {

    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    std::vector<gavc::OpType> ops;
    gavc::gavc_versions_range_data range;

    if (version.empty()) {
        ops.push_back(gavc::OpType(gavc::Op_all, GavcConstants::all_versions));
        return std::make_pair(ops, boost::none);
    }

    gavc::gavc_version_ops_grammar<std::string::const_iterator> version_ops_grammar;
    gavc::gavc_versions_range_grammar<std::string::const_iterator> version_range_grammar;

    bool range_found = false;
    auto range_callback = [&]() { range_found = true; };

    try {
        bool matched = qi::phrase_parse(
                version.begin(),
                version.end(),
                version_ops_grammar > -version_range_grammar[ range_callback ],
                ascii::space,
                ops, range);

        if (!matched)
            return std::make_pair(boost::none, boost::none);

        if (range_found)
            range.flags = version_range_grammar.flags();

    } catch (...) {
        return std::make_pair(boost::none, boost::none);
    }

    if (!version.empty() & ops.empty())
    {
        LOGE << "version query: " << version << " has wrong syntax!" << ELOG;
        return std::make_pair(boost::none, boost::none);
    }

    std::for_each(ops.begin(), ops.end(), [&](auto i) {
        {
            LOGT << "version query op: " << i.second << ELOG;
        }
    });

    if (!range_found)
        return std::make_pair(ops, boost::none);

    LOGT << "versions range left: " << range.left << ELOG;
    LOGT << "versions range right: " << range.right << ELOG;
    LOGT << "versions range flags: " << int(range.flags) << ELOG;

    return std::make_pair(ops, range);
}

/* static */ boost::optional<std::vector<gavc::OpType> > GavcQuery::query_version_ops(const std::string& version)
{
    return query_version_data(version).first;
}

/* static */ boost::optional<gavc::gavc_versions_range_data> GavcQuery::query_versions_range(const std::string& version)
{
    return query_version_data(version).second;
}

struct QueryOpsFinder_exact_version_query {
    bool operator()(const gavc::OpType& op) {
        return op.first > gavc::Op_const;
    }
};

bool GavcQuery::is_exact_version_query() const
{
    return is_exact_version_query(data_.version);
}

/* static */ bool GavcQuery::is_exact_version_query(const std::string& version)
{
    auto range = query_versions_range(version);
    if (range) return false;

    auto pops = query_version_ops(version);
    if (!pops) return false;

    return std::find_if(pops->begin(), pops->end(), QueryOpsFinder_exact_version_query()) == pops->end();
}

bool GavcQuery::is_single_version_query() const
{
    return is_single_version_query(data_.version);
}

/* static */ bool GavcQuery::is_single_version_query(const std::string& version)
{
    auto range = query_versions_range(version);
    if (range) return false;

    auto pops = query_version_ops(version);
    if (!pops) return false;

    bool is_last_op_all = false;
    std::for_each(pops->begin(), pops->end(), [&](auto i)
    {
        if (i.first != gavc::Op_const)
           is_last_op_all = i.first == gavc::Op_all;
    });

    return !is_last_op_all;
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
    std::string g = group();
    std::vector<std::string> parts;
    boost::split(parts, g, boost::is_any_of("."));
    return boost::join(parts, "/");
}

std::string GavcQuery::format_maven_metadata_url(const std::string& server_url, const std::string& repository) const
{
    LOGT << "Build url for maven metadata. server_url: " << server_url << " repository: " << repository << ELOG;

    std::string group_path = group();
    std::replace(group_path.begin(), group_path.end(), GavcConstants::group_delimiter, GavcConstants::path_delimiter);
    std::string result = boost::str(boost::format( "%2$s%1$c%3$s%1$c%4$s%1$c%5$s%1$c%6$s" )
        % GavcConstants::path_delimiter % server_url % repository % group_path % name() % GavcConstants::maven_metadata_filename);

    LOGT << "Maven metadata url: " << result << ELOG;
    return result;
}

std::string GavcQuery::format_maven_metadata_path(const std::string& repository) const
{
    LOGT << "Build path for maven metadata. repository: " << repository << ELOG;

    std::string group_path = group();
    std::replace(group_path.begin(), group_path.end(), GavcConstants::group_delimiter, GavcConstants::path_delimiter);
    std::string result = boost::str(boost::format( "%1$c%2$s%1$c%3$s%1$c%4$s" )
        % GavcConstants::path_delimiter % repository % group_path % name());

    LOGT << "Cache path to metadata url: " << result << ELOG;
    return result;
}

std::vector<std::string> GavcQuery::filter(const std::vector<std::string>& versions) const {
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
    std::ostringstream oss;
    oss << group_path();
    oss << "/";
    oss << name();
    oss << "/*";
    return oss.str();
}

std::string GavcQuery::to_aql_name() const
{
    bool append_star = classifier().empty() | extension().empty();
    std::ostringstream oss;
    oss << name();
    oss << "-";
    oss << version();
    if (!classifier().empty())
    {
        oss << "-";
        oss << classifier();
    }
    if (!extension().empty())
    {
        oss << ".";
        oss << extension();
    }
    if (append_star)
    {
        oss << "*";
    }
    return oss.str();
}

std::string GavcQuery::to_aql(const std::string& repo) const
{
    std::ostringstream oss;

//#if 0
    oss << "items.find(";
        oss << "{";

        oss << " \"repo\" :";
            oss << " \"";
            oss << repo;
            oss << "\" ";

        oss << ",";
        oss << " \"path\" :";
            oss << " { \"$match\": \"";
            oss << to_aql_path();
            oss << "\" } ";

        oss << ",";
        oss << " \"name\" :";
            oss << " { \"$match\": \"";
            oss << to_aql_name();
            oss << "\" } ";

        oss << "}";
    oss << ").include(\"*\")";
//#endif

#if 0
    std::ostringstream qoss;
    pt::ptree query, path_match, name_match;

    path_match.put("$match", to_aql_path());
    name_match.put("$match", to_aql_name());

    query.put("repo", repo);
    query.add_child("path", path_match);
    query.add_child("name", name_match);

    pt::write_json(qoss, query, false);

    std::string aql_json_query = qoss.str();
    LOGT << "aql json query: " << aql_json_query << ELOG;

    oss << "items.find(" << aql_json_query << ").include(\"*\")";
#endif

    std::string aql_query = oss.str();
    LOGT << "aql query: " << aql_query << ELOG;

    return aql_query;
}

} } // namespace art::lib
