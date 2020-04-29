/*
 * Copyright (c) 2017-2018
 *
 *  Dmytro Iakovliev daemondzk@gmail.com
 *  Oleksii Kogutenko https://github.com/oleksii-kogutenko
 *
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

#include <logging.h>
#include <artdeployartifactchecksumhandlers.h>
#include <artbaseconstants.h>

#include <boost_property_tree_ext.hpp>
#include <boost_filesystem_ext.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string.hpp>
#include <boost_property_tree_ext.hpp>

namespace pt = boost::property_tree;

//      custom_header,    handle_header,  handle_input,   handle_output,  before_input,   before_output)
CURLH_T_(art::lib::ArtDeployArtifactCheckSumHandlers,\
        true,             false,         true,            true,           false,          false);

namespace art { namespace lib {

std::string ArtDeployArtifactCheckSumHandlers::basic_digest_name(const std::string& digest) const
{
    if (digest == ArtBaseConstants::checksums_md5)
        return piel::lib::Md5::t::name();
    else if(digest == ArtBaseConstants::checksums_sha1)
        return piel::lib::Sha::t::name();
    else if (digest == ArtBaseConstants::checksums_sha256)
        return piel::lib::Sha256::t::name();
    else
        return "";
}

ArtDeployArtifactCheckSumHandlers::ArtDeployArtifactCheckSumHandlers(ArtDeployArtifactHandlers* processed_handler, const std::string& digest)
    : ArtDeployArtifactHandlers(processed_handler->api_token())
    , digest_(digest)
    , processed_handler_(processed_handler)
{
    piel::lib::ChecksumsDigestBuilder::StrDigests digests = processed_handler->str_digests();

    LOGT << "URI: " << processed_handler_->gen_uri()        << ELOG;
    LOGT << "Checksum URI: " << gen_uri()                   << ELOG;
    LOGT << "Checksum " << digest << ":" << digests[basic_digest_name(digest)] << ELOG;

    push_input_stream(std::make_shared<std::istringstream>(digests[basic_digest_name(digest)]));
}

/*virtual*/ ArtDeployArtifactCheckSumHandlers::~ArtDeployArtifactCheckSumHandlers()
{
}

/*virtual*/ std::string ArtDeployArtifactCheckSumHandlers::gen_uri()
{
    return processed_handler_->gen_uri().append(".").append(digest_);
}

} } // namespace art::lib
