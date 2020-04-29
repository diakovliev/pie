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

#ifndef ARTBASEDOWNLOADHANDLERS_H
#define ARTBASEDOWNLOADHANDLERS_H

#include <artbaseapihandlers.h>
#include <checksumsdigestbuilder.hpp>

#include <string>
#include <ostream>
#include <functional>

namespace art { namespace lib {

class ArtBaseDownloadHandlers: public ArtBaseApiHandlers
{
public:
    struct BufferInfo
    {
        std::string id;
        size_t size;
    };

    ArtBaseDownloadHandlers(const std::string& api_token);
    ArtBaseDownloadHandlers(const std::string& api_token, std::ostream* dest);
    virtual ~ArtBaseDownloadHandlers();

    virtual size_t handle_output(char *ptr, size_t size);

    virtual piel::lib::ChecksumsDigestBuilder::StrDigests str_digests();

    void set_destination(std::ostream* dest);

    void set_id(const std::string& id)  { id_ = id; }
    std::string id() const              { return id_; }

    template<typename FuncObj>
    void connect(FuncObj obj) {
        on_buffer_ = obj;
    }

private:
    std::string id_;                                                //!< Download id.
    std::string api_token_;                                         //!< Artifactory server REST API access token.
    std::ostream* dest_;                                            //!< Destination stream.
    piel::lib::ChecksumsDigestBuilder checksums_builder_;           //!< Checksums digest builder for the content.
    std::function<void (const BufferInfo& m)> on_buffer_;           //!< Signal processed buffer
};

} } // namespace art::lib


#endif // ARTBASEDOWNLOADHANDLERS_H
