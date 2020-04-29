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

#include <application.h>
#include <gavccommand.h>
#include <gavccachecleancommand.h>
#include <gavccacheinitcommand.h>
#include <uploadcommand.h>

namespace pa = pie::app;

int main(int argc, char **argv)
{
    pa::Application app(argc, argv);

    pa::CommandConstructor<pa::GavcCommand>             cgavc(   "gavc",             "GAVC query implementation." );
    pa::CommandConstructor<pa::GavcCacheCleanCommand>   cclean(  "gavccacheclean",   "Clean GAVC cache." );
    pa::CommandConstructor<pa::GavcCacheInitCommand>    cinit(   "gavccacheinit",    "Init GAVC cache." );
    pa::CommandConstructor<pa::UploadCommand>           cupload( "upload",           "Upload to Artifactory server." );

    app.register_command(&cgavc);
    app.register_command(&cclean);
    app.register_command(&cinit);
    app.register_command(&cupload);

    return app.run();
}
