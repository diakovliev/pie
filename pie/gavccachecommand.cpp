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

#include <iostream>
#include <cstdlib>
#include <gavccommand.h>
#include <gavccachecommand.h>
#include <gavccache.h>
#include <gavccacheinit.h>
#include <gavccacheclean.h>
#include <logging.h>
#include <mavenmetadata.h>

#include <boost_property_tree_ext.hpp>
#include <filesystem>

namespace pie { namespace app {

namespace pt = boost::property_tree;
namespace po = boost::program_options;

const int default_age = 180;

GavcCacheCommand::GavcCacheCommand(Application *app, int argc, char **argv)
    : ICommand(app)
    , argc_(argc)
    , argv_(argv)
    , do_init_(false)
    , do_clean_(false)
    , cache_path_(piel::cmd::utils::get_default_cache_path())
    , max_age_(default_age)
{
}

GavcCacheCommand::~GavcCacheCommand()
{
}

void GavcCacheCommand::show_command_help_message(const po::options_description& desc)
{
    std::cerr << "Usage: cache [options]" << std::endl;
    std::cout << desc;
}

bool GavcCacheCommand::parse_arguments()
{
    po::options_description desc("Query options");
    desc.add_options()
        ("cache-path",      po::value<std::string>(&cache_path_),   (std::string("Cache location. Can be set using GAVC_CACHE environment variable. Default: ") + piel::cmd::utils::get_default_cache_path()).c_str())
        ("init",                                                    (std::string("Perform cache initialization.")).c_str())
        ("clean",                                                   (std::string("Perform cache clean.")).c_str())
        ("max-age-days",    po::value<int>(&max_age_),              (std::string("(clean) Max age of elements in days. Default: ") + std::to_string(default_age)).c_str())
        ;

    if (show_help(desc, argc_, argv_)) {
        return false;
    }

    po::variables_map vm;
    po::parsed_options parsed = po::command_line_parser(argc_, argv_).options(desc).allow_unregistered().run();
    po::store(parsed, vm);
    try {
        po::notify(vm);
    } catch (...) {
        show_command_help_message(desc);
        return false;
    }

    do_init_    = (vm.count("init") > 0);
    do_clean_   = (vm.count("clean") > 0);

    get_from_env(vm, "cache",       "GAVC_CACHE",                   cache_path_);

    return true;
}

/*virtual*/ int GavcCacheCommand::perform()
{
    int result = -1;

    if (!parse_arguments()) {
        return result;
    }

    std::cout << "Cache path: " << cache_path_ << std::endl;

    if (do_init_)
    {
        std::cout << "Initialize cache ...";

        piel::cmd::GAVCCacheInit gavccachecheinit(cache_path_);

        gavccachecheinit();

        std::cout << " DONE" << std::endl;
    }

    if (do_clean_)
    {
        try 
        {
            std::cout << "Clean cache ...";

            piel::cmd::GAVCCacheClean gavccacheclean(
                             cache_path_,
                             max_age_);

            gavccacheclean();

            std::cout << " DONE" << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << piel::cmd::errors::format_exceptions_stack(e) << std::endl;
            return result;
        }
    }

    result = 0;

    return result;
}

} } // namespace pie::app
