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

//
// >> depman add test   git+ssh://git.test.org:repository.git;refspec=;mergebase=;logbase=;branch=master;revison=123456
// depman.conf:
//
// git: {
//   id: test;
//   remote: ssh://$ldap_user$@git.test.org:repository.git;
//   local: test;
//   branch: master;
//   revision: 123456;
// };
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

int main(int argc, char **argv)
{
    return 0;
}
