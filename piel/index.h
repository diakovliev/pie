/*
 * Copyright (c) 2017, diakovliev
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

#ifndef PIEL_INDEX_H_
#define PIEL_INDEX_H_

#include <asset.h>

#include <iostream>
#include <map>
#include <set>

namespace piel { namespace lib {

class Index
{
public:
    typedef std::map<std::string, Asset> Content;
    typedef std::map<std::string, std::string> Attributes;

    Index();
    ~Index();

    void add(const std::string& index_path, const Asset& asset);
    const Content& content() const;

    const Asset& self();
    const Asset& parent() const;

    void set_(const std::string& attribute, const std::string& value);
    std::string get_(const std::string& attribute, const std::string& default_value = std::string()) const;

#define Index_DECLARE_ATTRIBUTE(x) \
    inline void set_ ## x ## _(const std::string& value)     { set_( #x , value); } \
    inline std::string get_ ## x ## _() const                { return get_( #x, std::string() ); } \

    Index_DECLARE_ATTRIBUTE(message)
    Index_DECLARE_ATTRIBUTE(author)
    Index_DECLARE_ATTRIBUTE(email)
    Index_DECLARE_ATTRIBUTE(commiter)
    Index_DECLARE_ATTRIBUTE(commiter_email)

#undef Index_DECLARE_ATTRIBUTE

    // Serialization methods.
    void store(std::ostream& os) const;
    void load(std::istream& is);

    // Get all assets including Index asset. Method will be used by storage.
    std::set<Asset> assets() const;

private:
    Asset self_;
    Asset parent_;
    Content content_;
    Attributes attributes_;

};

} } // namespace piel::lib

#endif /* PIEL_INDEX_H_ */
