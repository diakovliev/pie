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

#include <artcreatedirectoryhandlers.h>

#include "artbaseconstants.h"
#include <cstring>

#include <logging.h>
#include <std_filesystem_ext.hpp>

#include <boost_property_tree_ext.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace pt = boost::property_tree;

//      custom_header,    handle_header,  handle_input,   handle_output,  before_input,   before_output)
CURLH_T_(art::lib::ArtCreateDirectoryHandlers,\
        true,             false,         true,           true,          false,          false);

namespace art { namespace lib {

ArtCreateDirectoryHandlers::ArtCreateDirectoryHandlers(const std::string& api_token)
    : ArtBaseDeployArtifactsHandlers(api_token)
{
    LOGT << ELOG;
}

ArtCreateDirectoryHandlers::ArtCreateDirectoryHandlers(const std::string& api_token, const std::string& url, const std::string& repo, const std::string& path)
    : ArtBaseDeployArtifactsHandlers(api_token, url, repo, path)
{
    LOGT << ELOG;
}

std::string ArtCreateDirectoryHandlers::get_path()
{
    std::string path = ArtBaseDeployArtifactsHandlers::get_path();
    if (path.compare(path.length() - ArtBaseConstants::uri_delimiter.length(), ArtBaseConstants::uri_delimiter.length(), ArtBaseConstants::uri_delimiter) != 0) {
        path.append(ArtBaseConstants::uri_delimiter);
    }
    return path;
}


} } // namespace art::lib
