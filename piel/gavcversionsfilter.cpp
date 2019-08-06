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

#include <gavcversionsfilter.h>
#include <gavcversionsmatcher.h>
#include <gavcversionscomparator.h>
#include <gavcfilterspredicates.hpp>

#include <logging.h>

#include <algorithm>

namespace art { namespace lib {

GavcVersionsFilter::GavcVersionsFilter(const std::vector<gavc::OpType>& query_ops)
    : query_ops_(query_ops)
    , matcher_(query_ops)
    , comparator_(query_ops)
{

}

GavcVersionsFilter::~GavcVersionsFilter()
{

}

std::vector<std::string> GavcVersionsFilter::filtered(const std::vector<std::string>& versions)
{
    std::vector<std::string> result = versions;

    // 1. Filter out all non matched versions.
    Match predicate(&matcher_);
    result.erase(std::remove_if(result.begin(), result.end(), Match::not_(predicate)), result.end());

    if (result.empty()) {
        return result;
    }

    // 2. Process significant parts. Build table <version, [significant parts]>
    SpartsTable sparts_table;
    for (std::vector<std::string>::const_iterator i = result.begin(), end = result.end(); i != end; ++i)
    {
        LOGT << "Table item for: " << *i << ELOG;
        sparts_table.push_back(std::make_pair(*i, matcher_.significant_parts(*i)));
    }

    // 3. For each query_ops_ filter out elements from sparts_table according to op
    std::vector<std::string>::size_type field_index = 0;
    for (std::vector<gavc::OpType>::const_iterator i = query_ops_.begin(), end = query_ops_.end(); i != end; ++i)
    {
        if (i->first != gavc::Op_const && i->first != gavc::Op_all)
        {
            LOGT << "Filtering for field: " << field_index << ELOG;

            SpartsTableComparator table_comparator(&comparator_, *i, field_index);
            SpartsTable::iterator element_to_keep = std::max_element(sparts_table.begin(), sparts_table.end(), table_comparator);

            if (element_to_keep != sparts_table.end()) {
                SpartsTableValuesFilter sparts_remove_filter(field_index, element_to_keep->second[field_index]);
                sparts_table.erase(std::remove_if(sparts_table.begin(), sparts_table.end(), sparts_remove_filter), sparts_table.end());
            }
            else
            {
                LOGF << "Can't find max element for field: " << field_index << ELOG;
            }
        }
        else
        {
            LOGT << "Skip filtering for field: " << field_index << ELOG;
        }

        if (i->first != gavc::Op_const) {
            ++field_index;
        }
    }

    // 4. Copy results into result
    result.erase(result.begin(),result.end());
    for (SpartsTable::const_iterator i = sparts_table.begin(), end = sparts_table.end(); i != end; ++i)
    {
        result.push_back(i->first);
    }

    // 5. Sort results
    std::sort(result.begin(), result.end(), comparator_);

    return result;
}

std::vector<std::string> GavcVersionsFilter::filtered_out(const std::vector<std::string>& versions)
{
    std::vector<std::string> result = versions;

    VectorContains filtered_contains_predicate(filtered(versions));
    result.erase(std::remove_if(result.begin(), result.end(), filtered_contains_predicate), result.end());

    return result;
}

} } // namespace art::lib
