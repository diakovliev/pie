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

#ifndef PIEL_WORKINGCOPY_H_
#define PIEL_WORKINGCOPY_H_

#include <properties.h>
#include <index.h>
#include <indexesdiff.h>
#include <localdirectorystorage.h>

namespace piel { namespace lib {

namespace errors {
    struct init_existing_working_copy {};
    struct attach_to_non_working_copy {};
};

class WorkingCopy
{
public:
    typedef std::vector<IObjectsStorage::Ptr> Storages;
    static const int local_storage_index;

    WorkingCopy(const boost::filesystem::path& working_dir);
    ~WorkingCopy();

    static WorkingCopy init(const boost::filesystem::path& working_dir);
    static WorkingCopy attach(const boost::filesystem::path& working_dir);

    std::string checkout(const std::string& ref_to);
    std::string reset(const std::string& ref_to);
    std::string commit(const std::string& message, const std::string& ref_to);
    IndexesDiff diff(const std::string& ref_base = std::string()) const;

    void set_config(const std::string& name, const std::string& value);
    std::string get_config(const std::string& name, const std::string& default_value = std::string());

private:
    void init_filesystem();
    void attach_filesystem();
    void init_storages();

    boost::optional<Index> index_from_ref(const IObjectsStorage::Ptr& storage, const std::string& ref) const;

private:
    boost::filesystem::path working_dir_;
    boost::filesystem::path metadata_dir_;
    boost::filesystem::path storage_dir_;
    boost::filesystem::path reference_index_file_;
    boost::filesystem::path config_file_;
    Properties config_;                             //!< Working copy configuration parameters
    Storages storages_;                             //!< Local storages collection
    Index reference_index_;                         //!< Original index what checkouted into working copy
    Index current_index_;                           //!< Actual directory index (current directory state)
};

} } // namespace piel::lib

#endif /* PIEL_WORKINGCOPY_H_ */
