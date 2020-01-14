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

#ifndef UPLOAD_H_
#define UPLOAD_H_

#include <command.h>
#include <gavcquery.h>
#include <uploadfilesspec.h>
#include <artdeployartifacthandlers.h>

namespace piel { namespace cmd {

namespace errors {
    struct nothing_to_upload {};
    struct file_upload_error {};
    struct pom_upload_error {};
    struct uploading_checksum_error
    {
        uploading_checksum_error(const std::string& e) : error(e) {}
        std::string error;
    };
};

class Upload: public Command
{
public:
    Upload();
    virtual ~Upload();

    void operator()();

    const Upload* set_server_url(const std::string& url);
    const Upload* set_server_api_access_token(const std::string& token);
    const Upload* set_server_repository(const std::string& repo);
    const Upload* set_query(const art::lib::GavcQuery& query);
    const Upload* set_classifiers(const art::lib::ufs::UFSVector& classifiers);

    static void upload_checksums_for(art::lib::ArtDeployArtifactHandlers *deploy_handlers, std::string checksum_name);

protected:
    void deploy_pom();

private:
    std::string server_url_;
    std::string server_api_access_token_;
    std::string server_repository_;
    art::lib::GavcQuery query_;
    art::lib::ufs::UFSVector classifier_vector_;
};

} } // namespace piel::cmd

#endif /* COMMANDS_COMMIT_H_ */
