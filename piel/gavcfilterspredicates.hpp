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

#include <vector>
#include <string>
#include <algorithm>

#include <logging.h>

#include <gavcversionsmatcher.h>
#include <gavcversionscomparator.h>

namespace art { namespace lib {

template<class Predicate, typename Arg>
struct Not {
    Not(Predicate& pred)
        : pred_(pred)
    {
    }

    bool operator()(Arg arg)
    {
        bool result = !pred_(arg);

        LOGT << "negated value: " << result << ELOG;

        return result;
    }

private:
    Predicate& pred_;

};

struct Match {

    typedef Not<Match, const std::string& > not_;

    Match(GavcVersionsMatcher *matcher)
        : matcher_(matcher)
    {
    }

    bool operator()(const std::string& val)
    {
        bool result = matcher_->match(val);

        LOGT << "match value: " << val << " result: " << result << ELOG;

        return result;
    }

private:
    GavcVersionsMatcher *matcher_;

};

typedef std::pair< std::string, std::vector<std::string> > SpartsTableElement;
typedef std::list< SpartsTableElement > SpartsTable;

struct SpartsTableComparator
{
    SpartsTableComparator(GavcVersionsComparator *comparator, const gavc::OpType& op, std::vector<std::string>::size_type field_index)
        : comparator_(comparator), op_(op), field_index_(field_index)
    {
    }

    bool operator()(const SpartsTableElement& lhs, const SpartsTableElement& rhs) const
    {
        switch (op_.first)
        {
        case gavc::Op_const:
        case gavc::Op_all:
            return false;
        case gavc::Op_latest:
            return comparator_->compare_part(lhs.second[field_index_], rhs.second[field_index_]);
        case gavc::Op_oldest:
            return !comparator_->compare_part(lhs.second[field_index_], rhs.second[field_index_]);
        }
        return false;
    }

private:
    GavcVersionsComparator              *comparator_;
    gavc::OpType                        op_;
    std::vector<std::string>::size_type field_index_;

};

struct SpartsTableValuesFilter
{
    SpartsTableValuesFilter(std::vector<std::string>::size_type field_index, const std::string& value)
        : field_index_(field_index)
        , value_(value)
    {
    }

    bool operator()(const SpartsTableElement& val) const
    {
        return val.second[field_index_] != value_;
    }

private:
    std::vector<std::string>::size_type field_index_;
    std::string                         value_;

};

struct VectorContains {

    VectorContains(const std::vector<std::string>& collection)
        : collection_(collection)
    {
    }

    bool operator()(const std::string& val) const
    {
        return std::find(collection_.begin(), collection_.end(), val) != collection_.end();
    }

private:
    std::vector<std::string> collection_;

};

} } // namespace art::lib
