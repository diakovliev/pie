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

#ifndef GAVCCOMMAND_H
#define GAVCCOMMAND_H

#include <application.h>
#include <gavc.h>
#include <gavcquery.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/program_options.hpp>
#include <filesystem>

namespace pie { namespace app {

class GavcCommand: public ICommand
{
public:
    GavcCommand(Application *app, int argc, char **argv);
    virtual ~GavcCommand();

    virtual int perform();

    bool have_to_download_results() const { return have_to_download_results_; }

protected:
    bool parse_arguments();
    void show_command_help_message(const boost::program_options::options_description& desc);

    void write_files_list(const piel::cmd::GAVC::paths_list& paths_list) const;

private:
    int argc_;
    char **argv_;

    std::string server_url_;
    std::string server_api_access_token_;
    std::string server_repository_;

    art::lib::GavcQuery query_;

    bool have_to_download_results_;
    bool have_to_delete_results_;

    std::string output_file_;
    std::string cache_path_;
    bool disable_cache_;

    std::string notifications_file_;
    unsigned int max_attempts_;
    unsigned int retry_timeout_s_;

    std::string files_list_;
    bool force_offline_;
};

} } // namespace pie::app

#endif // GAVCCOMMAND_H
