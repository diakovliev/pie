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

#include "sgsvar.h"

#include <logging.h>

namespace sgs { namespace lib {

Var::Var(const Engine::Ptr& engine)
    : engine_(engine)
    , accured_(false)
    , var_(sgs_MakeNull())
{
}

Var::~Var()
{
    if (accured_) {
        sgs_Release(engine_->ctx_, &var_);
    }
}

/*static*/ Var::Ptr Var::pop(const Engine::Ptr& engine)
{
    Ptr result(new Var(engine));
    sgs_StoreVariable(engine->ctx_, &result->var_);
    result->accured_ = true;
    return result;
}

/*static*/ int Var::push(const Engine::Ptr& engine, Ptr var)
{
    return sgs_PushVariable(engine->ctx_, var->var_);
}

sgs_Variable& Var::c_var()
{
    return var_;
}

std::string Var::debug_dump(int maxdepth)
{
    return sgs_DebugDumpVarExt(engine_->ctx_, var_, maxdepth);
}

bool Var::is_null()
{
    return var_.type == SGS_VT_NULL;
}

bool Var::is_dict()
{
    return sgs_IsDict(var_);
}

bool Var::is_map()
{
    return sgs_IsMap(var_);
}

bool Var::is_array()
{
    return sgs_IsArray(var_);
}

// Iterator
Var::Ptr Var::iterator()
{
    Ptr iter(new Var(engine_));
    sgs_CreateIterator(engine_->ctx_, &iter->var_, var_);
    return iter;
}

bool Var::has_next()
{
    return sgs_IterAdvance(engine_->ctx_, var_) > 0;
}

Var::Ptr Var::value()
{
    Ptr value(new Var(engine_));
    sgs_IterGetData(engine_->ctx_, var_, 0, &value->var_);
    value->accured_ = true;
    return value;
}

Var::PtrPair Var::key_value()
{
    Ptr key(new Var(engine_));
    Ptr value(new Var(engine_));
    sgs_IterGetData(engine_->ctx_, var_, &key->var_, &value->var_);
    key->accured_ = true;
    value->accured_ = true;
    return std::make_pair(key,value);
}

boost::optional<Var::Ptr> Var::get(const std::string& property_name)
{
    boost::optional<Var::Ptr> result = boost::none;
    if (sgs_PushProperty(engine_->ctx_, var_, property_name.c_str()))
    {
        result = pop(engine_);
    }
    return result;
}

bool Var::set(const std::string& property_name, const Var::Ptr& value)
{
    return sgs_SetProperty(engine_->ctx_, var_, property_name.c_str(), value->var_);
}

template<>
std::string Var::as<std::string>()
{
    return sgs_ToStringP(engine_->ctx_, &var_);
}

template<>
bool Var::as<bool>()
{
    return sgs_GetBoolP(engine_->ctx_, &var_);
}

template<>
int Var::as<int>()
{
    return sgs_GetIntP(engine_->ctx_, &var_);
}

template<>
float Var::as<float>()
{
    return sgs_GetRealP(engine_->ctx_, &var_);
}

} } // namespace sgs::lib
