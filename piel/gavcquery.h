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

#ifndef GAVCQUERY_H
#define GAVCQUERY_H

#include <vector>
#include <map>
#include <string>
#include <memory>
#include <optional>

namespace art { namespace lib {

namespace gavc {

    enum Ops {
        Op_const,
        Op_all,
        Op_latest,
        Op_oldest,
    };

    struct OpType: public std::pair<Ops,std::string>
    {
        OpType()                                : std::pair<Ops,std::string>(Op_const, "")      {}
        OpType(const std::string& val)          : std::pair<Ops,std::string>(Op_const, val)     {}
        OpType(Ops op, const std::string& val)  : std::pair<Ops,std::string>(op, val)           {}
        OpType(Ops op, char val)                : std::pair<Ops,std::string>()
        {
            first = op;
            second.push_back(val);
        }
    };

    struct gavc_data {
        std::string group;
        std::string name;
        std::string version;        // empty == all by default
        std::string classifier;     // empty by default
        std::string extension;      // empty by default
    };

    enum RangeFlags {
        RangeFlags_exclude_all      = 0x0,
        RangeFlags_include_left     = 0x01,
        RangeFlags_include_right    = 0x02,
    };

    struct gavc_versions_range_data {
        std::string left;
        std::string right;
        unsigned char flags;
    };

} // namespace gavc

class GavcQuery
{
public:
    GavcQuery();
    ~GavcQuery();

    static std::optional<GavcQuery> parse(const std::string& gavc_str);
    std::string to_string() const;

    std::string group() const           { return data_.group; }
    std::string name() const            { return data_.name; }
    std::string version() const         { return data_.version; }
    std::string classifier() const      { return data_.classifier; }
    std::string extension() const       { return data_.extension; }

    void set_group(const std::string& group)            { data_.group = group; }
    void set_name(const std::string& name)              { data_.name = name; }
    void set_version(const std::string& version)        { data_.version = version; }
    void set_classifier(const std::string& classifier)  { data_.classifier = classifier; }
    void set_extension(const std::string& extension)    { data_.extension = extension; }

    std::string group_path() const;

    std::optional<std::vector<gavc::OpType> > query_version_ops() const;
    static std::optional<std::vector<gavc::OpType> > query_version_ops(const std::string& version);

    std::optional<gavc::gavc_versions_range_data> query_versions_range() const;
    static std::optional<gavc::gavc_versions_range_data> query_versions_range(const std::string& version);

    std::pair<
        std::optional<std::vector<gavc::OpType> >,
        std::optional<gavc::gavc_versions_range_data>
    > query_version_data() const;

    static std::pair<
        std::optional<std::vector<gavc::OpType> >,
        std::optional<gavc::gavc_versions_range_data>
    > query_version_data(const std::string& version);

    std::string format_maven_metadata_url(const std::string& server_url, const std::string& repository) const;
    std::string format_maven_metadata_path(const std::string& repository) const;

    GavcQuery& operator= (const GavcQuery& g) {
        data_ = g.data_;
        return *this;
    }

    bool is_exact_version_query() const;
    static bool is_exact_version_query(const std::string& version);

    bool is_single_version_query() const;
    static bool is_single_version_query(const std::string& version);

    std::vector<std::string> filter(const std::vector<std::string>& versions) const;

    std::string to_aql_path() const;
    std::string to_aql_name() const;
    std::string to_aql(const std::string& repo) const;

private:
    gavc::gavc_data data_;
};

} } // namespace art::lib

#endif // GAVCQUERY_H
