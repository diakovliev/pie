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

#include "StringUtils.h"

#include <cctype>
#include <cstring>
#include <utility>
#include <algorithm>

namespace utils::string {

    std::string trim(const std::string& s) {
        std::string::const_iterator it = s.begin();
        while (it != s.end() && std::isspace(*it))
            it++;

        std::string::const_reverse_iterator rit = s.rbegin();
        while (rit.base() != it && std::isspace(*rit))
            rit++;

        return std::string(it, rit.base());
    }

    bool startswith(const std::string& s, const char pfx) {
        if (s.empty())
            return false;
        return s[0] == pfx;
    }

    bool startswith(const std::string& str, const std::string& start) {
        if (start.size() > str.size()) return false;
        return std::equal(start.begin(), start.end(), str.begin());
    }

    bool endswith(const std::string& str, const std::string& end) {
        if (end.size() > str.size()) return false;
        return std::equal(end.rbegin(), end.rend(), str.rbegin());
    }

    std::pair<std::string,std::string> split(const char delimiter, const std::string& s, bool do_trim, SplitDelimiterPolicy policy) {
        std::string::const_iterator it = s.begin();
        while (it != s.end() && *it != delimiter)
            it++;

        std::string::const_iterator itd = it;

        switch(policy) {
        case SplitDropDelimiter:
            if (itd != s.end())
                itd++;
        break;
        case SplitKeepDelimiterIn1st:
            if (it != s.end())
                it++;
            if (itd != s.end())
                itd++;
        break;
        case SplitKeepDelimiterIn2nd:
        default:;
        }

        if (do_trim) {
            return std::make_pair(
                    utils::string::trim(std::string(s.begin(), it)),
                    utils::string::trim(std::string(itd, s.end())));
        } else {
            return std::make_pair(
                    std::string(s.begin(), it),
                    std::string(itd, s.end()));
        }
    }

    std::vector<std::string> split2vec(const char delimiter, const std::string& s, bool do_trim, SplitDelimiterPolicy policy) {
        std::vector<std::string> ret;
        if (s.empty()) return ret;

        ret.reserve(10);
        std::string curr = s;
        do {
            auto sp = split(delimiter, curr, do_trim, policy);
            ret.push_back(sp.first);
            curr = sp.second;
        } while (!curr.empty());
        return ret;
    }

    std::string escaped(const std::string& s) {
        std::string res = "\"";
        res.reserve(s.size());
        std::for_each(s.begin(), s.end(), [&res](auto ch) {
            if (ch == '\"')
                res += "\\\"";
            else if (ch == '\\')
                res += "\\\\";
            else
                res.append(1, ch);
        });
        res += "\"";
        return res;
    }

}//namespace utils::string
