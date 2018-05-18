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

#include <logging.h>

namespace sgs { namespace lib {

Engine::Engine()
    : attached_(false)
    , ctx_(0)
{
    LOGD << "Create new SGS engine." << ELOG;
    ctx_ = sgs_CreateEngine();
    LOGT << "SGS: created engine: " << ctx_ << ELOG;
}

Engine::Engine(sgs_Context* ctx)
    : attached_(true)
    , ctx_(ctx)
{
    LOGD << "Attach to existing SGS engine." << ELOG;
    LOGT << "SGS: attached engine: " << ctx_ << ELOG;
}

Engine::~Engine()
{
    LOGT << "SGS: engine: " << ctx_ << " attached: " << attached_ << ELOG;
    if (!attached_)
    {
        sgs_DestroyEngine(ctx_);
        LOGD << "Destroyed SGS engine." << ELOG;
    }
    else
    {
        LOGD << "Skip destroing SGS engine for attached context." << ELOG;
    }
}

void Engine::load_lib(Engine::Libs lib)
{
    switch (lib) {
    case Fmt:
    {
        sgs_LoadLib_Fmt(ctx_);
        LOGD << "SGS: Load Fmt lib." << ELOG;
    }
    break;
    case IO:
    {
        sgs_LoadLib_IO(ctx_);
        LOGD << "SGS: Load IO lib." << ELOG;
    }
    break;
    case Math:
    {
        sgs_LoadLib_Math(ctx_);
        LOGD << "SGS: Load Math lib." << ELOG;
    }
    break;
    case OS:
    {
        sgs_LoadLib_OS(ctx_);
        LOGD << "SGS: Load OS lib." << ELOG;
    }
    break;
    case RE:
    {
        sgs_LoadLib_RE(ctx_);
        LOGD << "SGS: Load RE lib." << ELOG;
    }
    break;
    case String:
    {
        sgs_LoadLib_String(ctx_);
        LOGD << "SGS: Load String lib." << ELOG;
    }
    break;
    };
}

void Engine::exec_file(const std::string& filename)
{
    LOGD << "SGS: Exec file: " << filename << ELOG;
    sgs_ExecFile(ctx_, filename.c_str());
}

void Engine::func_name(const std::string& fn)
{
    LOGT << "SGS: Set func name: " << fn << ELOG;
    sgs_FuncName(ctx_, fn.c_str());
}

/*static*/ Engine::Ptr Engine::attach(sgs_Context* ctx)
{
    LOGT << "SGS: Attach to engine: " << ctx << ELOG;
    return Ptr(new Engine(ctx));
}

/*static*/ Engine::Ptr Engine::attach(sgs_Context* ctx, const std::string& fn)
{
    LOGT << "SGS: Attach to engine: " << ctx << " func_name: " << fn <<  ELOG;
    Ptr result = Ptr(new Engine(ctx));
    result->func_name(fn);
    return result;
}

sgs_Context* Engine::c_context()
{
    LOGW << "SGS: !!! Access to raw context: " << ctx_ <<  ELOG;
    return ctx_;
}

SGSBOOL Engine::load_args(int start_index, const std::string& commands, ...)
{
    va_list vl;
    va_start(vl, commands);
    SGSBOOL result = load_args_va(start_index, commands.c_str(), &vl);
    va_end(vl);
    LOGT << "SGS: load_args start_index: " << start_index << " commands: " << commands << " result: " << result << ELOG;
    return result;
}

SGSBOOL Engine::load_args(const std::string& commands, ...)
{
    va_list vl;
    va_start(vl, commands);
    SGSBOOL result = load_args_va(0, commands.c_str(), &vl);
    va_end(vl);
    LOGT << "SGS: load_args commands: " << commands << " result: " << result << ELOG;
    return result;
}

void Engine::set_global_by_name(const std::string& var_name, sgs_Variable var)
{
    LOGT << "SGS: set_global_by_name var_name: " << var_name << ELOG;
    sgs_SetGlobalByName(ctx_, var_name.c_str(), var);
}

void Engine::set_global_by_name(const std::string& var_name, Engine::CFunction func)
{
    LOGT << "SGS: set_global_by_name var_name (func): " << var_name << ELOG;
    set_global_by_name(var_name, sgs_MakeCFunc(func));
}

SGSBOOL Engine::load_args_va(int start_index, const std::string& commands, va_list *vlp)
{
    return sgs_LoadArgsExtVA(ctx_, start_index, commands.c_str(), vlp);
}

} } // namespace sgs::lib
