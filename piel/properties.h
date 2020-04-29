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

#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <iostream>
#include <string>
#include <map>

namespace piel { namespace lib {

class Properties
{
public:
    typedef std::map<std::string, std::string> MapType;

    // Forward
    struct DefaultFromEnv;

    struct Property
    {
        typedef MapType::value_type::first_type name_type;
        typedef MapType::value_type::second_type value_type;

        explicit Property(const name_type& name, const value_type& value, const std::string& description)
             : value_(name, value)
             , description_(description)
        {
        }

        Property(const Property& src)
            : value_(src.name(), src.value())
            , description_(src.description())
        {
        }

        virtual ~Property()
        {
        }

        name_type name() const
        {
            return value_.first;
        }

        virtual value_type value() const
        {
            return value_.second;
        }

        std::string description() const
        {
            return description_;
        }

        DefaultFromEnv default_from_env(const std::string& env_var)
        {
            return DefaultFromEnv(*this, env_var);
        }

    private:
        MapType::value_type value_;
        std::string description_;
    };

    struct DefaultFromEnv : public Property
    {
        DefaultFromEnv(const Property& property, const std::string& env_var)
            : Property(property)
            , env_var_(env_var)
        {
        }

        virtual ~DefaultFromEnv()
        {
        }

        value_type value() const
        {
            const char *env = ::getenv(env_var_.c_str());
            if (env)
            {
                return env;
            }
            else
            {
                return Property::value();
            }
        }

        std::string env_var() const
        {
            return env_var_;
        }

    private:
        std::string env_var_;
    };

    Properties();
    Properties(const Properties& src);
    ~Properties();

    static Properties load(std::istream &is);
    void store(std::ostream &os) const;

    void set(const Property::name_type& name, const Property::value_type& value);
    Property::value_type get(const Property::name_type& name, const Property::value_type& default_value) const;
    bool contains(const Property::name_type& name) const;

    template<class Property>
    void set(const Property& prop)
    {
        set(prop.name(), prop.value());
    }

    template<class P>
    Property get(const P& prop) const
    {
        return Property(prop.name(), get(prop.name(), prop.value()), prop.description());
    }

    const MapType& data() const;
    MapType& data();
    MapType::mapped_type& operator[](const MapType::key_type& key);
    void clear();

    static Properties from_map(const MapType &map) {
        Properties result;
        result.data_.insert(map.begin(), map.end());
        return result;
    }

    enum JoinDirection {
        JoinDirection_left,
        JoinDirection_right,
    };

    template<JoinDirection direction=JoinDirection_right>
    Properties join(const Properties& right) const
    {
        Properties result = *this;

        for (MapType::const_iterator i = right.data_.begin(), end = right.data_.end(); i != end; ++i)
            if (direction == JoinDirection_right || result.data_.find(i->first) == result.data_.end())
                result.data_[i->first] = i->second;

        return result;
    }

private:
    MapType data_;

};

} } //namespace piel::lib

#endif // PROPERTIES_H
