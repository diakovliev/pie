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

#include <gavcversionsrangefilter.h>

#include <gavcversionsfilter.h>
#include <gavcversionsmatcher.h>
#include <gavcversionscomparator.h>
#include <gavcfilterspredicates.hpp>

#include <logging.h>

#include <algorithm>

namespace art { namespace lib {

GavcVersionsRangeFilter::GavcVersionsRangeFilter(
        const std::vector<gavc::OpType>& query_ops,
        const std::vector<gavc::OpType>& query_ops_left,
        const std::vector<gavc::OpType>& query_ops_right,
        unsigned char flags)
    : left_ops_(query_ops_left)
    , right_ops_(query_ops_right)
    , flags_(flags)
    , matcher_(query_ops)
    , comparator_(query_ops)
{

}

GavcVersionsRangeFilter::~GavcVersionsRangeFilter()
{

}

std::string GavcVersionsRangeFilter::border_version(const std::vector<gavc::OpType>& border_ops, const std::vector<std::string>& versions) const
{
    GavcVersionsFilter border_filter(border_ops);

    std::vector<std::string> ret_vector = border_filter.filtered(versions);

    std::string result;

    if (!ret_vector.empty())
        result = ret_vector.front();

    return result;
}

std::vector<std::string> GavcVersionsRangeFilter::filtered(const std::vector<std::string>& versions)
{
    std::vector<std::string> filtered_versions = versions;

    Match predicate(&matcher_);
    filtered_versions.erase(std::remove_if(filtered_versions.begin(), filtered_versions.end(), Match::not_(predicate)), filtered_versions.end());

    if (filtered_versions.empty())
        return filtered_versions;

    std::vector<std::string> result;

    std::string left_version = border_version(left_ops_, filtered_versions);
    std::string right_version = border_version(right_ops_, filtered_versions);

    if (flags_ & gavc::RangeFlags_include_left && !left_version.empty()) {
        LOGT << "add left border value: " << left_version << ELOG;
        result.push_back(left_version);
    }

    std::for_each(filtered_versions.begin(), filtered_versions.end(), [&](auto i) {
        if (comparator_.compare(left_version, i) > 0 && comparator_.compare(right_version, i) < 0) {
            LOGT << "add value: " << i << ELOG;
            result.push_back(i);
        }
    });

    if (flags_ & gavc::RangeFlags_include_right && !right_version.empty()) {
        LOGT << "add right border value: " << right_version << ELOG;
        result.push_back(right_version);
    }

    std::sort(result.begin(), result.end(), comparator_);

    return result;
}

std::vector<std::string> GavcVersionsRangeFilter::filtered_out(const std::vector<std::string>& versions)
{
    std::vector<std::string> result = versions;

    VectorContains filtered_contains_predicate(filtered(versions));
    result.erase(std::remove_if(result.begin(), result.end(), filtered_contains_predicate), result.end());

    return result;
}

} } // namespace art::lib
