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

#ifndef PIEL_INDEXESDIFF_H_
#define PIEL_INDEXESDIFF_H_

#include <treeindex.h>

namespace piel { namespace lib {

class IndexesDiff
{
public:
    enum ElementState
    {
        ElementState_unmodified,
        ElementState_removed,
        ElementState_added,
        ElementState_modified
    };

    typedef std::pair<
            TreeIndex::Content::value_type::second_type,
            TreeIndex::Content::value_type::second_type
            > ContentDiffValueType;

    typedef std::pair<
            ElementState,
            ContentDiffValueType
            > ContentDiffElement;

    typedef std::map<
            TreeIndex::Content::value_type::first_type,
            ContentDiffElement
            > ContentDiff;

    typedef std::pair<
            TreeIndex::Attributes::value_type::second_type,
            TreeIndex::Attributes::value_type::second_type
            > AttributesDiffValueType;

    typedef std::pair<
            ElementState,
            AttributesDiffValueType
            > AttributesDiffElement;

    typedef std::map<
            TreeIndex::Attributes::value_type::first_type,
            AttributesDiffElement
            > AttributesDiff;

    typedef std::pair<
            TreeIndex::ContentAttributes::value_type::second_type,
            TreeIndex::ContentAttributes::value_type::second_type
            > ContentAttributesDiffValueType;

    typedef std::pair<
            ElementState,
            ContentAttributesDiffValueType
            > ContentAttributesDiffElement;

    typedef std::map<
            TreeIndex::ContentAttributes::value_type::first_type,
            ContentAttributesDiffElement
            > ContentAttributesDiff;

    IndexesDiff();
    ~IndexesDiff();

    bool empty() const;

    bool different_content() const;
    bool different_attributes() const;
    bool different_content_attributes() const;

    const ContentDiff& content_diff() const;
    const AttributesDiff& attributes_diff() const;
    const ContentAttributesDiff& content_attributes_diff() const;
    AttributesDiff content_item_attributes_diff(const ContentAttributesDiff::const_iterator& element) const;

    static IndexesDiff diff(const TreeIndex::Ptr& first_index, const TreeIndex::Ptr& second_index);

    std::string format() const;

private:
    ContentDiff             content_diff_;
    AttributesDiff          attributes_diff_;
    ContentAttributesDiff   content_attributes_diff_;

};

} } // namespace piel::lib

#endif /* PIEL_INDEXESDIFF_H_ */
