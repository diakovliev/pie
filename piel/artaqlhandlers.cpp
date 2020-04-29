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

#include <artbaseconstants.h>
#include <artaqlhandlers.h>

#include <sstream>

//      custom_header,    handle_header,  handle_input,   handle_output,  before_input,   before_output)
CURLH_T_(art::lib::ArtAqlHandlers,\
        true,             false,          true,           true,           false,          true);

namespace art { namespace lib {

ArtAqlHandlers::ArtAqlHandlers(const std::string& api_token, const std::string& aql)
    : ArtBaseApiHandlers(api_token)
    , input_()
    , url_()
{
    input_.push_input_stream(boost::shared_ptr<std::istringstream>(new std::istringstream(aql)));
}

ArtAqlHandlers::~ArtAqlHandlers()
{

}

/*virtual*/ size_t ArtAqlHandlers::handle_input(char* ptr, size_t size)
{
    return input_.putto(ptr, size);
}

void ArtAqlHandlers::set_url(const std::string& url)
{
    url_ = url;
}

std::string ArtAqlHandlers::gen_uri() const
{
    return url_ + "/api/search/aql";
}

std::string ArtAqlHandlers::method() const
{
    return "POST";
}

} } // namespace art::lib
