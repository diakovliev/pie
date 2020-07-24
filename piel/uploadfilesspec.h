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

#ifndef UPLOADFILESSPEC_H
#define UPLOADFILESSPEC_H

#include <map>
#include <vector>
#include <string>
#include <optional>

namespace art { namespace lib {

namespace ufs {
struct files_spec_data {
    std::string classifier;     // empty by default
    std::string extension;      // empty by default
    std::string file_name;
};

typedef std::vector<ufs::files_spec_data> UFSVector;

std::string to_string(const files_spec_data&);
std::string to_string(const UFSVector&);
std::string to_classifier(const files_spec_data&);

} //namespace ufs

class UploadFileSpec {
public:
    UploadFileSpec();
    ~UploadFileSpec();

    static std::optional<UploadFileSpec> parse(const std::string& files_spec_str);
    std::string to_string() const;

    ufs::UFSVector get_data() { return  data_; }

private:
    ufs::UFSVector data_;

};

} } // namespace art::lib

#endif // UPLOADFILESSPEC_H
