/*
 * Copyright (c) 2021, diakovliev
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
 * THIS SOFTWARE IS PROVIDED BY diakovliev ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL diakovliev BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#pragma once

#include <cstring>
#include <string>
#include <vector>

namespace utils::string {

    std::string trim(const std::string& s);

    bool startswith(const std::string& s, const char pfx);
    bool startswith(const std::string& s, const std::string& pfx);
    bool endswith(const std::string& s, const std::string& sfx);

    enum SplitDelimiterPolicy {
        SplitDropDelimiter,
        SplitKeepDelimiterIn1st,
        SplitKeepDelimiterIn2nd,
    };

    std::pair<std::string,std::string> split(
            const char delimiter,
            const std::string& s,
            bool do_trim=true,
            SplitDelimiterPolicy policy = SplitDropDelimiter);

    std::vector<std::string> split2vec(
            const char delimiter,
            const std::string& s,
            bool do_trim=true,
            SplitDelimiterPolicy policy = SplitDropDelimiter);

    std::string escaped(const std::string& s);


    template <typename ...Args>
    std::string format(std::string format, Args && ...args)
    {
        auto size = std::snprintf(nullptr, 0, format.c_str(), std::forward<Args>(args)...);
        std::string output(size + 1, '\0');
        std::sprintf(&output[0], format.c_str(), std::forward<Args>(args)...);
        return output;
    }

}//namespace utils::string
