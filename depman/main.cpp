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

#include <string>
#include <map>
#include <list>
#include <iostream>

#include "sgsengine.h"
#include "sgsvar.h"

namespace depman { namespace app {

    struct Transport {
        std::string remote_;
        std::string local_;
    };

    namespace transports {
        struct Git: public Transport {
            std::string branch_;
            std::string revision_;
        };
        struct Art: public Transport {
            std::string repo_;
            std::string gavc_;
            std::string version_;
            std::map<std::string,std::string> classifiers_;
        };
    }

} }

namespace da = depman::app;

// Basic action type
struct Action {

    typedef boost::shared_ptr<Action> Ptr;

    enum State {
        NotInitialized,
        Ready,
        Done,
    };

    enum Result {
        NotPerformed,
        Error,
        Ok,
    };

    State state_;
    Result result_;
    std::list<Ptr> dependencies_;
};

// Execute shell command
struct ShellAction: public Action {};

//
// depman.conf:
//
// var externals = {
//   test = {
//      git = "test",
//      remote = "ssh://" + ldap_user + "@git.test.org:repository.git",
//      local = "test",
//      branch = "master",
//      revision = "123456",
//   },
// };
//
// sample_func2(externals)
//

//int sample_func(sgs_Context* context)
//{
//    sgs::lib::Engine::Ptr engine = sgs::lib::Engine::attach(context, "sample_func");
//
//    char* str;
//    float q = 1;
//
//    SGSBOOL have_args = engine->load_args("s|f", &str, &q);
//    if( !have_args )
//    {
//        std::cout << "[sample_func] no args" << std::endl;
//        return 0; // < number of return values or a negative number on failure
//    }
//
//    std::cout << "[sample_func] arg 1: " << str   << std::endl;
//    std::cout << "[sample_func] arg 2: " << q     << std::endl;
//
//    engine->push_bool(1);
//    return 1; // < number of return values or a negative number on failure
//}

int sample_func2(sgs_Context* context)
{
    sgs::lib::Engine::Ptr engine = sgs::lib::Engine::attach(context, "sample_func2");

    sgs::lib::Var::Ptr dict_var = sgs::lib::Var::pop(engine);
    std::cout << "retrieved dict: " << dict_var->debug_dump() << std::endl;

    sgs::lib::Var::Ptr iterator = dict_var->iterator();
    while(iterator->has_next())
    {
        sgs::lib::Var::PtrPair kv = iterator->key_value();

        std::cout << "[sample_func2] id: "     << kv.first->as<std::string>()  << std::endl;
        //std::cout << "[sample_func2] value: "   << kv.second->debug_dump()      << std::endl;

        sgs::lib::Var::Ptr second_iterator = kv.second->iterator();
        while (second_iterator->has_next())
        {
            sgs::lib::Var::PtrPair second_kv = second_iterator->key_value();

            std::cout << "[sample_func2]\tkey: "    << second_kv.first->as<std::string>()    << std::endl;
            std::cout << "[sample_func2]\tvalue: "  << second_kv.second->as<std::string>()   << std::endl;
        }
    }

    boost::optional<sgs::lib::Var::Ptr> otest = dict_var->get("test");
    if (otest) {
        std::cout << "Access by id: " << (*otest)->as<std::string>() << std::endl;
    }
    boost::optional<sgs::lib::Var::Ptr> otest1 = dict_var->get("test1");
    if (otest1) {
        std::cout << "Access by id: " << (*otest1)->as<std::string>() << std::endl;
    }

    //engine->push_bool(1);
    return 0; // < number of return values or a negative number on failure
}

int main(int argc, char **argv)
{
    // create the context
    sgs::lib::Engine::Ptr engine(new sgs::lib::Engine());

    // load the built-in OS, Math libraries
    engine->load_lib(sgs::lib::Engine::Libs::OS);
    engine->load_lib(sgs::lib::Engine::Libs::Math);

    //engine->set_global_by_name("sample_func", sample_func);
    engine->set_global_by_name("sample_func2", sample_func2);

     // load a file
    engine->exec_file("script.sgs");

    return 0;
}
