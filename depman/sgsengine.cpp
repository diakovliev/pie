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

#include "sgsengine.h"

namespace sgs { namespace lib {

Engine::Engine()
    : attached_(false)
    , ctx_(0)
{
    ctx_ = sgs_CreateEngine();
}

Engine::Engine(sgs_Context* ctx)
    : attached_(true)
    , ctx_(ctx)
{
}

Engine::~Engine()
{
    if (!attached_) {
        sgs_DestroyEngine(ctx_);
    }
}

void Engine::load_lib(Engine::Libs lib)
{
    switch (lib) {
    case Fmt:
        sgs_LoadLib_Fmt(ctx_);
    break;
    case IO:
        sgs_LoadLib_IO(ctx_);
    break;
    case Math:
        sgs_LoadLib_Math(ctx_);
    break;
    case OS:
        sgs_LoadLib_OS(ctx_);
    break;
    case RE:
        sgs_LoadLib_RE(ctx_);
    break;
    case String:
        sgs_LoadLib_String(ctx_);
    break;
    };
}

void Engine::exec_file(const std::string& filename)
{
    sgs_ExecFile(ctx_, filename.c_str());
}

void Engine::func_name(const std::string& fn)
{
    sgs_FuncName(ctx_, fn.c_str());
}

/*static*/ Engine::Ptr Engine::attach(sgs_Context* ctx)
{
    return Ptr(new Engine(ctx));
}

/*static*/ Engine::Ptr Engine::attach(sgs_Context* ctx, const std::string& fn)
{
    Ptr result = Ptr(new Engine(ctx));
    result->func_name(fn);
    return result;
}

// Stack
void Engine::push_bool(sgs_Bool value)
{
    sgs_PushBool(ctx_, value);
}

sgs_Context* Engine::c_context()
{
    return ctx_;
}

SGSBOOL Engine::load_args(int start_index, const std::string& commands, ...)
{
    va_list vl;
    va_start(vl, commands);
    SGSBOOL result = load_args_va(start_index, commands.c_str(), &vl);
    va_end(vl);
    return result;
}

SGSBOOL Engine::load_args(const std::string& commands, ...)
{
    va_list vl;
    va_start(vl, commands);
    SGSBOOL result = load_args_va(0, commands.c_str(), &vl);
    va_end(vl);
    return result;
}

void Engine::set_global_by_name(const std::string& var_name, sgs_Variable var)
{
    sgs_SetGlobalByName(ctx_, var_name.c_str(), var);
}

void Engine::set_global_by_name(const std::string& var_name, Engine::function func)
{
    set_global_by_name(var_name, sgs_MakeCFunc(func));
}

SGSBOOL Engine::load_args_va(int start_index, const std::string& commands, va_list *vlp)
{
    return sgs_LoadArgsExtVA(ctx_, start_index, commands.c_str(), vlp);
}

} } // namespace sgs::lib
