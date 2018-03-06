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

#ifndef ARTBASECONSTANTS_H
#define ARTBASECONSTANTS_H

#include <string>

namespace art { namespace lib {

struct ArtBaseConstants {
    static const std::string uri_attribute;
    static const std::string repo_attribute;
    static const std::string path_attribute;
    static const std::string created_date_attribute;
    static const std::string uri_delimiter;

    static const std::string download_uri;
    static const std::string size;
    static const std::string checksums;
    static const std::string checksums_md5;
    static const std::string checksums_sha1;

    static const std::string mime_type;
    static const std::string mime_type_text;

    static const std::string pom_modelVersion;
    static const std::string pom_modelVersion_ver;
    static const std::string pom_groupId;
    static const std::string pom_artifactId;
    static const std::string pom_version;
    static const std::string pom_packaging;
    static const std::string pom_packaging_pack;
    static const std::string pom_project;

    static const std::string pom_classifier;
};

} } // namespace art::lib

#endif // ARTBASECONSTANTS_H
