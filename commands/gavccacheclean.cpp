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

#include "CacheDirectory.h"
#include "CacheVersion.h"

#include <ctime>

namespace piel::cmd {

    GAVCCacheClean::GAVCCacheClean(const std::string &cache_path, int max_age)
        : pl::IOstreamsHolder()
        , cache_path_(cache_path)
        , max_age_(max_age)
    {
    }


    void GAVCCacheClean::operator()()
    {
        CacheDirectory cache_dir(cache_path_);

        if (!cache_dir.validate()) {
            LOGE << "Cache validation failed!" << ELOG;
            return;
        }

        for(auto& artifact_cache: cache_dir.artifacts()) {
            for(auto version: artifact_cache.versions()) {

                auto v = artifact_cache.load_version(version);

                LOGT << "Version: " << version << ELOG;

                for(auto object: v.objects()) {

                    LOGT << "Object: " << object.object() << ELOG;

                    std::tm     last_access_tm   = object.load_last_access_time();
                    std::time_t last_access_time = std::mktime(&last_access_tm);
                    std::time_t current_time     = std::time(nullptr);

                    int days = int(std::difftime(current_time, last_access_time)/(GAVCConstants::seconds_in_day));

                    //std::ostringstream last_access_time_buffer;
                    //last_access_time_buffer << std::put_time(&last_access_tm, GAVCConstants::last_access_time_format.c_str());
                    //LOGT << "last_access_time:" << last_access_time_buffer.str() << ELOG;

                    LOGT << "age (days)" << ((days >= max_age_)?"(OLD)":"(NEW)") << " :" << days << " for " << object.object().filename().c_str() <<ELOG;
                    if (days >= max_age_) {
                        object.delete_object();
                    }
                }
            }
        }
    }

} // namespace piel::cmd
