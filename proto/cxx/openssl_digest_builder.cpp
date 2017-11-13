#include <openssl/sha.h>
#include <openssl/md5.h>

#include <iostream>
#include <vector>
#include <algorithm>
#include <boost/format.hpp>

template<typename value_type> class DigestFormatter
{
public:
    DigestFormatter(std::string& str)
        : _str(str)
    {}

    void operator()(const value_type& v)
    {
        _str.append((boost::format("%1$02x") % (int)v).str());
    }

private:
    std::string& _str;
};

template<typename CTX, size_t digestSize> class DigestContext
{
public:
    typedef unsigned char uchar;

    typedef std::vector<uchar> Digest;
    typedef DigestFormatter<Digest::value_type> Formatter;

    static const size_t digest_size = digestSize;

    DigestContext()
        : _ctx()
        , _digest(digest_size)
    {}

    void init();
    void update(const void *data, size_t size);
    Digest& finalize();

private:
    Digest _digest;
    CTX _ctx;
};

// SHA-256
template<> void DigestContext<SHA256_CTX,SHA256_DIGEST_LENGTH>::init()
{
    SHA256_Init(&_ctx);
}
template<> void DigestContext<SHA256_CTX,SHA256_DIGEST_LENGTH>::update(const void *data, size_t size)
{
    SHA256_Update(&_ctx, data, size);
}
template<> DigestContext<SHA256_CTX,SHA256_DIGEST_LENGTH>::Digest& DigestContext<SHA256_CTX,SHA256_DIGEST_LENGTH>::finalize()
{
    SHA256_Final(_digest.data(), &_ctx);
    return _digest;
}

typedef DigestContext<SHA256_CTX,SHA256_DIGEST_LENGTH> Sha256Context;

// SHA-1
template<> void DigestContext<SHA_CTX,SHA_DIGEST_LENGTH>::init()
{
    SHA1_Init(&_ctx);
}
template<> void DigestContext<SHA_CTX,SHA_DIGEST_LENGTH>::update(const void *data, size_t size)
{
    SHA1_Update(&_ctx, data, size);
}
template<> DigestContext<SHA_CTX,SHA_DIGEST_LENGTH>::Digest& DigestContext<SHA_CTX,SHA_DIGEST_LENGTH>::finalize()
{
    SHA1_Final(_digest.data(), &_ctx);
    return _digest;
}
typedef DigestContext<SHA_CTX,SHA_DIGEST_LENGTH> ShaContext;

// MD5
template<> void DigestContext<MD5_CTX,MD5_DIGEST_LENGTH>::init()
{
    MD5_Init(&_ctx);
}
template<> void DigestContext<MD5_CTX,MD5_DIGEST_LENGTH>::update(const void *data, size_t size)
{
    MD5_Update(&_ctx, data, size);
}
template<> DigestContext<MD5_CTX,MD5_DIGEST_LENGTH>::Digest& DigestContext<MD5_CTX,MD5_DIGEST_LENGTH>::finalize()
{
    MD5_Final(_digest.data(), &_ctx);
    return _digest;
}
typedef DigestContext<MD5_CTX,MD5_DIGEST_LENGTH> Md5Context;

template<class DigestCtx> class ChecksumDigestBuilder
{
public:
    ChecksumDigestBuilder() {}

    typename DigestCtx::Digest digest_for(const std::string& string) const
    {
        DigestCtx context;
        context.init();
        context.update(string.c_str(), string.size());
        return context.finalize();
    }

    std::string str_digest_for(const std::string& string) const
    {
        std::string result;
        typename DigestCtx::Formatter resultFormatter(result);
        typename DigestCtx::Digest digest = digest_for(string);
        std::for_each(digest.begin(), digest.end(), resultFormatter);
        return result;
    }
};

typedef ChecksumDigestBuilder<Sha256Context> Sha256DigestBuilder;
typedef ChecksumDigestBuilder<ShaContext> ShaDigestBuilder;
typedef ChecksumDigestBuilder<Md5Context> Md5DigestBuilder;

int main(int argc, char** argv)
{
    std::string value = "1234";

    Sha256DigestBuilder builderSha256;
    std::cout << "sha-256: " << builderSha256.str_digest_for(value) << std::endl;

    ShaDigestBuilder builderSha;
    std::cout << "sha-1: " << builderSha.str_digest_for(value) << std::endl;

    Md5DigestBuilder builderMd5;
    std::cout << "md5: " << builderMd5.str_digest_for(value) << std::endl;
    return 0;
}

