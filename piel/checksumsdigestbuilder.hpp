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

#ifndef CHECKSUM_DIGEST_BUILDER_HPP
#define CHECKSUM_DIGEST_BUILDER_HPP

#include <openssl/sha.h>
#include <openssl/md5.h>

#include <map>
#include <iostream>
#include <vector>
#include <memory>
#include <iomanip>
#include <sstream>

namespace piel { namespace lib {

////////////////////////////////////////////////////////////////////////////////
//! File constants and traits.
struct DigestConstants {
    static const size_t buf_size;   //!< Size of the internal IO buffers.
};

template<class Digest>
struct DigestTraits {
    typedef typename Digest::ctx ctx;
    static std::string name() {
        return name_;
    }
    static size_t len() {
        return len_;
    }
private:
    static const char* const name_;
    static const size_t len_;
};

struct Sha256 {
    typedef SHA256_CTX ctx;
    typedef DigestTraits<Sha256> t;
};
struct Sha {
    typedef SHA_CTX ctx;
    typedef DigestTraits<Sha> t;
};
struct Md5 {
    typedef MD5_CTX ctx;
    typedef DigestTraits<Md5> t;
};

////////////////////////////////////////////////////////////////////////////////
//! \brief Checksums formatter.
//! \param value_type Digest container value_type
//! Usage:
//! \code{.cpp}
//!  std::string str;
//!  std::for_each(digest.begin(), digest.end(), DigestFormatter(str));
//! \endcode
template<typename value_type> class DigestFormatter
{
public:
    //! Constructor.
    //! \param str reference to string what will be used to store format result.
    DigestFormatter(std::string& str)
        : str_(str)
    {}

    //! Functor called to collect checksum bytes.
    //! \param v checksum data byte.
    void operator()(const value_type& v)
    {
        std::ostringstream oss;
        oss << std::setfill('0');
        oss << std::setw(2);
        oss << std::hex << int(v & 0xFF);
        str_.append(oss.str());
    }

private:
    std::string& str_; //!< Reference to format result.
};

////////////////////////////////////////////////////////////////////////////////
//! Interface of C api & data structures wrappers which are used for work with
//! OpenSSL library.
struct IDigestContext {
    typedef unsigned char uchar;                            //!< Digest data container value_type.
    typedef std::vector<uchar> Digest;                      //!< Type of a digest data container.
    typedef DigestFormatter<Digest::value_type> Formatter;  //!< Type of a digest formatter.

    virtual ~IDigestContext() {}

    //! Init internal data.
    virtual void init() = 0;

    //! Process data block.
    //! \param data Pointer to a data block.
    //! \param size Data block size.
    virtual void update(const void *data, size_t size) = 0;

    //! Finalize calculations.
    //! \return Reference to digest data container
    virtual Digest& finalize() = 0;

    //! Get checksum name.
    //! \return Checksum name.
    virtual std::string name() const = 0;

    //! Format digest string.
    //! \return Digest string representation.
    static std::string format(const Digest& digest);
};

////////////////////////////////////////////////////////////////////////////////
//! Template for wrappers of C api & data structures used by OpenSSL library for
//! calculating checksums.
//! \param CTX Type of the OpenSSL api context structure.
//! \param digestSize the digest size in bytes.
template<typename CTX> class DigestContext
        : public IDigestContext
{
public:
    //! Constructor.
    DigestContext()
        : digest_(CTX::t::len())
        , ctx_()
    {}

    //! Init internal data.
    void init();

    //! Process data block.
    //! \param data Pointer to a data block.
    //! \param size Data block size.
    void update(const void *data, size_t size);

    //! Finalize calculations.
    //! \return Reference to digest data container
    Digest& finalize();

    //! Get checksum name.
    //! \return Checksum name.
    std::string name() const {
        return CTX::t::name();
    }

private:
    Digest digest_;                 //!< Digest data container.
    typename CTX::ctx ctx_;         //!< OpenSSL api context structure.
};

////////////////////////////////////////////////////////////////////////////////
//! SHA-256 specialization.
typedef DigestContext<Sha256> Sha256Context;

////////////////////////////////////////////////////////////////////////////////
//! SHA-1 specialization.
typedef DigestContext<Sha> ShaContext;

////////////////////////////////////////////////////////////////////////////////
//! MD5 specialization.
typedef DigestContext<Md5> Md5Context;

////////////////////////////////////////////////////////////////////////////////
//! Upper level class for checksums calculations.
//!
//! \sa DigestContext, Sha256Context, ShaContext, Md5Context.
class ChecksumsDigestBuilder {
public:

    typedef std::map<std::string, std::string> StrDigests;
    typedef std::map<std::string, IDigestContext::Digest> Digests;
    typedef std::vector<std::shared_ptr<IDigestContext> >::iterator CtxIter;

    //! Constructor
    ChecksumsDigestBuilder();

    //! Method will return istream.bad() after last digests_for(istream) str_digests_for(istream)
    //! call. Must be used to check if there are no IO errors during last calculation.
    //! \return istream.bad() result after calculations.
    //! \sa std::istream::bad()
    bool bad() const;

    //! Calculate checksums for input stream data.
    //! \param is Input stream to process.
    //! \return map of the digest containers.
    //! \sa bool bad() const
    Digests digests_for(std::istream& is);

    //! Calculate checksums for input stream data.
    //! \param is Input stream to process.
    //! \return map of the digest containers.
    //! \sa bool bad() const
    StrDigests str_digests_for(std::istream& is);

    //! Calculate checksums for string data.
    //! \param string String to process.
    //! \return map of the digest containers.
    Digests digests_for(const std::string& string);

    //! Calculate checksums for string data.
    //! \param string String to process.
    //! \return map of the digest containers.
    StrDigests str_digests_for(const std::string& string);

    void init();

    void update(const void *data, size_t size);

    void update(const std::string& string);

    template<class DigestsCollection> DigestsCollection finalize();

protected:
    void calculate_for_stream(std::istream& is);

    //! Reset internal state.
    void reset();

private:
    std::vector<std::shared_ptr<IDigestContext> > contexts_;  //!< Digests contexts.
    std::vector<std::istream::char_type> buf_;                  //!< Internal IO buffer.
    bool bad_;                                                  //!< Field to store istream.bad() result after calculations based on istream data.

};

} } // namespace piel::lib

#endif // CHECKSUM_DIGEST_BUILDER_HPP
