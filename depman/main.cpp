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
// >> depman add test   git+ssh://git.test.org:repository.git;refspec=;mergebase=;logbase=;branch=master;revison=123456
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
// git({
//   id = "test",
//   remote = "ssh://" + ldap_user + "@git.test.org:repository.git",
//   local = "test",
//   branch = "master",
//   revision = "123456",
// });
//
// >> depman add test2  art+https:://artifactory.server/artifactory;repo=bin-release;qavc=test:test2:+;version=123
// depman.conf:
//
// git: {
//   id: test;
//   remote: ssh://$ldap_user$@git.test.org:repository.git;
//   local: test;
//   branch: master;
//   revision: 123456;
// };
// art: {
//   id: test2;
//   remote: https:://artifactory.server/artifactory;
//   local: test2;
//   repo: bin-release-local;
//   gavc: test:test2:+;
//   version: 123;
//   classifiers: {
//      debug: dbg;
//      release: prd;
//   };
// };
//
//
// >> depman show
// >> depman export
//
//


int sample_func(sgs_Context* context)
{
    sgs::lib::Engine::Ptr engine = sgs::lib::Engine::attach(context, "sample_func");

    char* str;
    float q = 1;

    SGSBOOL have_args = engine->load_args("s|f", &str, &q);
    if( !have_args )
    {
        std::cout << "[sample_func] no args" << std::endl;
        return 0; // < number of return values or a negative number on failure
    }

    std::cout << "[sample_func] arg 1: " << str   << std::endl;
    std::cout << "[sample_func] arg 2: " << q     << std::endl;

    engine->push_bool(1);
    return 1; // < number of return values or a negative number on failure
}

int sample_func2(sgs_Context* context)
{
    sgs::lib::Engine::Ptr engine = sgs::lib::Engine::attach(context, "sample_func2");

    //SGSBOOL have_args = engine->load_args("t", &dict_size);
    //if( !have_args )
    //{
    //    return 0; // < number of return values or a negative number on failure
    //}

    //std::cout << "retrieved size: " << sgs_DebugDumpVarExt( engine->c_context(), dict_size, -1) << std::endl;

    sgs_Variable dict_var;
    sgs_StoreVariable( engine->c_context(), &dict_var );

    std::cout << "retrieved dict: " << sgs_DebugDumpVarExt( engine->c_context(), dict_var, -1) << std::endl;

    sgs_Variable iterator, key, value;
    // .. assuming iterable is initalized here ..
    sgs_CreateIterator( engine->c_context(), &iterator, dict_var );
    while( sgs_IterAdvance( engine->c_context(), iterator ) > 0 )
    {
        sgs_IterGetData( engine->c_context(), iterator, NULL, &value );
        // .. use value ..
        sgs_Release( engine->c_context(), &value );

        sgs_IterGetData( engine->c_context(), iterator, &key, &value );
        // .. use key and value ..
        // 
        std::cout << "[sample_func2] key: " << sgs_ToStringP( engine->c_context(), &key ) << std::endl;
        std::cout << "[sample_func2] value: " << sgs_ToStringP( engine->c_context(), &value ) << std::endl;

        sgs_Release( engine->c_context(), &key );
        sgs_Release( engine->c_context(), &value );
    }
    sgs_Release( engine->c_context(), &iterator );

    sgs_Release( engine->c_context(), &dict_var );

    engine->push_bool(1);
    return 1; // < number of return values or a negative number on failure
}

int main(int argc, char **argv)
{
    // create the context
    sgs::lib::Engine::Ptr engine(new sgs::lib::Engine());

    // load the built-in OS, Math libraries
    engine->load_lib(sgs::lib::Engine::Libs::OS);
    engine->load_lib(sgs::lib::Engine::Libs::Math);

    engine->set_global_by_name("sample_func", sample_func);
    engine->set_global_by_name("sample_func2", sample_func2);

     // load a file
    engine->exec_file("script.sgs");

    // call a global function with 0 arguments, expecting 0 values returned
    //sgs_GlobalCall( C, "myFunction", 0, 0 );

    // destroy the context

    return 0;
}
