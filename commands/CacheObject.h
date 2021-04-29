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

#pragma once

#include <filesystem>
#include <functional>

#include <properties.h>

namespace piel::cmd {

    namespace fs = std::filesystem;
    namespace pl = piel::lib;

    class CacheVersion;

    class CacheObject {
    public:
        using Filter = std::function<bool(const CacheObject&)>;

        CacheObject(const CacheVersion* parent, fs::path object);

        fs::path object() const;
        fs::path properties_file() const;

        static fs::path properties_file(const fs::path& object_path);
        static bool is_object_properties(const fs::path& file_path);
        static bool is_object(const fs::path& object_path);
        static void write_object_properties(const fs::path& object_path, const pl::Properties& props);

        pl::Properties properties() const;
        std::string classifier() const;

        bool is_valid() const;

        const CacheVersion* parent() const;

        void update_last_access_time() const;
        std::tm load_last_access_time() const;

        std::string load_classifier() const;

        void store_properties(const pl::Properties& props) const;
        void delete_object() const;

        void uncache_object_to(const fs::path& destination) const;

    protected:
        static std::string now_string();

    private:
        const CacheVersion* parent_;

        fs::path object_;

    };

} // namespace piel::cmd
