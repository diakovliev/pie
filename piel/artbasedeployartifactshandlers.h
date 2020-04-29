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

#ifndef ARTBASEAPISENDLERHANDLERS_H
#define ARTBASEAPISENDLERHANDLERS_H

#include <artbaseapihandlers.h>
#include <boost_property_tree_ext.hpp>
#include <datasequencecutter.h>

namespace art { namespace lib {

class ArtDeployArtifactCheckSumHandlers;

class ArtBaseDeployArtifactsHandlers : public ArtBaseApiHandlers
{
    typedef std::map<std::string, std::string> Attributes;
    friend class ArtDeployArtifactCheckSumHandlers;
public:
    ArtBaseDeployArtifactsHandlers(const ArtBaseDeployArtifactsHandlers& handle);
    ArtBaseDeployArtifactsHandlers(const std::string& api_token);
    ArtBaseDeployArtifactsHandlers(const std::string& api_token, const std::string& uri, const std::string& repo, const std::string& path);
    virtual ~ArtBaseDeployArtifactsHandlers(){}

    virtual size_t handle_input(char *ptr, size_t size);
    virtual size_t handle_output(char *ptr, size_t size);

    virtual void set_url(const std::string& url);
    virtual void set_repo(const std::string& repo);
    virtual void set_path(const std::string& path);

    virtual std::string get_url() {  return url_; }
    virtual std::string get_repo() { return repo_; }
    virtual std::string get_path() { return  path_; }

    virtual std::string gen_uri();

    void push_input_stream(std::shared_ptr<std::istream> is);
    size_t putto(char* ptr, size_t size);

    piel::lib::ChecksumsDigestBuilder::StrDigests& str_digests(bool reset = false);

protected:
    std::string trim(const std::string& src);

private:
    boost::property_tree::ptree tree_;
    piel::lib::DataSequenceCutter uploader_;
    std::stringstream os_;
    Attributes  answer_;
    std::string url_;
    std::string repo_;
    std::string path_;
    bool first_call_;
    piel::lib::ChecksumsDigestBuilder::StrDigests str_digests_;
};

} } // namespace art::lib

#endif // ARTBASEAPISENDLERHANDLERS_H
