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

#include <gavcversionsmatcher.h>
#include <logging.h>
#include <algorithm>

namespace art { namespace lib {

GavcVersionsMatcher::GavcVersionsMatcher(const std::vector<gavc::OpType>& query_ops)
    : regex_(create_regex(query_ops))
{
}

GavcVersionsMatcher::~GavcVersionsMatcher()
{
}

bool GavcVersionsMatcher::match(const std::string& version) const
{
    bool result = std::regex_match(version, regex_);

    LOGT << "match version: " << version << " result: " << result << ELOG;

    return result;
}

std::vector<std::string> GavcVersionsMatcher::significant_parts(const std::string& version) const
{
    std::vector<std::string> result;

    std::smatch what;
    if (std::regex_match(version, what, regex_)) {
        result.resize(what.size() - 1);
        std::copy(++what.begin(), what.end(), result.begin());
    }

    return result;
}

const std::regex& GavcVersionsMatcher::regex()
{
    return regex_;
}

std::string GavcVersionsMatcher::escape_string(const std::string& str_to_escape) const
{
    const std::regex esc("[.^$|()\\[\\]{}*+?\\\\]");
    const std::string rep("\\\\&");
    return std::regex_replace(str_to_escape, esc, rep,
            std::regex_constants::match_default | std::regex_constants::format_sed);
}

std::regex GavcVersionsMatcher::create_regex(const std::vector<gavc::OpType>& query_ops) const
{
    std::ostringstream regex_content;

    regex_content << "^";

    for(std::vector<gavc::OpType>::const_iterator i = query_ops.begin(), end = query_ops.end(); i != end; ++i)
    {
        switch (i->first) {
        case gavc::Op_const:
        {
            regex_content << escape_string(i->second);
            break;
        }
        case gavc::Op_all:
        case gavc::Op_latest:
        case gavc::Op_oldest:
        {
            // Lazy all
            regex_content << "(.+)";
        }
        }
    }

    regex_content << "$";

    LOGT << "created regex: " << regex_content.str() << ELOG;

    return std::regex(regex_content.str());
}

} } // namespace art::lib
