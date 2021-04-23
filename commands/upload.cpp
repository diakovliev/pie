/*
 * Copyright (c) 2017-2018
 *
 *  Dmytro Iakovliev daemondzk@gmail.com
 *  Oleksii Kogutenko https://github.com/oleksii-kogutenko
 *
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

#include <upload.h>
#include <logging.h>
#include <mavenpom.h>
#include <artbaseconstants.h>
#include <artdeployartifactchecksumhandlers.h>
#include <artdeployartifacthandlers.h>

namespace al = art::lib;

namespace piel { namespace cmd {

Upload::Upload()
    : server_url_()
    , server_api_access_token_()
    , server_repository_()
    , query_()
    , classifier_vector_()
{
}

Upload::~Upload()
{
}

// Also used by Push
/*static*/ void Upload::upload_checksums_for(art::lib::ArtDeployArtifactHandlers *deploy_handlers, std::string checksum_name)
{
    art::lib::ArtDeployArtifactCheckSumHandlers deploy_checksum_handlers(deploy_handlers, checksum_name);
    piel::lib::CurlEasyClient<art::lib::ArtDeployArtifactHandlers>
            upload_checksum_client(deploy_checksum_handlers.gen_uri(), &deploy_checksum_handlers);

    LOGD << "Upload checksum: " << checksum_name << " to: " << deploy_checksum_handlers.gen_uri() << ELOG;

    try {
        upload_checksum_client.perform(true);
    } catch (...) {
        std::throw_with_nested(std::runtime_error("Error on upload file " + checksum_name + " checksum!"));
    }
}

const Upload* Upload::set_server_url(const std::string& url)
{
    server_url_ = url;
    return this;
}

const Upload* Upload::set_server_api_access_token(const std::string& token)
{
    server_api_access_token_ = token;
    return this;
}

const Upload* Upload::set_server_repository(const std::string& repo)
{
    server_repository_ = repo;
    return this;
}

const Upload* Upload::set_query(const art::lib::GavcQuery &query)
{
    query_ = query;
    return this;
}

const Upload* Upload::set_classifiers(const art::lib::ufs::UFSVector& classifiers)
{
    classifier_vector_ = classifiers;
    return this;
}

void Upload::operator()()
{
    LOGT << "Classifiers vector:" << al::ufs::to_string(classifier_vector_) << ELOG;

    if (classifier_vector_.empty())
    {
        throw std::runtime_error("Nothing to upload!");
    }

    for (al::ufs::UFSVector::const_iterator it = classifier_vector_.begin(), end = classifier_vector_.end(); it != end; ++it)
    {
        art::lib::ArtDeployArtifactHandlers deploy_handlers(server_api_access_token_);

        deploy_handlers.set_url(server_url_);
        deploy_handlers.set_repo(server_repository_);
        deploy_handlers.set_path(query_.group_path());
        deploy_handlers.set_name(query_.name());
        deploy_handlers.set_version(query_.version());
        deploy_handlers.set_classifier(al::ufs::to_classifier(*it));
        deploy_handlers.file(it->file_name);

        piel::lib::CurlEasyClient<art::lib::ArtDeployArtifactHandlers> upload_client(deploy_handlers.gen_uri(), &deploy_handlers);

        LOGD << "Upload: " << al::ufs::to_string(*it) << " to: " << deploy_handlers.gen_uri() << ELOG;

        try {
            upload_client.perform(true);
        } catch (...) {
            std::throw_with_nested(std::runtime_error("Error on upload file: " + it->file_name + "!"));
        }

        upload_checksums_for(&deploy_handlers, art::lib::ArtBaseConstants::checksums_md5);
        upload_checksums_for(&deploy_handlers, art::lib::ArtBaseConstants::checksums_sha1);
        upload_checksums_for(&deploy_handlers, art::lib::ArtBaseConstants::checksums_sha256);
    }

    deploy_pom();
}

void Upload::deploy_pom()
{
    piel::lib::MavenPom pom;
    pom.set_group(query_.group());
    pom.set_name(query_.name());
    pom.set_version(query_.version());

    std::ostringstream os;
    pom.store(os);

    art::lib::ArtDeployArtifactHandlers deploy_handlers(server_api_access_token_);

    deploy_handlers.set_url(server_url_);
    deploy_handlers.set_repo(server_repository_);
    deploy_handlers.set_path(query_.group_path());
    deploy_handlers.set_name(query_.name());
    deploy_handlers.set_version(query_.version());
    deploy_handlers.set_classifier(art::lib::ArtBaseConstants::pom_classifier);
    deploy_handlers.push_input_stream(std::make_shared<std::istringstream>(os.str()));

    piel::lib::CurlEasyClient<art::lib::ArtDeployArtifactHandlers> upload_client(deploy_handlers.gen_uri(), &deploy_handlers);

    LOGD << "Upload POM to: " << deploy_handlers.gen_uri() << ELOG;

    try {
        upload_client.perform(true);
    } catch (...) {
        std::throw_with_nested(std::runtime_error("Error on upload POM to: " + deploy_handlers.gen_uri() + "!"));
    }

    upload_checksums_for(&deploy_handlers, art::lib::ArtBaseConstants::checksums_md5);
    upload_checksums_for(&deploy_handlers, art::lib::ArtBaseConstants::checksums_sha1);
    upload_checksums_for(&deploy_handlers, art::lib::ArtBaseConstants::checksums_sha256);
}

} } // namespace piel::cmd
