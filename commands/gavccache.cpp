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

#include "gavccache.h"

#include <logging.h>

#include <commands/SleepFor.hpp>
#include <commands/Retrier.hpp>

#include "CacheDirectory.h"
#include "QueryCache.h"
#include "GavcUtils.h"

namespace piel::cmd {

    GAVCCache::GAVCCache( const QueryContext *context
             , const QueryOperationParameters *params
             , const std::string& cache_path
             , const std::string& output_file
             , const std::string& notifications_file
             , bool force_offline)
        : QueryOperation(context, params)
        , pl::IOstreamsHolder()
        , path_to_download_()
        , output_file_(output_file)
        , cache_path_(cache_path)
        , notifications_file_(notifications_file)
        , force_offline_(force_offline)
        , query_results_()
        , list_of_queued_files_()
        , versions_()
    {
    }


    bool GAVCCache::is_force_offline() const
    {
        bool offline = force_offline_;
        if (!offline) {
            const char *offline_str = ::getenv("PIE_GAVC_FORCE_OFFLINE");
            if (offline_str) {
                offline = std::string(offline_str) == "1" || std::string(offline_str) == "true";
            }
        }
        LOGT << "Force offline mode: " << offline << ELOG;
        return offline;
    }


    void GAVCCache::perform() {

        CacheDirectory cache_dir(cache_path_);
        cache_dir.reinit();

        GAVC gavc(context(),
             params(),
             std::string(),
             notifications_file_);

        gavc.setup_iostreams(&cout(), &cerr(), &cin());

        QueryCache cache(context(), cache_path_, &gavc);

        std::vector<std::string> versions_to_process_remote;
        std::vector<std::string> versions_to_process_cache;
        std::vector<std::string> versions_to_process;

        bool force_offline      = is_force_offline();
        bool offline            = force_offline;

        std::vector<CacheObject> cached_objects;

        std::optional<std::string> remote_exception;

        if (!offline) {
            try {
                versions_to_process_remote  = gavc.get_versions_to_process();
            }
            catch (const std::exception& e) {
                // Query error. Force use cache.
                offline                     = true;
                remote_exception            = errors::format_exceptions_stack(e);
            }
        }

        LOGT << "force_offline: " << force_offline << " offline: " << offline << ELOG;

        if (offline) {

            try {
                versions_to_process_cache   = cache.versions();
                cached_objects              = cache.objects();

            } catch (const std::exception &e) {
                // Error on retrieving cached data.
            }

            for (auto ver = versions_to_process_cache.begin(), end = versions_to_process_cache.end(); ver != end; ++ver) {
                cout() << "Version: " << *ver << std::endl;
                if (force_offline) {
                    cout() << "Mode: force offline"     << std::endl;
                } else {
                    cout() << "Mode: offline"           << std::endl;
                }
            }

            for (auto cached_object: cached_objects) {

                fs::path object_name = cached_object.object().filename();

                LOGT << cached_object.object() << "->" << object_name << ELOG;

                cout() << "c " << object_name.string() << std::endl;
            }

            versions_to_process = versions_to_process_cache;

        } else {

            for (auto i = versions_to_process_remote.begin(), end = versions_to_process_remote.end(); i != end; ++i) {
                LOGT << "Cache version: " << *i  << " objects." << ELOG;

                cached_objects = cache.cache_version_objects(*i);
            }

            versions_to_process = versions_to_process_remote;
        }

        if (versions_to_process.empty() && cached_objects.empty()) {
            LOGT << "No versions to process!" << ELOG;
            if(remote_exception) {
                LOGT << "Raise exception remote exception." << ELOG;
                throw std::runtime_error(*remote_exception);
            } else {
                LOGT << "Raise exception." << ELOG;
                throw std::runtime_error("No versions to process!");
            }
        }

        versions_ = cache.get_gavc_versions();

        if (params()->have_to_download_results()) {

            if (offline && cached_objects.empty()) {
                throw std::runtime_error("No cached items for query: " + context()->query().to_string() + "!");
            }

            for (auto cached_object: cached_objects) {

                fs::path path        = output_file_.empty()         ?   cached_object.object().filename().string()  : output_file_                          ;
                fs::path object_path = path_to_download_.empty()    ?   path                                        : path_to_download_ / path.filename()   ;

                LOGT << cached_object.object() << "->" << object_path << ELOG;

                cached_object.uncache_object_to(object_path);

                std::string version             = cached_object.parent()->version();
                std::string object_classifier   = cached_object.load_classifier();

                cout() << "+ " << object_path.filename().string() << std::endl;

                query_results_.insert(std::make_pair(object_path, std::make_pair(object_classifier, version)));
                list_of_queued_files_.push_back(object_path.string());
            }
        }

        if (params()->have_to_delete_results()) {

            for (auto cached_object: cached_objects) {
                cout() << "cx " << cached_object.object().filename().string() << std::endl;

                cached_object.delete_object();
            }
        }

        if (params()->have_to_delete_versions()) {

            for (auto version_to_remove: versions_to_process) {
                LOGT << "Remove cached version: " << version_to_remove << ELOG;

                cache.delete_version(version_to_remove);

                cout() << "cX " << version_to_remove << std::endl;
            }
        }
    }

    void GAVCCache::operator()()
    {
        if (is_force_offline()) {
            perform();
            return;
        }

        Retrier<SleepFor<> >(
            [this] (auto attempt, auto max) -> bool {
                LOGT << "GAVCCache query attempt: " << attempt << " from: " << max << ELOG;
                perform();
                return true;
            },
            std::max(1u, params()->max_attempts()),
            std::max(5u, params()->retry_timeout_s())
        )(
            [](auto attempt, auto max, const auto& e) {
                LOGT << "GAVCCacche query attempt: " << attempt << " from: " << max << " error: " << errors::format_exceptions_stack(e) << ELOG;
            }
        );
    }

    void GAVCCache::set_path_to_download(const std::filesystem::path& path)
    {
        path_to_download_ = path;
    }

    std::filesystem::path GAVCCache::get_path_to_download() const
    {
        return path_to_download_;
    }

    GAVC::query_results GAVCCache::get_query_results() const
    {
        return query_results_;
    }

    GAVC::paths_list GAVCCache::get_list_of_queued_files() const
    {
        return list_of_queued_files_;
    }

    std::vector<std::string> GAVCCache::get_versions() const
    {
        return versions_;
    }

} // namespace piel::cmd
