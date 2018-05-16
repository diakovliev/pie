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

#ifndef DEPMAN_SGSENGINE_H_
#define DEPMAN_SGSENGINE_H_

#include <cstdarg>
#include <sgscript.h>

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

namespace sgs { namespace lib {

class Engine: boost::noncopyable {
public:
    typedef boost::shared_ptr<Engine> Ptr;
    typedef int (*function)(sgs_Context*);

    enum Libs {
        Fmt,
        IO,
        Math,
        OS,
        RE,
        String,
    };

    Engine();
    ~Engine();

    void load_lib(Libs lib);

    void exec_file(const std::string& filename);

    void func_name(const std::string& fn);

    static Ptr attach(sgs_Context* ctx);

    static Ptr attach(sgs_Context* ctx, const std::string& fn);

    // Stack
    void push_bool(sgs_Bool value);

    sgs_Context* c_context();

    SGSBOOL load_args(int start_index, const std::string& commands, ...);

    SGSBOOL load_args(const std::string& commands, ...);

    void set_global_by_name(const std::string& var_name, sgs_Variable var);

    void set_global_by_name(const std::string& var_name, function func);

private:
    Engine(sgs_Context* ctx);

    SGSBOOL load_args_va(int start_index, const std::string& commands, va_list *vlp);

private:
    const bool attached_;
    sgs_Context* ctx_;
};

} } // namespace sgs::lib

#endif /* DEPMAN_SGSENGINE_H_ */
