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

#pragma once

#include <gavcquery.h>
#include <uploadfilesspec.h>
#include <artdeployartifacthandlers.h>

#include <commands/base_errors.h>

#include "QueryContext.h"
#include "QueryOperation.h"

namespace piel::cmd {

    class Upload: public QueryOperation
    {
    public:
        Upload(const QueryContext *context);
        virtual ~Upload();

        void operator()();

        void set_classifiers(const al::ufs::UFSVector& classifiers);

    protected:

        static void upload_checksum_for(al::ArtDeployArtifactHandlers *deploy_handlers, const std::string& checksum_name);
        static void upload_checksums_for(al::ArtDeployArtifactHandlers *deploy_handlers);
        void setup_deploy_handlers_by_context(al::ArtDeployArtifactHandlers *deploy_handlers);
        void upload_object(std::string op_name, std::function<void(al::ArtDeployArtifactHandlers*)> setup_handlers);
        void deploy_pom();

    private:
        art::lib::ufs::UFSVector classifiers_vector_;
    };

} // namespace piel::cmd
