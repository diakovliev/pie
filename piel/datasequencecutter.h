/*
 * Copyright (c) 2017-2018
 *
 *  Dmytro Iakovliev daemondzk@gmail.com
 *  Oleksii Kogutenko https://github.com/oleksii-kogutenko
 *
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

#ifndef DATASEQUENCECUTTER_H
#define DATASEQUENCECUTTER_H

#include <memory>
#include <queue>
#include <checksumsdigestbuilder.hpp>

namespace piel { namespace lib {

class DataSequenceCutter
{
public:
    typedef std::shared_ptr<std::istream> ISPtr;
    typedef std::queue<ISPtr> ISPtrQueue;

    DataSequenceCutter();
    ~DataSequenceCutter(){}

    void push_input_stream(ISPtr is);

    size_t putto(char* ptr, size_t size);

    piel::lib::ChecksumsDigestBuilder& digest_builder();

private:
    bool next();

private:
    ISPtrQueue                          is_queue_;
    ISPtr                               current_is_;
    piel::lib::ChecksumsDigestBuilder   digest_builder_;

};

} } // namespace art::lib

#endif // DATASEQUENCECUTTER_H
