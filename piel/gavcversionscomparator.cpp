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

#include <gavcversionscomparator.h>
#include <logging.h>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>
#include <cctype>

namespace art { namespace lib {

struct Part {

    enum Type {
        Type_empty,
        Type_lexeme,
        Type_number
    };

    Part(const std::string& presentation)
        : presentation_(presentation)
    {

    }

    Type type() const {
        if (presentation_.empty())
            return Type_empty;

        try
        {
            boost::lexical_cast<long long>(presentation_);
            return Type_number;
        }
        catch (const boost::bad_lexical_cast&) {
            return Type_lexeme;
        }
    }

    template<typename T> T value() const
    {
        try
        {
            return boost::lexical_cast<T>(presentation_);
        }
        catch (const boost::bad_lexical_cast&) {
            return (T)0;
        }
    }

    const std::string& presentation() const
    {
        return presentation_;
    }

private:
    std::string presentation_;

};

template<>
std::string Part::value<std::string>() const
{
    return presentation_;
}

GavcVersionsComparator::GavcVersionsComparator(const std::vector<gavc::OpType>& query_ops)
    : matcher_(query_ops)
{

}

GavcVersionsComparator::~GavcVersionsComparator()
{

}

bool GavcVersionsComparator::is_comparable(const std::string& lhs, const std::string& rhs) const
{
    std::vector<std::string> lhs_parts = matcher_.significant_parts(lhs);
    std::vector<std::string> rhs_parts = matcher_.significant_parts(rhs);
    return lhs_parts.size() == rhs_parts.size();
}

CompareNumericType GavcVersionsComparator::compare_part(const std::string& lhs, const std::string& rhs) const
{
    int part_result = 0;
    Part lhs_part(lhs), rhs_part(rhs);

    if (lhs_part.type() == Part::Type_number && rhs_part.type() == Part::Type_number )
    {
        // numeric compare
        if (lhs_part.value<CompareNumericType>() != rhs_part.value<CompareNumericType>()) {
            part_result = lhs_part.value<CompareNumericType>() < rhs_part.value<CompareNumericType>() ? +1 : -1;
        }

        LOGT << "Versions corresponding numeric parts: "
                << lhs_part.presentation() << " "
                << rhs_part.presentation()
                << " part_result: " << part_result << ELOG;
    }
    else
    {
        // lexicographic compare
        if (lhs_part.presentation() != rhs_part.presentation())
        {
            if (boost::lexicographical_compare(std::make_pair(lhs_part.presentation().begin(), lhs_part.presentation().end()),
                    std::make_pair(rhs_part.presentation().begin(), rhs_part.presentation().end())))
            {
                part_result = +1;
            }
            else
            {
                part_result = -1;
            }
        }

        LOGT << "Versions corresponding lexicographic parts: "
                << lhs_part.presentation() << " "
                << rhs_part.presentation()
                << " part_result: " << part_result << ELOG;
    }

    return part_result;
}

CompareNumericType GavcVersionsComparator::compare(const std::string& lhs, const std::string& rhs) const
{
    int result = 0;

    LOGT << "Compare versions: " << lhs << " " << rhs << ELOG;

    if (lhs != rhs)
    {
        typedef std::vector<std::string>::const_iterator Iter;
        std::vector<std::string> lhs_parts = matcher_.significant_parts(lhs);
        std::vector<std::string> rhs_parts = matcher_.significant_parts(rhs);

        BOOST_ASSERT(lhs_parts.size() == rhs_parts.size());

        for(Iter lhs_i = lhs_parts.begin(), rhs_i = rhs_parts.begin(), lhs_end = lhs_parts.end(); lhs_i != lhs_end; ++lhs_i, ++rhs_i)
        {
            int part_result = compare_part(*lhs_i, *rhs_i);
            if (part_result != 0)
            {
                result = part_result;
                break;
            }
        }
    }

    LOGT << "Compare versions: " << lhs << " " << rhs << " result: " << result << ELOG;

    return result;
}

} } // namespace art::lib
