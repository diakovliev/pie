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

#include <artbaseconstants.h>

namespace art { namespace lib {

/*static*/ const std::string ArtBaseConstants::rest_api_header__access_key          = "X-JFrog-Art-Api:";
/*static*/ const std::string ArtBaseConstants::rest_api_header__gavc_details        = "X-Result-Detail:";
/*static*/ const std::string ArtBaseConstants::rest_api_header__gavc_details_value  = "info, properties";

/*static*/ const std::string ArtBaseConstants::uri_delimiter                        = "/";
/*static*/ const std::string ArtBaseConstants::checksums_md5                        = "md5";
/*static*/ const std::string ArtBaseConstants::checksums_sha1                       = "sha1";
/*static*/ const std::string ArtBaseConstants::checksums_sha256                     = "sha256";

/*static*/ const std::string ArtBaseConstants::pom_classifier                       = ".pom";

/*static*/ const std::string ArtBaseConstants::rest_api__delete_request             = "DELETE";

/*static*/ const std::string ArtBaseConstants::aql_repo                             = "repo";
/*static*/ const std::string ArtBaseConstants::aql_name                             = "name";
/*static*/ const std::string ArtBaseConstants::aql_path                             = "path";
/*static*/ const std::string ArtBaseConstants::aql_md5                              = "actual_md5";
/*static*/ const std::string ArtBaseConstants::aql_sha1                             = "actual_sha1";
/*static*/ const std::string ArtBaseConstants::aql_sha256                           = "sha256";

} } // namespace art::lib
