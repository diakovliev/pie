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

#include <utils.h>
#include <application.h>
#include <iostream>

#include <curl/curl.h>

namespace pie { namespace app {

bool ICommand::show_help(boost::program_options::options_description &desc, int argc, char **argv)
{
    boost::program_options::options_description help_desc("Help options");

    help_desc.add_options()
        ("help,h", "Show help message.")
        ;

    boost::program_options::variables_map vm;
    boost::program_options::parsed_options parsed =
        boost::program_options::command_line_parser(argc, argv).options(help_desc).allow_unregistered().run();
    boost::program_options::store(parsed, vm);
    boost::program_options::notify(vm);

    if (vm.count("help")) {
        std::cout << help_desc;
        if (!desc.options().empty())
        {
            show_command_help_message(desc);
        }
        return true;
    }

    return false;
}

/*static*/ bool ICommand::get_from_env(boost::program_options::variables_map& vm,
                               const std::string& opt_name,
                               const std::string& env_var,
                               std::string& var)
{
    if (!vm.count(opt_name)) {
        const char *value = ::getenv(env_var.c_str());
        if (value)
        {
            var = std::string(value);
            return true;
        }
        else
        {
            return false;
        }
    }
    return true;
}

/*static*/ bool ICommand::get_from_env(boost::program_options::variables_map& vm,
                               const std::string& opt_name,
                               const std::string& env_var,
                               bool& var)
{
    var = vm.count(opt_name) > 0;
    if (!var) {
        const char *value = ::getenv(env_var.c_str());
        if (value)
        {
            var = piel::lib::string2bool(value);
            return true;
        }
        else
        {
            return false;
        }
    }
    return true;
}

/*virtual*/ void ICommand::show_command_help_message(const boost::program_options::options_description& desc)
{
    std::cout << desc;
}

UnknownCommand::UnknownCommand(Application *app, int argc, char **argv)
    : ICommand(app)
    , argc_(argc)
    , argv_(argv)
{
}

void UnknownCommand::print_curl_version()
{
    ::curl_version_info_data *data = ::curl_version_info(CURLVERSION_NOW);

    std::cout << "CURL:"                                    << std::endl;
    std::cout << "\tversion: "      << data->version        << std::endl;
    std::cout << "\tssl: "          << data->ssl_version    << std::endl;

    unsigned int i = 0;
    const char* protocol = nullptr;

    std::cout << "\tprotocols:";
    while ((protocol = data->protocols[i++])) {
        std::cout << " " << protocol;
    }
    std::cout << std::endl;
}

int UnknownCommand::perform()
{
    std::cerr << "Incorrect command line or unknown command."               << std::endl;

    std::cout << "Version: " << PIE_VERSION                                 << std::endl;
    print_curl_version();

    std::cout << "Usage:"                                                   << std::endl;
    std::cout << "\t" << argv_[0] << " <command name> [command arguments]"  << std::endl;
    std::cout << "Help:"                                                    << std::endl;
    std::cout << "\t" << argv_[0] << " <command name> --help|-h"            << std::endl;
    std::cout << "\t\t- Will out detailed list of the command arguments."   << std::endl;
    app()->show_registered_commands();
    return -1;
}

CommandsFactory::CommandsFactory(Application *app)
    : app_(app)
    , constructors_()
{
}

void CommandsFactory::register_command(ICommandConstructor *constructor)
{
    constructors_.insert(std::make_pair(constructor->name(), constructor));
}

std::shared_ptr<ICommand> CommandsFactory::create(int argc, char **argv)
{
    // second argument is command name
    if (argc < 2) {
        return std::make_shared<UnknownCommand>(app_, argc, argv);
    }

    Constructors::iterator cmd_iter = constructors_.find(std::string(argv[1]));
    if (cmd_iter == constructors_.end()) {
        return std::make_shared<UnknownCommand>(app_, argc, argv);
    }

    return cmd_iter->second->create(app_, argc - 1, argv + 1);
}

void CommandsFactory::show_registered_commands() const
{
    std::cout << "Commands:" << std::endl;
    Constructors::const_iterator cmd_iter = constructors_.begin(),
                                 end      = constructors_.end();
    for (;cmd_iter != end; ++cmd_iter) {
        std::cout << "\t" << cmd_iter->second->name()
                  << ":\t" << cmd_iter->second->description()
                  << std::endl;
    }
}

Application::Application(int argc, char **argv)
    : argc_(argc)
    , argv_(argv)
    , commands_factory_(this)
{
}

Application::~Application()
{
}

int Application::run()
{
    std::shared_ptr<ICommand> command = commands_factory_.create(argc_, argv_);
    return command->perform();
}

void Application::register_command(ICommandConstructor *constructor)
{
    commands_factory_.register_command(constructor);
}

void Application::show_registered_commands() const
{
    commands_factory_.show_registered_commands();
}

} } // namespace pie::app
