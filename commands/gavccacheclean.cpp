/*
 * Copyright (c) 2017-2018
 *
 *  Dmytro Iakovliev daemondzk@gmail.com
 *  Oleksii Kogutenko https://github.com/oleksii-kogutenko
 *
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

#include <ctime>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <vector>

#include <gavccache.h>
#include <gavccacheclean.h>
#include "gavcconstants.h"

#include <logging.h>

#include <properties.h>

#include <boost_property_tree_ext.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <ctime>

namespace al = art::lib;
namespace pl = piel::lib;
namespace fs = std::filesystem;
namespace pt = boost::property_tree;
namespace po = boost::program_options;

namespace piel { namespace cmd {

GAVCCacheClean::GAVCCacheClean(const std::string &cache_path
           , int max_age)
    : pl::IOstreamsHolder()
    , cache_path_(cache_path)
    , max_age_(max_age)
{
}

GAVCCacheClean::~GAVCCacheClean()
{
}

bool GAVCCacheClean::clean(fs::path path)
{
    bool do_remove = true;

    if(!fs::is_directory(path)) {
        throw piel::cmd::errors::cache_folder_does_not_exist(path.generic_string());
    }

    for(auto entry : boost::make_iterator_range(fs::directory_iterator(path), {})){
        LOGT << entry.path().c_str() << ELOG;

        if(fs::is_directory(entry)) {
            bool do_remove_entry = clean(entry.path());
            do_remove           &= do_remove_entry;

            LOGT << "->" << entry.path().c_str() << " do_remove:" << do_remove << " do_remove_entry:" << do_remove_entry << ELOG;

            if (do_remove_entry) {
                cout() << "Removing " << entry.path().c_str() << std::endl;
                cout().flush();

                LOGT << "Removing " << entry.path().c_str() << ELOG;

                fs::remove_all(entry.path());
            }
        } else if (fs::is_regular_file(entry)
                   && (entry.path().extension() != GAVCConstants::properties_ext) ) {

            std::tm     last_access_tm   = GAVCCache::get_last_access_time(entry.path().generic_string());
            std::time_t last_access_time = std::mktime(&last_access_tm);
            std::time_t current_time     = std::time(nullptr);

            int days = int(std::difftime(current_time, last_access_time)/(GAVCConstants::seconds_in_day));

            //std::ostringstream last_access_time_buffer;
            //last_access_time_buffer << std::put_time(&last_access_tm, GAVCConstants::last_access_time_format.c_str());
            //LOGT << "last_access_time:" << last_access_time_buffer.str() << ELOG;

            LOGT << "age (days)" << ((days >= max_age_)?"(OLD)":"(NEW)") << " :" << days << " for " << entry.path().filename().c_str() <<ELOG;

            do_remove &= days >= max_age_;
        }
    }

    return do_remove;
}


void GAVCCacheClean::operator()()
{
    if (!GAVCCache::validate(cache_path_)) {
        LOGE << "Cache validate failed!!!" << ELOG;
        return;
    }

    clean(cache_path_);
}

} } // namespace piel::cmd
