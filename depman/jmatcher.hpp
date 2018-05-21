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

#pragma once

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace java { namespace matchers {

enum Ops {
    Const,
    Char,       // ?
    Sequence,   // *
    Groups,     // **
};

struct Op {
    Ops op;
    std::string data;
};

typedef std::vector<std::vector<Op> > Operations;

struct PathMatcher {

    typedef boost::shared_ptr<PathMatcher> Ptr;

    static void fill_ops(Operations& v, const std::string& q)
    {
        std::string data;
        Operations::value_type curr_group;

        for (auto i = q.begin(), end = q.end(); i != end; ++i)
        {
            if (*i == '\\' || *i == '/')
            {
                if (!data.empty()) 
                {
                    Op op;
                    op.data = data;
                    op.op = Ops::Const;
                    curr_group.push_back(op);
                    data = "";
                }

                v.push_back(curr_group);
                curr_group.clear();
            }
            else if (*i == '*')
            {
                auto n = i + 1;
                if (n != end && *n == '*')
                {
                    Op op;
                    op.op = Ops::Groups;
                    curr_group.push_back(op);
                    i = n;
                } 
                else 
                {
                    Op op;
                    op.op = Ops::Sequence;
                    curr_group.push_back(op);
                }
                if (i == end) break;
            }
            else if (*i == '?')
            {
                Op op;
                op.op = Ops::Char;
                curr_group.push_back(op);
            }
            else
            {
                data += *i;
            }
        }
        if (!data.empty())
        {
            Op op;
            op.data = data;
            op.op = Ops::Const;
            curr_group.push_back(op);
            data = "";
        }
        if (!curr_group.empty())
        {
            v.push_back(curr_group);
        }
    }

    static Ptr compile(const std::string& q)
    {
        Ptr result_(new PathMatcher());
        fill_ops(result_->ops_, q);
        return result_;
    }

    bool match(const std::string& path)
    {
        
    }

    Operations& ops()
    {
        return ops_;
    }

private:
    Operations ops_;

};

} } // namespace java::matcher
