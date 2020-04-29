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

#include <checksumsdigestbuilder.hpp>
#include <commonconstants.h>
#include <logging.h>

namespace piel { namespace lib {

const size_t DigestConstants::buf_size = piel::lib::CommonConstants::io_buffer_size;       //!< Size of the internal IO buffers.

template<> char const* const DigestTraits<Sha256>::name_    = "SHA-256";
template<> const int DigestTraits<Sha256>::len_             = SHA256_DIGEST_LENGTH;
template<> char const* const DigestTraits<Sha>::name_       = "SHA-1";
template<> const int DigestTraits<Sha>::len_                = SHA_DIGEST_LENGTH;
template<> char const* const DigestTraits<Md5>::name_       = "MD5";
template<> const int DigestTraits<Md5>::len_                = MD5_DIGEST_LENGTH;

//! Format digest string.
//! \return Digest string representation.
std::string IDigestContext::format(const Digest& digest) {
    std::string result;
    Formatter resultFormatter(result);
    std::for_each(digest.begin(), digest.end(), resultFormatter);
    return result;
}

template<> void DigestContext<Sha256>::init()
{
    SHA256_Init(&ctx_);
}

template<> void DigestContext<Sha256>::update(const void *data, size_t size)
{
    SHA256_Update(&ctx_, data, size);
}

template<> DigestContext<Sha256>::Digest& DigestContext<Sha256>::finalize()
{
    SHA256_Final(digest_.data(), &ctx_);
    return digest_;
}

typedef DigestContext<Sha256> Sha256Context;

template<> void DigestContext<Sha>::init()
{
    SHA1_Init(&ctx_);
}

template<> void DigestContext<Sha>::update(const void *data, size_t size)
{
    SHA1_Update(&ctx_, data, size);
}

template<> DigestContext<Sha>::Digest& DigestContext<Sha>::finalize()
{
    SHA1_Final(digest_.data(), &ctx_);
    return digest_;
}

typedef DigestContext<Sha> ShaContext;

template<> void DigestContext<Md5>::init()
{
    MD5_Init(&ctx_);
}

template<> void DigestContext<Md5>::update(const void *data, size_t size)
{
    MD5_Update(&ctx_, data, size);
}

template<> DigestContext<Md5>::Digest& DigestContext<Md5>::finalize()
{
    MD5_Final(digest_.data(), &ctx_);
    return digest_;
}

//! Constructor
ChecksumsDigestBuilder::ChecksumsDigestBuilder()
    : contexts_()
    , buf_(DigestConstants::buf_size)
    , bad_(false)
{
    contexts_.push_back(std::make_shared<Sha256Context>());
    contexts_.push_back(std::make_shared<ShaContext>());
    contexts_.push_back(std::make_shared<Md5Context>());
}

bool ChecksumsDigestBuilder::bad() const
{
    return bad_;
}

void ChecksumsDigestBuilder::init()
{
    reset();
    for(CtxIter i = contexts_.begin(); i != contexts_.end(); i++) {
        (*i)->init();
    }
}

void ChecksumsDigestBuilder::update(const void *data, size_t size)
{
    for(CtxIter i = contexts_.begin(); i != contexts_.end(); i++) {
        (*i)->update(data, size);
    }
}

void ChecksumsDigestBuilder::update(const std::string& string)
{
    update(string.c_str(), string.size());
}

void ChecksumsDigestBuilder::calculate_for_stream(std::istream& is) {
    init();
    std::streamsize readed = 0;
    do {
        readed = is.read(buf_.data(), buf_.size()).gcount();
        if (readed != 0) {
            update(buf_.data(), readed);
        }
    } while(!is.eof() & !is.fail() & !is.bad());
    bad_ = is.bad();
}

//! Reset internal state.
void ChecksumsDigestBuilder::reset() {
    bad_ = false;
}

template<>
ChecksumsDigestBuilder::StrDigests
ChecksumsDigestBuilder::finalize<ChecksumsDigestBuilder::StrDigests>()
{
    ChecksumsDigestBuilder::StrDigests result;
    for(ChecksumsDigestBuilder::CtxIter i = contexts_.begin(); i != contexts_.end(); i++) {
        result.insert(std::make_pair((*i)->name(), (*i)->format((*i)->finalize())));
    }
    return result;
}

template<>
ChecksumsDigestBuilder::Digests
ChecksumsDigestBuilder::finalize<ChecksumsDigestBuilder::Digests>()
{
    ChecksumsDigestBuilder::Digests result;
    for(ChecksumsDigestBuilder::CtxIter i = contexts_.begin(); i != contexts_.end(); i++) {
        result.insert(std::make_pair((*i)->name(), (*i)->finalize()));
    }
    return result;
}

ChecksumsDigestBuilder::Digests ChecksumsDigestBuilder::digests_for(std::istream& is)
{
    calculate_for_stream(is);
    return finalize<Digests>();
}

ChecksumsDigestBuilder::StrDigests ChecksumsDigestBuilder::str_digests_for(std::istream& is)
{
    calculate_for_stream(is);
    return finalize<StrDigests>();
}

ChecksumsDigestBuilder::Digests ChecksumsDigestBuilder::digests_for(const std::string& string)
{
    init();
    update(string);
    return finalize<Digests>();
}

ChecksumsDigestBuilder::StrDigests ChecksumsDigestBuilder::str_digests_for(const std::string& string)
{
    init();
    update(string);
    return finalize<StrDigests>();
}

} } // namespace piel::lib

