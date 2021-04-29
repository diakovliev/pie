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

namespace piel::cmd {

    namespace pl = piel::lib;


    Upload::Upload(const QueryContext *context)
        : QueryOperation(context, nullptr)
        , classifiers_vector_()
    {
    }


    Upload::~Upload()
    {
    }


    void Upload::set_classifiers(const al::ufs::UFSVector& classifiers)
    {
        classifiers_vector_ = classifiers;
    }


    /*static*/ void Upload::upload_checksum_for(al::ArtDeployArtifactHandlers *deploy_handlers, const std::string& checksum_name)
    {
        al::ArtDeployArtifactCheckSumHandlers deploy_checksum_handlers(deploy_handlers, checksum_name);

        pl::CurlEasyClient<al::ArtDeployArtifactHandlers>
                upload_checksum_client(deploy_checksum_handlers.gen_uri(), &deploy_checksum_handlers);

        LOGD << "Upload checksum: " << checksum_name << " to: " << deploy_checksum_handlers.gen_uri() << ELOG;

        try {
            upload_checksum_client.perform(true);
        } catch (...) {
            std::throw_with_nested(std::runtime_error("Error on upload file " + checksum_name + " checksum!"));
        }
    }


    /*static*/ void Upload::upload_checksums_for(al::ArtDeployArtifactHandlers *deploy_handlers)
    {
        upload_checksum_for(deploy_handlers, al::ArtBaseConstants::checksums_md5);
        upload_checksum_for(deploy_handlers, al::ArtBaseConstants::checksums_sha1);
        upload_checksum_for(deploy_handlers, al::ArtBaseConstants::checksums_sha256);
    }


    void Upload::setup_deploy_handlers_by_context(al::ArtDeployArtifactHandlers *deploy_handlers)
    {
        deploy_handlers->set_url(context()->server_url());
        deploy_handlers->set_repo(context()->repository());
        deploy_handlers->set_path(context()->query().group_path());
        deploy_handlers->set_name(context()->query().name());
        deploy_handlers->set_version(context()->query().version());
    }


    void Upload::upload_object(std::string op_name, std::function<void(al::ArtDeployArtifactHandlers*)> setup_handlers) {

        al::ArtDeployArtifactHandlers deploy_handlers(context()->token());

        setup_deploy_handlers_by_context(&deploy_handlers);

        setup_handlers(&deploy_handlers);

        pl::CurlEasyClient<al::ArtDeployArtifactHandlers> upload_client(deploy_handlers.gen_uri(), &deploy_handlers);

        LOGD << op_name << " to: " << deploy_handlers.gen_uri() << ELOG;

        try {
            upload_client.perform(true);
        } catch (...) {
            std::throw_with_nested(std::runtime_error("Error on '" + op_name + "' to: " + deploy_handlers.gen_uri() + "!"));
        }

        upload_checksums_for(&deploy_handlers);
    }


    void Upload::deploy_pom()
    {
        pl::MavenPom pom;
        pom.set_group(context()->query().group());
        pom.set_name(context()->query().name());
        pom.set_version(context()->query().version());

        std::ostringstream os;
        pom.store(os);

        upload_object("Upload POM", [&](auto *deploy_handlers) {
            deploy_handlers->set_classifier(al::ArtBaseConstants::pom_classifier);
            deploy_handlers->push_input_stream(std::make_shared<std::istringstream>(os.str()));
        });
    }


    void Upload::operator()()
    {
        LOGT << "Classifiers vector:" << al::ufs::to_string(classifiers_vector_) << ELOG;

        if (classifiers_vector_.empty())
        {
            throw std::runtime_error("Nothing to upload!");
        }

        for (al::ufs::UFSVector::const_iterator it = classifiers_vector_.begin(), end = classifiers_vector_.end(); it != end; ++it)
        {
            upload_object("Upload", [&](auto *deploy_handlers) {
                deploy_handlers->set_classifier(al::ufs::to_classifier(*it));
                deploy_handlers->file(it->file_name);
            });
        }

        deploy_pom();
    }

} // namespace piel::cmd
