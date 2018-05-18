/*
 * Copyright (c) 2018, diakovliev
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

#ifndef DEPMAN_SGSVAR_H_
#define DEPMAN_SGSVAR_H_

#include "sgsengine.h"

#include <boost/optional.hpp>

namespace sgs { namespace lib {

class Var: boost::noncopyable {
public:
    typedef boost::shared_ptr<Var> Ptr;
    typedef std::pair<Ptr,Ptr> PtrPair;

    Var(const Engine::Ptr& engine);
    ~Var();

    static Ptr pop(const Engine::Ptr& engine);
    static int push(const Engine::Ptr& engine, Ptr var);

    sgs_Variable& c_var();

    std::string debug_dump(int maxdepth = -1);

    bool is_null();
    bool is_dict();
    bool is_map();
    bool is_array();

    boost::optional<Ptr> get(const std::string& property_name);
    bool set(const std::string& property_name, const Ptr& value);

    // Iterator
    Ptr iterator();
    bool has_next();
    Ptr value();
    PtrPair key_value();

    template<typename T> T as();
    //    template<>
    //    std::string Var::as<std::string>();
    //
    //    template<>
    //    bool Var::as<bool>();
    //
    //    template<>
    //    int Var::as<int>();
    //
    //    template<>
    //    float Var::as<float>();

private:
    Engine::Ptr engine_;
    bool accured_;
    sgs_Variable var_;
};

} } // namespace sgs::lib

#endif /* DEPMAN_SGSVAR_H_ */
