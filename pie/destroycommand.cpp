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

#include <destroycommand.h>
#include <destroy.h>

#include <boost_filesystem_ext.hpp>

namespace pie { namespace app {

namespace po = boost::program_options;

DestroyCommand::DestroyCommand(Application *app, int argc, char **argv)
    : ICommand(app)
    , argc_(argc)
    , argv_(argv)
    , working_copy_()
    , ref_()
{
}

DestroyCommand::~DestroyCommand()
{
}

void DestroyCommand::show_command_help_message(const po::options_description& desc)
{
    std::cerr << "Usage: destroy [--tree] <ref>" << std::endl;
    std::cout << desc;
}

int DestroyCommand::perform()
{
    po::options_description desc("Destroy tree options");
    desc.add_options()
        ("tree",         po::value<std::string>(&ref_)->required(),     "Tree name for destroy.");
        ;

    po::positional_options_description pos_desc;
    pos_desc.add("tree", -1);

    if (show_help(desc, argc_, argv_))
    {
        return -1;
    }

    po::variables_map vm;
    po::parsed_options parsed =
        po::command_line_parser(argc_, argv_).options(desc).positional(pos_desc).allow_unregistered().run();
    po::store(parsed, vm);
    po::notify(vm);

    try
    {
        working_copy_ = piel::lib::WorkingCopy::attach(boost::filesystem::current_path());

        piel::cmd::Destroy destroy(working_copy_, ref_);
        destroy();

        std::cout << ref_ <<  std::endl;
    }
    catch (const piel::lib::errors::attach_to_non_working_copy& e)
    {
        std::cerr << "No working copy detected at " <<  boost::filesystem::current_path() << "!" << std::endl;
        return -1;
    }
    catch (const piel::lib::errors::unable_to_find_reference_file& e)
    {
        std::cerr << "Unable to find reference file at working copy!" << std::endl;
        return -1;
    }
    catch (const piel::cmd::errors::attempt_to_destroy_current_tree& e)
    {
        std::cerr << "Can't destroy current working tree!" << std::endl;
        return -1;
    }

    if (!working_copy_->is_valid())
    {
        std::cerr << "Unknown error. Working copy state is invalid." << std::endl;
        return -1;
    }

    return 0;
}


} } // namespace pie::app
