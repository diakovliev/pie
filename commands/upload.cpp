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

#include <upload.h>
#include <logging.h>
#include <fsindexer.h>
#include <artbaseapideployartifacthandlers.h>

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
    LOGT << "list:" << al::ufs::to_string(classifier_vector_) << ELOG;
    for (al::ufs::UFSVector::const_iterator it = classifier_vector_.begin(), end = classifier_vector_.end(); it != end; ++it) {
        LOGT << "element:" << al::ufs::to_string(*it) << ELOG;

        art::lib::ArtBaseApiDeployArtifactHandlers deploy_handlers(server_api_access_token_);
        deploy_handlers.set_url(server_url_);
        deploy_handlers.set_repo(server_repository_);
        deploy_handlers.set_path(query_.group());
        deploy_handlers.set_name(query_.name());
        deploy_handlers.set_version(query_.version());
        deploy_handlers.set_classifier(al::ufs::to_classifier(*it));
        deploy_handlers.file(it->file_name);

        LOGT << "--" << __LINE__ << "--" << ELOG;

        piel::lib::CurlEasyClient<art::lib::ArtBaseApiDeployArtifactHandlers> upload_client(deploy_handlers.gen_uri(), &deploy_handlers);
        LOGT << "--" << __LINE__ << "--" << ELOG;

        std::cout << "upload to here: " << deploy_handlers.gen_uri() << std::endl;

        if (!upload_client.perform())
        {
            LOGE << "Error on downloading file attempt!"        << ELOG;
            LOGE << upload_client.curl_error().presentation() << ELOG;
        }
        LOGT << "--" << __LINE__ << "--" << ELOG;
    }
    deploy_pom();
}

void Upload::deploy_pom()
{
    art::lib::ArtBaseApiDeployArtifactHandlers deploy_handlers(server_api_access_token_);
    deploy_handlers.generate_pom(server_url_, server_repository_, query_.group(), query_.name(), query_.version());

    piel::lib::CurlEasyClient<art::lib::ArtBaseApiDeployArtifactHandlers> upload_client(deploy_handlers.gen_uri(), &deploy_handlers);

    if (!upload_client.perform())
    {
        LOGE << "Error on downloading file attempt!"        << ELOG;
        LOGE << upload_client.curl_error().presentation() << ELOG;
    }
}

} } // namespace piel::cmd
