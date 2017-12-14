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

#ifndef CURLEASYCLIENT_H
#define CURLEASYCLIENT_H

#include <curl/curl.h>

#include <vector>
#include <string>

//! File contains implementation of libcurl curl_easy_* api wrapper. Wrapper is designed
//! for hiding C style api usage.

namespace piel { namespace lib {

//! Traits what are used to check what exact handlers are implemented in *Handlers class
//! implementation.
//! \param CurlEasyHandlers *Handlers implementation.
template<class CurlEasyHandlers>
struct CurlEasyHandlersTraits {
    static const bool have_custom_header;   //!< Implemantation has custom_header handler.
    static const bool have_handle_header;   //!< Implemantation has handle_header handler.
    static const bool have_handle_input;    //!< Implemantation has handle_input handler.
    static const bool have_handle_output;   //!< Implemantation has handle_output handler.
    static const bool have_before_input;    //!< Implemantation has before_input handler.
    static const bool have_before_output;   //!< Implemantation has before_output handler.
};

// Macroses to define handlers traits.
#define CURLH_T_ITEM_(CLASS,TRAIT,VALUE) \
    template<> const bool piel::lib::CurlEasyHandlersTraits< CLASS > :: TRAIT = VALUE

#define CURLH_T_(CLASS,custom_header,handle_header,handle_input,handle_output,before_input,before_output) \
    CURLH_T_ITEM_(CLASS,have_custom_header, custom_header);  \
    CURLH_T_ITEM_(CLASS,have_handle_header, handle_header);  \
    CURLH_T_ITEM_(CLASS,have_handle_input,  handle_input);   \
    CURLH_T_ITEM_(CLASS,have_handle_output, handle_output);  \
    CURLH_T_ITEM_(CLASS,have_before_input,  before_input);   \
    CURLH_T_ITEM_(CLASS,have_before_output, before_output)

//! Propotype for *Handlers classes.
struct CurlEasyHandlers {
    typedef std::vector<std::string> headers_type;  //!< Type used for HTTP headers storages.

    //! Handler to create custom HTTP header for query.
    headers_type custom_header()                    { return headers_type(); }

    //! Handler to get HTTP header returned by server.
    //! \param ptr Pointer to libcurl internal buffer with header data.
    //! \param size Size of data block passed in libcurl internal buffer with header data.
    //! \return Size of data processed bu handler.
    size_t handle_header(char *ptr, size_t size)    { return CURLE_WRITE_ERROR; }

    //! Handler to retieve data returned by server (responce body).
    //! \param ptr Pointer to libcurl internal buffer with data block retrieved from server.
    //! \param size Size of data block.
    //! \return Size of data processed by handler.
    size_t handle_output(char *ptr, size_t size)    { return CURLE_WRITE_ERROR; }

    //! Handler to send data to server (request body).
    //! \param ptr Pointer to libcurl internal buffer for data what must be send to server.
    //! \param size Maximum size of data block what can be written in libcurl internal buffer.
    //! \return Size of the data block written in libcurl internal buffer. If the returned value
    //! is equal to size parameter, handle_input will be called again to collect next pease of data.
    size_t handle_input(char *ptr, size_t size)     { return CURLE_READ_ERROR; }

    //! Handler what will be called before after custom_header and before handle_input.
    void before_input()                             { }

    //! Handler what will be called before after handle_header and handle_output.
    void before_output()                            { }

};

//! libcurl curl_easy_* api wrapper.
//! \param Handlers Type of the implementation of *Handlers.
template<class Handlers>
class CurlEasyClient {
public:
    typedef CurlEasyClient* CurlEasyClientPtr;  //!< Type of the pointer to template instantiation type.
    typedef Handlers* HandlersPtr;              //!< Type of the pointer to implementation of *Handlers.

    //! Constructor. Will init internal libcurl handle.
    //! \param url Working url.
    //! \param handlers Pointer to implementation instance of *Handlers.
    //! \sa curl_easy_init
    CurlEasyClient(const std::string& url, HandlersPtr handlers)
        : _url(url)
        , _handlers(handlers)
    {
        _curl = ::curl_easy_init();
    }

    //! Destructor. Will release internal libcurl handle.
    //! \sa curl_easy_cleanup
    ~CurlEasyClient()
    {
        ::curl_easy_cleanup(_curl);
    }

    //! Perform request.
    //! \return libcurl error code.
    //! \sa curl_easy_perform
    CURLcode perform();

protected:
    static size_t handle_header(char *ptr, size_t size, size_t count, void* ctx);
    static size_t handle_write(char *ptr, size_t size, size_t count, void* ctx);
    static size_t handle_read(char *ptr, size_t size, size_t count, void* ctx);

private:
    std::string _url;       //!< Working url.
    ::CURL *_curl;          //!< libcurl handle.
    HandlersPtr _handlers;  //!< Pointer to implementation instance of *Handlers.
};

template<class Handlers>
size_t CurlEasyClient<Handlers>::handle_header(char *ptr, size_t size, size_t count, void* ctx)
{
    CurlEasyClientPtr thiz = static_cast<CurlEasyClientPtr>(ctx);
    return thiz->_handlers->handle_header(ptr, size*count);
}

template<class Handlers>
size_t CurlEasyClient<Handlers>::handle_write(char *ptr, size_t size, size_t count, void* ctx)
{
    CurlEasyClientPtr thiz = static_cast<CurlEasyClientPtr>(ctx);
    if (CurlEasyHandlersTraits<Handlers>::have_before_output) {
        thiz->_handlers->before_output();
    }
    return thiz->_handlers->handle_output(ptr, size*count);
}

template<class Handlers>
size_t CurlEasyClient<Handlers>::handle_read(char *ptr, size_t size, size_t count, void* ctx)
{
    CurlEasyClientPtr thiz = static_cast<CurlEasyClientPtr>(ctx);
    if (CurlEasyHandlersTraits<Handlers>::have_before_input) {
        thiz->_handlers->before_input();
    }
    return thiz->_handlers->handle_input(ptr, size*count);
}

template<class Handlers>
CURLcode CurlEasyClient<Handlers>::perform()
{
    ::curl_easy_setopt(_curl, CURLOPT_URL, _url.c_str());
    if (CurlEasyHandlersTraits<Handlers>::have_custom_header) {
        ::curl_slist *chunk = 0;
        CurlEasyHandlers::headers_type headers = _handlers->custom_header();
        typedef CurlEasyHandlers::headers_type::const_iterator Iter;
        for (Iter i = headers.begin(); i != headers.end(); ++i)
        {
            chunk = ::curl_slist_append(chunk, (*i).c_str());
        }
        // TODO: process errors
        ::curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, chunk);
    }
    if (CurlEasyHandlersTraits<Handlers>::have_handle_header) {
        ::curl_easy_setopt(_curl, CURLOPT_HEADERDATA, this);
        ::curl_easy_setopt(_curl, CURLOPT_HEADERFUNCTION, handle_header);
    }
    if (CurlEasyHandlersTraits<Handlers>::have_handle_output) {
        ::curl_easy_setopt(_curl, CURLOPT_WRITEDATA, this);
        ::curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, handle_write);
    }
    if (CurlEasyHandlersTraits<Handlers>::have_handle_input) {
        ::curl_easy_setopt(_curl, CURLOPT_READDATA, this);
        ::curl_easy_setopt(_curl, CURLOPT_READFUNCTION, handle_read);
        ::curl_easy_setopt(_curl, CURLOPT_UPLOAD, 1L);
    }
    curl_easy_setopt(_curl, CURLOPT_VERBOSE, 1L);
    return ::curl_easy_perform(_curl);
}

} } // namespace piel::lib

#endif // CURLEASYCLIENT_H
