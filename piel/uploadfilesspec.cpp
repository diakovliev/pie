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

#include <uploadfilesspec.h>

#include <logging.h>

#include <algorithm>

namespace art { namespace lib {

namespace ufs {

    namespace UFSConstants {
        static const char extension_prefix = '.';
        static const char delimiter        = ':';
        static const char vector_delimiter = ',';
    }

    std::string to_string(const files_spec_data& it)
    {
        std::ostringstream result;
        result << "[";
        if (it.classifier)
        {
            result << *it.classifier;
        }
        if (it.extension)
        {
            result << ufs::UFSConstants::extension_prefix;
            result << *it.extension;
        }
        if (!it.file_name.empty())
        {
            result << ufs::UFSConstants::delimiter;
            result << it.file_name;
        }
        result << "]";

        return  result.str();
    }

    std::string to_string(const UFSVector& data_)
    {
        std::ostringstream result;

        for (ufs::UFSVector::const_iterator it = data_.begin(), end=data_.end(); it != end; ++it) {
            if (it != data_.begin()) result << ufs::UFSConstants::vector_delimiter;
            result << to_string(*it);
        }

        LOGT << result.str() << ELOG;

        return result.str();
    }

    std::string to_classifier(const files_spec_data& it)
    {
        std::ostringstream result;
        if (it.classifier)
        {
            result << *it.classifier;
        }
        if (it.extension)
        {
            result << ufs::UFSConstants::extension_prefix;
            result << *it.extension;
        }

        return  result.str();
    }
} // namespace ufs

UploadFileSpec::UploadFileSpec()
    : data_()
{
}

UploadFileSpec::~UploadFileSpec()
{
}

std::optional<UploadFileSpec> UploadFileSpec::parse(const std::string& files_spec_str)
{
    if (files_spec_str.empty())
        return std::nullopt;

    UploadFileSpec result;

    auto data = parsers::upload::parse_upload_spec(files_spec_str);
    if (!data) {
        LOGE << "Can't parse input: '" << files_spec_str << "'!" << ELOG;
        return std::nullopt;
    }

    result.data_ = *data;

    return result;
}

std::string UploadFileSpec::to_string() const
{
    return ufs::to_string(data_);
}

} } // namespace art::lib
