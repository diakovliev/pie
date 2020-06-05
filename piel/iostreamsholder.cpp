/*
 * Copyright (c) 2018, Dmytro Iakovliev daemondzk@gmail.com
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

#include <logging.h>
#include <iostreamsholder.h>

namespace piel { namespace lib {

IOstreamsHolder::IOstreamsHolder()
    : cout_(nullptr), cerr_(nullptr), cin_(nullptr)
{
}

IOstreamsHolder::~IOstreamsHolder()
{
}

std::ostream& IOstreamsHolder::cout() const
{
    if (cout_) {
        LOGT << "Return custom cout." << ELOG;
        return *cout_;
    } else {
        LOGT << "Return std::cout." << ELOG;
        return std::cout;
    }
}

std::ostream& IOstreamsHolder::cerr() const
{
    if (cerr_) {
        LOGT << "Return custom cerr." << ELOG;
        return *cerr_;
    } else {
        LOGT << "Return std::cerr." << ELOG;
        return std::cerr;
    }
}

std::istream& IOstreamsHolder::cin() const
{
    if (cin_) {
        LOGT << "Return custom cin." << ELOG;
        return *cin_;
    } else {
        LOGT << "Return std::cin." << ELOG;
        return std::cin;
    }
}

void IOstreamsHolder::setup_iostreams(std::ostream *coutp, std::ostream *cerrp, std::istream *cinp)
{
    LOGT << "Setup custom streams." << ELOG;
    cout_   = coutp;
    cerr_   = cerrp;
    cin_    = cinp;
}

} } // namespace piel::lib
