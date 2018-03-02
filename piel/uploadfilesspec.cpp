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

#include <uploadfilesspec.h>

#include <logging.h>
#include <gavcconstants.h>

#include <algorithm>
#include <boost/format.hpp>
#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/qi.hpp>

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
    art::lib::ufs::files_spec_data,
    (std::string, classifier)
    (std::string, extension)
    (std::string, file_name)
)

namespace art { namespace lib {

namespace ufs {

    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    namespace UFSConstants {
        static const char extension_prefix = '.';
        static const char delimiter        = ':';
        static const char vector_delimiter = ',';
    }

    // req         :req         :opt              :opt           @opt
    // [[classifier][.][extension]]:<file_name>
    template<typename Iterator>
    struct ufs_grammar: qi::grammar<Iterator, UFSVector(), ascii::space_type> {
        ufs_grammar(): ufs_grammar::base_type(_fsdv)
        {

            using qi::char_;
            using qi::skip;

            _classifier      = +( char_ - (char_(UFSConstants::extension_prefix)|char_(UFSConstants::delimiter)) );
            _extension       = +( char_ - UFSConstants::delimiter );
            _file_name       = +( char_ - UFSConstants::vector_delimiter );

            _fsd   = -_classifier
                   > -( UFSConstants::extension_prefix > _extension )
                   > UFSConstants::delimiter > _file_name
                   ;

            _fsdv = _fsd % UFSConstants::vector_delimiter;
        }
    private:
        qi::rule<Iterator, std::string()>                   _file_name;         //!< Consumer is gavc_data.name .
        qi::rule<Iterator, std::string()>                   _classifier;        //!< Consumer is gavc_data.classifier .
        qi::rule<Iterator, std::string()>                   _extension;         //!< Consumer is gavc_data.extension .
        qi::rule<Iterator, files_spec_data(), ascii::space_type>  _fsd;         //!< Consumer is gavc_data.
        qi::rule<Iterator, UFSVector(), ascii::space_type>  _fsdv;    //!< Consumer is gavc_data.
    };

} // namespace gavc

UploadFileSpec::UploadFileSpec()
    : data_()
{
}

UploadFileSpec::~UploadFileSpec()
{
}

boost::optional<UploadFileSpec> UploadFileSpec::parse(const std::string& gavc_str)
{
    LOGT << __LINE__ << ELOG;
    if (gavc_str.empty())
        return boost::none;

    namespace qi = boost::spirit::qi;
    namespace ascii = boost::spirit::ascii;

    UploadFileSpec               result;
    ufs::ufs_grammar<std::string::const_iterator>   grammar;

    LOGT << __LINE__ << ELOG;
    try {
        LOGT << __LINE__ << ELOG;
        qi::phrase_parse( gavc_str.begin(), gavc_str.end(), grammar, ascii::space, result.data_ );
        LOGT << __LINE__ << ELOG;
    } catch (...) {
        LOGT << __LINE__ << ELOG;
        return boost::none;
    }

    return result;
}

std::string UploadFileSpec::to_string() const
{
	std::ostringstream result;
    for (ufs::UFSVector::const_iterator it = data_.begin(), end=data_.end(); it != end; ++it) {
        if (it != data_.begin()) result << ufs::UFSConstants::vector_delimiter;
        result << "[";
        if (!it->classifier.empty())
        {
            result << it->classifier;
        }
        if (!it->extension.empty())
        {
            result << ufs::UFSConstants::extension_prefix;
            result << it->extension;
        }
        if (!it->file_name.empty())
        {
            result << ufs::UFSConstants::delimiter;
            result << it->file_name;
        }
        result << "]";
    }

	LOGT << result.str() << ELOG;

    return result.str();
}

} } // namespace art::lib
