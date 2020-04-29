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

#ifndef PIEL_GAVCVERSIONSCOMPARATOR_H_
#define PIEL_GAVCVERSIONSCOMPARATOR_H_

#include <boost/assert.hpp>
#include <gavcversionsmatcher.h>

namespace art { namespace lib {

typedef long long CompareNumericType;

class GavcVersionsComparator
{
public:
    GavcVersionsComparator(const std::vector<gavc::OpType>& query_ops);
    ~GavcVersionsComparator();

    bool is_comparable(const std::string& lhs, const std::string& rhs) const;
    CompareNumericType compare(const std::string& lhs, const std::string& rhs) const;
    CompareNumericType compare_part(const std::string& lhs, const std::string& rhs) const;

    bool operator()(const std::string& lhs, const std::string& rhs) const {
        BOOST_ASSERT(is_comparable(lhs, rhs));
        return compare(lhs, rhs) > 0;
    }

private:
    GavcVersionsMatcher matcher_;

};

} } // namespace art::lib

#endif /* PIEL_GAVCVERSIONSCOMPARATOR_H_ */
