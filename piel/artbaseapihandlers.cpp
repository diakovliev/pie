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
#include <artbaseapihandlers.h>
#include <logging.h>

#include <algorithm>
#include <boost/algorithm/string.hpp>

//      custom_header,    handle_header,  handle_input,   handle_output,  before_input,   before_output)
CURLH_T_(art::lib::ArtBaseApiHandlers,\
        true,             false,          false,          true,           false,          false);

namespace art { namespace lib {

ArtBaseApiHandlers::ArtBaseApiHandlers(const std::string& api_token)
    : api_token_(api_token)
    , response_buffer_()
    , stream_()
    , before_input_callback_(0)
    , before_output_callback_(0)
{

}

ArtBaseApiHandlers::ArtBaseApiHandlers(const ArtBaseApiHandlers& handler)
    : api_token_(handler.api_token_)
    , response_buffer_(handler.response_buffer_)
    , stream_(handler.stream_)
    , headers_(handler.headers_)
    , before_input_callback_(handler.before_input_callback_)
    , before_output_callback_(handler.before_output_callback_)
{
}

/*virtual*/ ArtBaseApiHandlers::~ArtBaseApiHandlers()
{

}

/*virtual*/ piel::lib::CurlEasyHandlers::headers_type ArtBaseApiHandlers::custom_header()
{
    piel::lib::CurlEasyHandlers::headers_type result;
    result.push_back(std::string(ArtBaseConstants::rest_api_header__access_key).append(api_token_));
    return result;
}

/*virtual*/ size_t ArtBaseApiHandlers::handle_header(char *ptr, size_t size)
{
    std::string headers(ptr, size);
    LOGT << "headers: " << headers << ELOG;

    std::string::iterator sepa = std::find_if(headers.begin(), headers.end(), boost::is_any_of(":"));
    if (sepa == headers.end()) {
        return size;
    }

    std::string name;
    name.append(headers.begin(), sepa);
    boost::trim(name);

    std::string value;
    ++sepa;
    if (sepa != headers.end()) {
        value.append(sepa, headers.end());
        boost::trim(value);
    }

    LOGT << name << "= " << value << ELOG;
    headers_.insert(std::make_pair(name,value));

    return size;
}

/*virtual*/ bool ArtBaseApiHandlers::before_input()
{
    bool result = true;

    // Call & reset callback pointer
    if (before_input_callback_) {
        result = before_input_callback_->callback(this);
        before_input_callback_ = 0;
    }

    return result;
}

/*virtual*/ bool ArtBaseApiHandlers::before_output()
{
    bool result = true;

    // Call & reset callback pointer
    if (before_output_callback_) {
        result = before_output_callback_->callback(this);
        before_output_callback_ = 0;
    }

    return result;
}

void ArtBaseApiHandlers::set_before_input_callback(ArtBaseApiHandlers::IBeforeCallback *callback)
{
    before_input_callback_ = callback;
}

void ArtBaseApiHandlers::set_before_output_callback(ArtBaseApiHandlers::IBeforeCallback *callback)
{
    before_output_callback_ = callback;
}

std::map<std::string, std::string>& ArtBaseApiHandlers::headers()
{
    return headers_;
}

/*virtual*/ size_t ArtBaseApiHandlers::handle_output(char *ptr, size_t size)
{
    response_buffer_.append(ptr, ptr + size);
    LOGT << "response: " << response_buffer_ << ELOG;
    return size;
}

/*virtual*/ size_t ArtBaseApiHandlers::handle_input(char *ptr, size_t size)
{
    // Prepare command parameters
    return -1;
}

/*virtual*/ std::istringstream &ArtBaseApiHandlers::responce_stream()
{
    stream_ = std::make_shared<std::istringstream>(response_buffer_);
    return *stream_.get();
}

std::string ArtBaseApiHandlers::api_token() const
{
    return api_token_;
}

} } // namespace art::lib
