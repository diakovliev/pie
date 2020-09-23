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

#include <logging.h>
#include <properties.h>
#include <parsers/properties.h>

namespace piel { namespace lib {

    struct JavaPropertiesConstants {
        static const char assign;
        static const char endl;
    };

    const char JavaPropertiesConstants::assign = '=';
    const char JavaPropertiesConstants::endl = '\n';

    Properties::Properties()
        : data_()
    {
    }

    Properties::Properties(const Properties& src)
        : data_(src.data_)
    {

    }

    Properties::~Properties()
    {
    }

    Properties Properties::load(std::istream &is)
    {
        Properties result;

        std::string buffer;

        while (is) {
            std::getline(is, buffer);
            LOGT << "buffer: " << buffer << ELOG;

            auto pair = parsers::properties::parse_property(buffer);
            if (pair)
            {
                result.data_[pair->first] = pair->second;
                LOGT << "'" << pair->first << "' = '" << pair->second << "'" << ELOG;
            }
        }

        return result;
    }

    void Properties::store(std::ostream &os) const
    {
        for(MapType::const_iterator i = data_.begin(), end = data_.end(); i != end; ++i)
        {
            if (!i->first.empty() && !i->second.empty())
            {
                os << i->first << JavaPropertiesConstants::assign << i->second << JavaPropertiesConstants::endl;
            }
        }
    }

    void Properties::set(const Property::name_type& name, const Property::value_type& value)
    {
        data_[name] = value;
    }

    Properties::Property::value_type Properties::get(const Property::name_type& name, const Property::value_type& default_value) const
    {
        if (data_.find(name) == data_.end())
        {
            return default_value;
        }
        else
        {
            return data_.at(name);
        }
    }

    bool Properties::contains(const Property::name_type& name) const
    {
        return data_.find(name) != data_.end();
    }

    const Properties::MapType& Properties::data() const
    {
        return data_;
    }

    Properties::MapType& Properties::data()
    {
        return data_;
    }

    Properties::MapType::mapped_type& Properties::operator[](const Properties::MapType::key_type& key)
    {
        return data_[key];
    }

    void Properties::clear()
    {
        data_.clear();
    }

} } //namespace piel::lib
