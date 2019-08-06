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

#ifndef GAVCVERSIONSRANGEFILTER_H_
#define GAVCVERSIONSRANGEFILTER_H_

#include <vector>
#include <gavcquery.h>
#include <gavcversionsmatcher.h>
#include <gavcversionscomparator.h>

namespace art { namespace lib {

//! GAVC versions range filter.
//! @sa MavenMetadata, GavcQuery, art::lib::gavc::OpType
class GavcVersionsRangeFilter
{
public:
    //! Constructor.
    //!
    //! @param query_ops Query operations for creating filter.
    GavcVersionsRangeFilter(
            const std::vector<gavc::OpType>& query_ops,
            const std::vector<gavc::OpType>& query_ops_left,
            const std::vector<gavc::OpType>& query_ops_right,
            unsigned char flags);

    //! Destructor.
    ~GavcVersionsRangeFilter();

    //! Method will return filtered collection.
    //!
    //! @param versions Collection of versions to filter.
    //! @return Filtered sorted versions vector.
    std::vector<std::string> filtered(const std::vector<std::string>& versions);

    //! Method will return vector with versions what was filtered.
    //!
    //! @param versions Collection of versions to filter.
    //! @return Unsorted vector of the filtered elements.
    std::vector<std::string> filtered_out(const std::vector<std::string>& versions);

    //! Access to versions marcher used by filter.
    //!
    //! @return reference to versions marcher instance.
    GavcVersionsMatcher& matcher() const;

    //! Access to versions comparator used by filter.
    //!
    //! @return reference to versions comparator instance.
    GavcVersionsComparator& comparator() const;

protected:
    std::string border_version(const std::vector<gavc::OpType>& border_ops, const std::vector<std::string>& versions) const;

private:
    std::vector<gavc::OpType>   left_ops_;
    std::vector<gavc::OpType>   right_ops_;
    unsigned int                flags_;
    GavcVersionsMatcher         matcher_;       //!< Marcher
    GavcVersionsComparator      comparator_;    //!< Comparator
};

} } // namespace art::lib

#endif /* GAVCVERSIONSRANGEFILTER_H_ */
