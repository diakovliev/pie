/*
 * Copyright (c) 2018, diakovliev
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
 * THIS SOFTWARE IS PROVIDED BY diakovliev ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL diakovliev BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <checkout.h>
#include <fsindexer.h>
#include <indextofsexporter.h>
#include <boost_filesystem_ext.hpp>

namespace piel { namespace cmd {

Checkout::Checkout(const piel::lib::WorkingCopy::Ptr& working_copy, const std::string& ref_to)
    : WorkingCopyCommand(working_copy)
    , ref_to_(ref_to)
    , force_(false)
{
}

Checkout::~Checkout()
{
}

const Checkout* Checkout::set_force(bool force)
{
    force_ = force;
    return this;
}

std::string Checkout::operator()()
{
    piel::lib::Index reference_index = working_copy()->reference_index();

    if (piel::lib::AssetId::empty != working_copy()->local_storage()->resolve(ref_to_))
    {
        if (!force_)
        {
            // Check for non commit changes
            piel::lib::Index current_index = piel::lib::FsIndexer::build(working_copy()->working_dir(), working_copy()->metadata_dir());

            piel::lib::IndexesDiff diff = piel::lib::IndexesDiff::diff(reference_index, current_index);
            if (!diff.empty())
            {
                throw errors::there_are_non_commited_changes();
            }
        }

        // Remove working directory content (exclude metadata)
        boost::filesystem::remove_directory_content(working_copy()->working_dir(), working_copy()->metadata_dir());

        // Export data from index
        boost::optional<piel::lib::Index> ref_index = piel::lib::Index::from_ref(working_copy()->local_storage(), ref_to_);
        reference_index = *ref_index;

        piel::lib::IndexToFsExporter index_exporter(reference_index, piel::lib::ExportPolicy__replace_existing);
        index_exporter.export_to(working_copy()->working_dir());
    }

    // Update working copy reference
    working_copy()->update_reference(ref_to_, reference_index);
    return working_copy()->reference_index().self().id().string();
}

} } // namespace piel::cmd
