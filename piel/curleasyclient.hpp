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
#include <cstdlib>
#include <cstring>

#include <vector>
#include <string>
#include <sstream>

#define DEBUG_VERBOSE_CURL "debug_verbose_curl"

//! File contains implementation of libcurl curl_easy_* api wrapper. Wrapper is designed
//! for hiding C style api usage.

namespace piel { namespace lib {

//! Traits what are used to check what exact handlers are implemented in *Handlers class
//! implementation.
//! \param CurlEasyHandlers *Handlers implementation.
template<class CurlEasyHandlers>
struct CurlEasyHandlersTraits {
    static const bool have_custom_header;   //!< Implementation has custom_header handler.
    static const bool have_handle_header;   //!< Implementation has handle_header handler.
    static const bool have_handle_input;    //!< Implementation has handle_input handler.
    static const bool have_handle_output;   //!< Implementation has handle_output handler.
    static const bool have_before_input;    //!< Implementation has before_input handler.
    static const bool have_before_output;   //!< Implementation has before_output handler.
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
    //! \return if false will input operation will be aborted
    bool before_input()                             { return true; }

    //! Handler what will be called before after handle_header and handle_output.
    //! //! \return if false will output operation will be aborted
    bool before_output()                            { return true; }

};

struct CurlError {
    CurlError()
        : code_(CURLE_OK)
        , message_()
        , http_code_(200)
    {
    }

    CurlError(CURLcode code, long http_code, const std::string& message)
        : code_(code)
        , message_(message)
        , http_code_(http_code)
    {
    }

    CurlError(const CurlError& src)
        : code_(src.code_)
        , message_(src.message_)
        , http_code_(src.http_code_)
    {
    }

    CURLcode code() const
    {
        return code_;
    }

    std::string message() const
    {
        return message_;
    }

    long http_code() const
    {
        return http_code_;
    }

    std::string presentation() const
    {
        std::ostringstream oss;
        oss << "libcurl error code: ";
        oss << code_;
        oss << " http code: ";
        oss << http_code_;
        oss << " message: ";
        oss << message_;
        return oss.str();
    }

private:
    CURLcode code_;
    std::string message_;
    long http_code_;
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
    //! \param custom_request The CURLOPT_CUSTOMREQUEST value.
    //! \sa curl_easy_init
    CurlEasyClient(const std::string& url, HandlersPtr handlers, const std::string& custom_request = std::string())
        : url_(url)
        , handlers_(handlers)
        , errbuf_(CURL_ERROR_SIZE, ' ')
        , curl_error_()
        , custom_request_(custom_request)
    {
        curl_ = ::curl_easy_init();
    }

    //! Destructor. Will release internal libcurl handle.
    //! \sa curl_easy_cleanup
    ~CurlEasyClient()
    {
        ::curl_easy_cleanup(curl_);
    }

    //! Perform request.
    //! \return true if no errors, false otherwise.
    //! \sa curl_easy_perform, curl_error
    bool perform();

    //! Get CurlError structure.
    //! Can be used to determine error reason if false was resurned by perform.
    //! \return reference to internal CurlError.
    const CurlError& curl_error() const
    {
        return curl_error_;
    }

protected:
    static size_t handle_header(char *ptr, size_t size, size_t count, void* ctx);
    static size_t handle_write(char *ptr, size_t size, size_t count, void* ctx);
    static size_t handle_read(char *ptr, size_t size, size_t count, void* ctx);

private:
    std::string url_;               //!< Working url.
    ::CURL *curl_;                  //!< libcurl handle.
    HandlersPtr handlers_;          //!< Pointer to implementation instance of *Handlers.
    std::string errbuf_;            //!< libcurl error buffer
    CurlError curl_error_;          //!< libcurl error description

    std::string custom_request_;    //!< CURLOPT_CUSTOMREQUEST
};

template<class Handlers>
size_t CurlEasyClient<Handlers>::handle_header(char *ptr, size_t size, size_t count, void* ctx)
{
    CurlEasyClientPtr thiz = static_cast<CurlEasyClientPtr>(ctx);
    return thiz->handlers_->handle_header(ptr, size*count);
}

template<class Handlers>
size_t CurlEasyClient<Handlers>::handle_write(char *ptr, size_t size, size_t count, void* ctx)
{
    bool write_abort = false;

    CurlEasyClientPtr thiz = static_cast<CurlEasyClientPtr>(ctx);
    if (CurlEasyHandlersTraits<Handlers>::have_before_output) {
        write_abort = !thiz->handlers_->before_output();
    }

    if (write_abort)
        return 0;
    else
        return thiz->handlers_->handle_output(ptr, size*count);
}

template<class Handlers>
size_t CurlEasyClient<Handlers>::handle_read(char *ptr, size_t size, size_t count, void* ctx)
{
    bool read_abort = false;

    CurlEasyClientPtr thiz = static_cast<CurlEasyClientPtr>(ctx);
    if (CurlEasyHandlersTraits<Handlers>::have_before_input) {
        read_abort = !thiz->handlers_->before_input();
    }

    if (read_abort)
        return CURL_READFUNC_ABORT;
    else
        return thiz->handlers_->handle_input(ptr, size*count);
}

template<class Handlers>
bool CurlEasyClient<Handlers>::perform()
{
    const char* debug_verbose_curl = ::getenv(DEBUG_VERBOSE_CURL);
    if (debug_verbose_curl && 0 == ::strncmp(debug_verbose_curl, "1", 1)) {
        ::curl_easy_setopt(curl_, CURLOPT_VERBOSE, 1L);
    }

    ::curl_slist *custom_headers = 0;

    ::curl_easy_setopt(curl_, CURLOPT_URL, url_.c_str());
    if (!custom_request_.empty()) {
        ::curl_easy_setopt(curl_, CURLOPT_CUSTOMREQUEST, custom_request_.c_str());
    }
    if (CurlEasyHandlersTraits<Handlers>::have_custom_header) {
        CurlEasyHandlers::headers_type headers = handlers_->custom_header();
        typedef CurlEasyHandlers::headers_type::const_iterator Iter;
        for (Iter i = headers.begin(); i != headers.end(); ++i)
        {
            custom_headers = ::curl_slist_append(custom_headers, (*i).c_str());
        }
        ::curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, custom_headers);
    }
    if (CurlEasyHandlersTraits<Handlers>::have_handle_header) {
        ::curl_easy_setopt(curl_, CURLOPT_HEADERDATA, this);
        ::curl_easy_setopt(curl_, CURLOPT_HEADERFUNCTION, handle_header);
    }
    if (CurlEasyHandlersTraits<Handlers>::have_handle_output) {
        ::curl_easy_setopt(curl_, CURLOPT_WRITEDATA, this);
        ::curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, handle_write);
    }
    if (CurlEasyHandlersTraits<Handlers>::have_handle_input) {
        ::curl_easy_setopt(curl_, CURLOPT_READDATA, this);
        ::curl_easy_setopt(curl_, CURLOPT_READFUNCTION, handle_read);
        ::curl_easy_setopt(curl_, CURLOPT_UPLOAD, 1L);
    }
    ::curl_easy_setopt(curl_, CURLOPT_FAILONERROR, 1L);
    ::curl_easy_setopt(curl_, CURLOPT_ERRORBUFFER, errbuf_.data());
    CURLcode code = ::curl_easy_perform(curl_);

    long http_code = 0;
    ::curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &http_code);
    bool result = CURLE_OK == code && http_code < 400;
    if (!result)
    {
        curl_error_ = CurlError(code, http_code, errbuf_);
    }
    else
    {
        curl_error_ = CurlError(code, http_code, "OK");
    }

    if (custom_headers)
    {
        ::curl_slist_free_all(custom_headers);
    }

    return result;
}

} } // namespace piel::lib

#endif // CURLEASYCLIENT_H
