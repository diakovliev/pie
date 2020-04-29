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

#include <datasequencecutter.h>

#include <cstring>
#include <logging.h>

#include <boost_property_tree_ext.hpp>
#include <std_filesystem_ext.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/trim.hpp>

namespace pt = boost::property_tree;

namespace piel { namespace lib {

DataSequenceCutter::DataSequenceCutter()
    : is_queue_()
    , current_is_()
    , digest_builder_()
{
    digest_builder_.init();
}

void DataSequenceCutter::push_input_stream(DataSequenceCutter::ISPtr is)
{
    is_queue_.push(is);
}

bool DataSequenceCutter::next()
{
    bool res = !is_queue_.empty();
    if (res) {
        current_is_ = is_queue_.front();
        is_queue_.pop();
    }
    return res;
}

piel::lib::ChecksumsDigestBuilder& DataSequenceCutter::digest_builder()
{
    return digest_builder_;
}

size_t DataSequenceCutter::putto(char* ptr, size_t size)
{
    size_t filled_size = 0;

    while (filled_size < size) {
        size_t readed = 0;

        if (current_is_) {
            readed += boost::numeric_cast<size_t>(current_is_->read(ptr + filled_size,
                                        boost::numeric_cast<std::streamsize>(size - filled_size)).gcount());

            if (readed > 0) {
                digest_builder_.update(ptr + filled_size, readed);
            }
        }

        if (!readed){
            if (next()) {
                continue;
            }
            else {
                return filled_size;
            }
        }

        filled_size += readed;
    }

    return filled_size;
}

} } // namespace art::lib
