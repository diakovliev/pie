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

#include "gavcconstants.h"
#include <iostream>
#include <sstream>

namespace piel::cmd {

/*static*/ const std::string GAVCConstants::empty_classifier           = "<none>";
/*static*/ const std::string GAVCConstants::properties_ext             = ".properties";
/*static*/ const std::string GAVCConstants::object_id_property         = "object_id";
/*static*/ const std::string GAVCConstants::object_classifier_property = "object_classifier";

/*static*/ const std::string GAVCConstants::cache_version              = "1.1";
/*static*/ const std::string GAVCConstants::cache_version_property     = "cache_version";
/*static*/ const std::string GAVCConstants::cache_properties_filename  = ".properties";

/*static*/ const std::string GAVCConstants::last_access_time_property  = "last_access_time";
/*static*/ const std::string GAVCConstants::last_access_time_format    = "%d.%m.%Y %T";

/*static*/ const int GAVCConstants::seconds_in_day                     = 60*60*24;

} // namespace piel::cmd
