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
        const std::string& left_version,
        const std::string& right_version,
        unsigned char flags)
    : left_version_(left_version)
    , right_version_(right_version)
    , flags_(flags)
    , matcher_(query_ops)
    , comparator_(query_ops)
{

}

GavcVersionsRangeFilter::~GavcVersionsRangeFilter()
{

}

std::pair<std::string, bool> GavcVersionsRangeFilter::border_version(const std::string& border_version, const std::vector<std::string>& versions) const
{
    bool exists = true;
    std::string result;

    if (GavcQuery::is_exact_version_query(border_version)) {

        exists = std::find(versions.begin(), versions.end(), border_version) != versions.end();
        result = border_version;

    } else if (GavcQuery::is_single_version_query(border_version)) {

        GavcVersionsFilter border_filter(*GavcQuery::query_version_ops(border_version));

        std::vector<std::string> ret_vector = border_filter.filtered(versions);

        if (!ret_vector.empty())
            result = ret_vector.front();
    }

    return std::make_pair(result, exists);
}

std::vector<std::string> GavcVersionsRangeFilter::filtered(const std::vector<std::string>& versions)
{
    std::vector<std::string> filtered_versions = versions;

    LOGT << "Filter versions" << ELOG;

    Match predicate(&matcher_);
    filtered_versions.erase(std::remove_if(filtered_versions.begin(), filtered_versions.end(), Match::not_(predicate)), filtered_versions.end());

    if (filtered_versions.empty())
        return filtered_versions;

    std::vector<std::string> result;

    LOGT << "Calculate borders" << ELOG;

    auto left_version = border_version(left_version_, filtered_versions);
    if (left_version.first.empty()) {
        LOGE << "Unable to find left border version in metadata! Border version must be exact or a single version query!" << ELOG;
        return result;
    }

    auto right_version = border_version(right_version_, filtered_versions);
    if (right_version.first.empty()) {
        LOGE << "Unable to find right border version in metadata! Border version must be exact or a single version query!" << ELOG;
        return result;
    }

    LOGT << "left version: " << left_version.first << ELOG;
    LOGT << "right version: " << right_version.first << ELOG;

    if (flags_ & gavc::RangeFlags_include_left && left_version.second) {
        LOGT << "add left border value: " << left_version.first << ELOG;
        result.push_back(left_version.first);
    }

    std::for_each(filtered_versions.begin(), filtered_versions.end(), [&](auto i) {
        LOGT << "check version: " << i << ELOG;
        if (comparator_.compare(left_version.first, i) > 0 && comparator_.compare(right_version.first, i) < 0) {
            LOGT << "add value: " << i << ELOG;
            result.push_back(i);
        }
    });

    if (flags_ & gavc::RangeFlags_include_right && right_version.second) {
        LOGT << "add right border value: " << right_version.first << ELOG;
        result.push_back(right_version.first);
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
