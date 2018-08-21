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
#include <gavccacheinit.h>
#include "gavcconstants.h"

#include <logging.h>

#include <properties.h>

#include <boost/bind.hpp>
#include <boost_property_tree_ext.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include <ctime>

namespace al = art::lib;
namespace pl = piel::lib;
namespace fs = boost::filesystem;
namespace pt = boost::property_tree;
namespace po = boost::program_options;

namespace piel { namespace cmd {

GAVCCacheInit::GAVCCacheInit(const std::string &cache_path)
    : pl::IOstreamsHolder()
    , cache_path_(cache_path)
{
}

GAVCCacheInit::~GAVCCacheInit()
{
}

void GAVCCacheInit::operator()()
{
    if (!GAVCCache::validate(cache_path_)) {
        try {
            fs::remove_all(cache_path_);
        } catch (...) {}
        GAVCCache::init(cache_path_);
    }
}

} } // namespace piel::cmd