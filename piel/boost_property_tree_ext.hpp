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

#ifndef BOOST_PROPERTY_TREE_EXT_HPP
#define BOOST_PROPERTY_TREE_EXT_HPP

#include <list>
#include <optional>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace boost { namespace property_tree {

    struct FindPropertyHelper
    {
        std::string _prop_name;

        FindPropertyHelper(const std::string& prop_name)
            : _prop_name(prop_name)
        {
        }

        bool operator()(ptree::value_type prop)
        {
            return prop.first == _prop_name;
        }
    };

    // Several common algorithms.

    // Emumerate sub objects.
    // void callback(const ptree::value_type& obj)
    template<class Callback, class EmptyCallback>
    void each(const ptree& obj, Callback callback, EmptyCallback empty_callback = [](){}) {
        if (obj.begin() == obj.end()) {
            empty_callback();
            return;
        }

        for(ptree::const_iterator i = obj.begin(), end = obj.end(); i != end; ++i) {
            callback(*i);
        }
    }

    // Find sub object.
    // bool predicate(const ptree::value_type& obj)
    template<class UnaryPredicate>
    std::optional<ptree::value_type> find(const ptree& obj, UnaryPredicate predicate) {
        for(ptree::const_iterator i = obj.begin(), end = obj.end(); i != end; ++i) {
            if (predicate(*i)) {
                return (*i);
            }
        }
        return std::nullopt;
    }

    // Find sub object values.
    // bool predicate(const ptree::value_type& obj)
    template<class UnaryPredicate>
    std::optional<std::string> find_value(const ptree& obj, UnaryPredicate predicate) {
        for(ptree::const_iterator i = obj.begin(), end = obj.end(); i != end; ++i) {
            if (predicate(*i)) {
                return i->second.data();
            }
        }
        return std::nullopt;
    }

    // Find sub objects.
    // bool predicate(const ptree::value_type& obj)
    template<class UnaryPredicate>
    std::list<ptree::value_type> find_all(const ptree& obj, UnaryPredicate predicate) {
        std::list<ptree::value_type> result;
        for(ptree::const_iterator i = obj.begin(), end = obj.end(); i != end; ++i) {
            if (predicate(*i)) {
                result.push_back(*i);
            }
        }
        return result;
    }

    // Find sub objects values.
    // bool predicate(const ptree::value_type& obj)
    template<class UnaryPredicate>
    std::list<std::string> find_all_values(const ptree& obj, UnaryPredicate predicate) {
        std::list<std::string> result;
        for(ptree::const_iterator i = obj.begin(), end = obj.end(); i != end; ++i) {
            if (predicate(*i)) {
                result.push_back(i->second.data());
            }
        }
        return result;
    }

} } // namespace boost::property_tree

#endif //BOOST_PROPERTY_TREE_EXT_HPP
