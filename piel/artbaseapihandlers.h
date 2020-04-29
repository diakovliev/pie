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

#ifndef ARTBASEAPIHANDLERS_H
#define ARTBASEAPIHANDLERS_H

#include <string>
#include <sstream>
#include <map>
#include <memory>
#include <curleasyclient.hpp>

namespace art { namespace lib {

//! Base artifactory REST api handlers for the CurlEasyClient.
//! \sa CurlEasyHandlers
class ArtBaseApiHandlers
{
public:
    //! Interface for the callbacks. Callbacks can be set up by and will be called before IO operations.
    //! \sa set_before_input_callback, set_before_output_callback
    struct IBeforeCallback {
        virtual bool callback(ArtBaseApiHandlers *handlers) = 0;
    };

    //! Constructor.
    //! \param api_token Artifactory server REST api access token.
    ArtBaseApiHandlers(const std::string& api_token);

    //! Copy Constructor.
    //! \param .
    ArtBaseApiHandlers(const ArtBaseApiHandlers& handler);

    //! Destructor.
    virtual ~ArtBaseApiHandlers();

    //! Handler is used to create HTTP header what used during performing REST api call.
    //! \return Set of HTTP headers.
    virtual piel::lib::CurlEasyHandlers::headers_type custom_header();

    //! Handler is used to process HTTP headers returned by server during REST api call.
    //! \param ptr Pointer to libcurl internal buffer with header data.
    //! \param size Size of data block passed in libcurl internal buffer with header data.
    //! \return Size of data processed by handler.
    virtual size_t handle_header(char *ptr, size_t size);

    //! Handler is used to collect server response data returned by server during REST api call.
    //! \param ptr Pointer to libcurl internal buffer with data.
    //! \param size Size of data block.
    //! \return Size of data processed by handler.
    virtual size_t handle_output(char *ptr, size_t size);

    //! Handler is used to pass data into server during REST api call.
    //! \param ptr Pointer to libcurl internal buffer for data what must be passed to server.
    //! \param size Maximum size of data block what can be written in libcurl internal buffer.
    //! \return Size of the data block written in libcurl internal buffer. If the returned value
    //! is equal to size parameter, handle_input will be called again to collect next pease of data.
    virtual size_t handle_input(char *ptr, size_t size);

    //! Get input stream for reading server response data.
    //! \return Reference to stream what can be used for read data returned by server.
    virtual std::istringstream &responce_stream();

    //! Handler what will be called before after custom_header and before handle_input.
    virtual bool before_input();

    //! Handler what will be called before after handle_header and handle_output.
    virtual bool before_output();

    //! Set callback what will be called during before_input.
    //! \param callback Pointer to IBeforeCallback implementation instance.
    void set_before_input_callback(IBeforeCallback *callback);

    //! Set callback what will be called during before_output.
    //! \param callback Pointer to IBeforeCallback implementation instance.
    void set_before_output_callback(IBeforeCallback *callback);

    //! Get server returned HTTP headers.
    //! \return Map with server returned headers.
    std::map<std::string, std::string>& headers();

    //! Get server API token
    std::string api_token() const;

private:
    std::string api_token_;                         //!< Artifactory server REST api access token.
    std::string response_buffer_;                   //!< Buffer to collect server responce data.
    std::shared_ptr<std::istringstream> stream_;    //!< Input stream to read server responce data.
    std::map<std::string, std::string> headers_;    //!< Map with server responce HTTP headers.

    IBeforeCallback *before_input_callback_;        //!< Pointer to the callback instance.
    IBeforeCallback *before_output_callback_;       //!< Pointer to the callback instance.
};

} } // namespace art::lib


#endif // ARTBASEAPIHANDLERS_H
