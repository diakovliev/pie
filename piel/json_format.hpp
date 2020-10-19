/*
 * Copyright (c) 2020, Dmytro Iakovliev daemondzk@gmail.com
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
#include <string_view>
#include <functional>

namespace json_format {

    namespace symbols {
        static constexpr const char* next = ",";
        static constexpr const char* assign = ":";
        static constexpr const char* scope_bra = "{";
        static constexpr const char* scope_ket = "}";
        static constexpr const char* list_bra = "[";
        static constexpr const char* list_ket = "]";
    };

    std::string escape(std::string_view s) {
        std::string o;
        for (auto c = s.cbegin(); c != s.cend(); c++) {
            switch (*c) {
            case '"': o.append("\\\""); break;
            case '\\': o.append("\\\\"); break;
            case '\b': o.append("\\b"); break;
            case '\f': o.append("\\f"); break;
            case '\n': o.append("\\n"); break;
            case '\r': o.append("\\r"); break;
            case '\t': o.append("\\t"); break;
            default:
                //if ('\x00' <= *c && *c <= '\x1f') {
                //    o << "\\u"
                //      << std::hex << std::setw(4) << std::setfill('0') << (int)*c;
                //} else {
                    o.append(1, *c);
                //}
            }
        }
        return o;
    }

    std::string string(std::string_view value) {
        std::string o;
        o.append("\"");
        o.append(escape(value));
        o.append("\"");
        return o;
    }

    template<class Oss>
    struct block {
        block(Oss& oss, std::string begin, std::string end)
            : oss_(oss), begin_(std::move(begin)), end_(std::move(end)), first_(true)
        {
            oss_ << begin_;
        }

        virtual ~block()
        {
            oss_ << end_;
        }

        template<typename Arg>
        block& operator<<(const Arg& arg)
        {
            oss_ << arg;
            return *this;
        }

        block& next() {
            if (first_) {
                first_ = false;
            } else {
                oss_ << symbols::next;
            }
            return *this;
        }

        Oss& oss() { return oss_; }

    private:
        Oss& oss_;
        std::string begin_;
        std::string end_;
        bool first_;

    };

    template<class Oss>
    class list: public block<Oss> {
    public:
        list(Oss& oss): block<Oss>(oss, symbols::list_bra, symbols::list_ket) {}
        virtual ~list() = default;

        template<typename Arg>
        list& add(Arg arg) {
            block<Oss>::next();
            block<Oss>::oss() << arg;
            return *this;
        }

        list& adds(const std::string& str) {
            block<Oss>::next();
            block<Oss>::oss() << string(str);
            return *this;
        }

        template<class Block>
        list& addb(std::function<void(Block& b)> bf, bool call_next = true) {
            if (call_next) block<Oss>::next();
            Block b(block<Oss>::oss());
            bf(b);
            return *this;
        }

    protected:
        list(Oss& oss, std::string begin, std::string end): block<Oss>(oss, begin, end) {}
    };

    template<class Oss>
    class scope: public list<Oss> {
    public:
        scope(Oss& oss): list<Oss>(oss, symbols::scope_bra, symbols::scope_ket) {}
        virtual ~scope() = default;

        template<typename Arg>
        scope& set(const std::string& key, const Arg& arg)
        {
            list<Oss>::next();
            list<Oss>::oss() << string(key) << symbols::assign;
            (*this) << arg;
            return *this;
        }

        scope& sets(const std::string& key, const std::string& arg)
        {
            list<Oss>::next();
            list<Oss>::oss() << string(key) << symbols::assign << string(arg);
            return *this;
        }

        template<class Block>
        scope& setb(const std::string& key, std::function<void(Block& b)> bf) {
            list<Oss>::next();
            list<Oss>::oss() << string(key) << symbols::assign;
            list<Oss>::template addb<Block>(bf, false);
            return *this;
        }

    protected:
        scope(Oss& oss, std::string begin, std::string end): list<Oss>(oss, begin, end) {}

    };

} // namespace json_format
