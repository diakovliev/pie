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

#include <mavenmetadata.h>
#include <logging.h>

#include <boost_property_tree_ext.hpp>

#include <gavcversionsfilter.h>
#include <gavcversionsrangefilter.h>

namespace art { namespace lib {

namespace pt = boost::property_tree;

struct MavenMetadataConstants {
    static const std::string root_element;
    static const std::string group_element;
    static const std::string artifact_element;
    static const std::string version_element;
    static const std::string versioning_element;
    static const std::string versioning_latest_element;
    static const std::string versioning_release_element;
    static const std::string versioning_last_updated_element;
    static const std::string versioning_versions_element;
    static const std::string versioning_versions_version_element;
};

const std::string MavenMetadataConstants::root_element                          = "metadata";
const std::string MavenMetadataConstants::group_element                         = "groupId";
const std::string MavenMetadataConstants::artifact_element                      = "artifactId";
const std::string MavenMetadataConstants::version_element                       = "version";
const std::string MavenMetadataConstants::versioning_element                    = "versioning";
const std::string MavenMetadataConstants::versioning_latest_element             = "latest";
const std::string MavenMetadataConstants::versioning_release_element            = "release";
const std::string MavenMetadataConstants::versioning_last_updated_element       = "lastUpdated";
const std::string MavenMetadataConstants::versioning_versions_element           = "versions";
const std::string MavenMetadataConstants::versioning_versions_version_element   = "version";

MavenMetadata::MavenMetadata()
    : group_()
    , artifact_()
    , version_()
    , versioning_()
{

}

MavenMetadata::~MavenMetadata()
{
}

std::optional<MavenMetadata> MavenMetadata::parse(std::istream& is)
{
    MavenMetadata result;

    pt::ptree metadata_root;
    pt::read_xml(is, metadata_root);

    pt::ptree metadata = metadata_root.get_child(MavenMetadataConstants::root_element);

    std::optional<std::string> op_group       = pt::find_value(metadata,
            pt::FindPropertyHelper(MavenMetadataConstants::group_element));
    std::optional<std::string> op_artifact    = pt::find_value(metadata,
            pt::FindPropertyHelper(MavenMetadataConstants::artifact_element));
    std::optional<std::string> op_version     = pt::find_value(metadata,
            pt::FindPropertyHelper(MavenMetadataConstants::version_element));

    pt::ptree versioning = metadata.get_child(MavenMetadataConstants::versioning_element);

    std::optional<std::string> op_latest      = pt::find_value(versioning,
            pt::FindPropertyHelper(MavenMetadataConstants::versioning_latest_element));
    std::optional<std::string> op_release     = pt::find_value(versioning,
            pt::FindPropertyHelper(MavenMetadataConstants::versioning_release_element));
    std::optional<std::string> op_last_updated= pt::find_value(versioning,
            pt::FindPropertyHelper(MavenMetadataConstants::versioning_last_updated_element));

    pt::ptree versions = versioning.get_child(MavenMetadataConstants::versioning_versions_element);
    std::list<std::string> versions_list = pt::find_all_values(versions,
            pt::FindPropertyHelper(MavenMetadataConstants::versioning_versions_version_element));

    typedef std::list<std::string>::const_iterator ConstIter;
    for (ConstIter i = versions_list.begin(), end = versions_list.end(); i != end; ++i)
    {
        result.versioning_.versions_.push_back(*i);
    }

    bool have_errors = false;
    if (op_group)
    {
        result.group_ = *op_group;
    }
    else
    {
        LOGE << "There is no group element!" << ELOG;
        have_errors = true;
    }

    if (op_artifact)
    {
        result.artifact_ = *op_artifact;
    }
    else
    {
        LOGE << "There is no artifact element!" << ELOG;
        have_errors = true;
    }

    if (op_version)
    {
        result.version_ = *op_version;
    }
    else
    {
        LOGE << "There is no version element!" << ELOG;
        have_errors = true;
    }

    if (op_latest)
    {
        result.versioning_.latest_ = *op_latest;
    }
    else
    {
        LOGE << "There is no versioning.latest element!" << ELOG;
        have_errors = true;
    }

    if (op_release)
    {
        result.versioning_.release_ = *op_release;
    }
    else
    {
        LOGE << "There is no versioning.release element!" << ELOG;
        have_errors = true;
    }

    if (op_last_updated)
    {
        result.versioning_.last_updated_ = *op_last_updated;
    }
    else
    {
        LOGE << "There is no versioning.last_updated element!" << ELOG;
        have_errors = true;
    }

    if (result.versioning_.versions_.empty())
    {
        LOGE << "There are no versions!" << ELOG;
        have_errors = true;
    }

    // Trace
    LOGT << "metadata group id: "                  << result.group_                     << ELOG;
    LOGT << "metadata artifact id: "               << result.artifact_                  << ELOG;
    LOGT << "metadata version: "                   << result.version_                   << ELOG;
    LOGT << "metadata versioning latest: "         << result.versioning_.latest_        << ELOG;
    LOGT << "metadata versioning release: "        << result.versioning_.release_       << ELOG;
    LOGT << "metadata versioning last updated: "   << result.versioning_.last_updated_  << ELOG;
    typedef std::vector<std::string>::iterator Iter;
    for (Iter i = result.versioning_.versions_.begin(), end = result.versioning_.versions_.end(); i != end; ++i)
    {
        LOGT << "metadata versioning versions version: " << *i << ELOG;
    }

    if (!have_errors) {
        return result;
    } else {
        return std::nullopt;
    }
}

std::vector<std::string> MavenMetadata::versions_for(const GavcQuery& query) const
{
    return query.filtered(versioning_.versions_);
}

} } // namespace art::lib
