/*
 * Copyright (c) 2018, 2021, diakovliev
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

#include <ctime>
#include <iomanip>
#include <sstream>
#include <fstream>

#include "CacheObject.h"

#include "GavcUtils.h"
#include "gavcconstants.h"

namespace piel::cmd {

    CacheObject::CacheObject(const CacheVersion* parent, fs::path object)
        : parent_(parent)
        , object_(object)
    {
    }


    const CacheVersion* CacheObject::parent() const {
        return parent_;
    }


    fs::path CacheObject::object() const {
        return object_;
    }


    fs::path CacheObject::properties_file() const {
        return properties_file(object_);
    }

    /*static*/ fs::path CacheObject::properties_file(const fs::path& object_path) {
        return fs::path(object_path.string() + GAVCConstants::properties_ext);
    }


    /*static*/ bool CacheObject::is_object_properties(const fs::path& file_path) {
        return !is_object(file_path) && ::utils::string::endswith(file_path.string(), GAVCConstants::properties_ext);
    }


    /*static*/ bool CacheObject::is_object(const fs::path& object_path) {
        auto properties_path = properties_file(object_path);
        return fs::exists(properties_path);
    }


    /*static*/ void CacheObject::write_object_properties(const fs::path& object_path, const pl::Properties& props) {
        auto properties_path = properties_file(object_path);
        std::ofstream ofs(properties_path.string().c_str());
        props.store(ofs);
    }


    pl::Properties CacheObject::properties() const {
        std::ifstream ifs(properties_file());
        return pl::Properties::load(ifs);
    }


    std::string CacheObject::classifier() const {
        return ::utils::gavc::classifier_significant_part(properties().get(GAVCConstants::object_classifier_property, ""));
    }


    bool CacheObject::is_valid() const {
        return ::utils::gavc::validate_local_file(object_, properties());
    }


    void CacheObject::store_properties(const pl::Properties& props) const {
        write_object_properties(object_, props);
    }


    /*static*/ std::string CacheObject::now_string()
    {
        std::ostringstream buffer;
        std::time_t tm = std::time(nullptr);
        buffer << std::put_time(std::localtime(&tm), GAVCConstants::last_access_time_format.c_str());
        return buffer.str();
    }


    void CacheObject::update_last_access_time() const {
        auto props = properties();
        props.set(GAVCConstants::last_access_time_property, now_string());
        store_properties(props);
    }


    std::tm CacheObject::load_last_access_time() const {
        pl::Properties props = properties();
        std::tm t = {};

        std::istringstream ss(props.get(GAVCConstants::last_access_time_property, now_string()));
        ss >> std::get_time(&t, GAVCConstants::last_access_time_format.c_str());

        t.tm_isdst = 1;

        return t;
    }


    std::string CacheObject::load_classifier() const {
        pl::Properties props = properties();
        return props.get(GAVCConstants::object_classifier_property, "");
    }


    void CacheObject::delete_object() const {
        fs::remove(properties_file());
        fs::remove(object());
    }


    void CacheObject::uncache_object_to(const fs::path& destination) const {
        fs::copy_file(object(), destination, fs::copy_options::overwrite_existing);
        update_last_access_time();
    }

} // namespace piel::cmd
