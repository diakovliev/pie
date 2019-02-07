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

#ifndef PIEL_ASSETID_H_
#define PIEL_ASSETID_H_

#include <iostream>
#include <string>

namespace piel { namespace lib {

/// AssetId is abstract payload identifier.
/// Physically is string representation of payload content digest.
class AssetId
{
public:
    static const AssetId not_calculated;
    static const AssetId empty;
    static const std::string digest_algo;
    static const unsigned int str_digest_len;

    AssetId();
    AssetId(std::istream& is);
    AssetId(const std::string& id);
    AssetId(const AssetId& src);

    ~AssetId();

    bool operator==(const AssetId& src) const;
    bool operator!=(const AssetId& src) const;

    bool operator<(const AssetId& src) const;

    std::string string() const;

    /// Calculate digest and create AssetId object for the input stream.
    static AssetId create_for(std::istream& is);

    /// Create AssetId from string presentation.
    static AssetId create(const std::string& id);

private:
    std::string id_;

};

} } // namespace piel::lib

#endif /* PIEL_ASSETID_H_ */
