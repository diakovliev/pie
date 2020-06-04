#include <boost/python.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>

#include <logging.h>
#include <gavccache.h>

#include <string>
#include <map>
#include <set>
#include <vector>
#include <exception>
#include <filesystem>

#define MODULE_VESRION 1

////////////////////////////////////////////////////////////////////////////////
enum Errors {
    OK = 0,
    UNKNOWN_PARAM,
    QUERY_ERROR,
};

////////////////////////////////////////////////////////////////////////////////
int version() {
    return 1;
}

////////////////////////////////////////////////////////////////////////////////
struct Error: public std::exception {
    Error(std::string what, int err = 0): what_(std::move(what)), err_(err) { }

    char const* what() throw() { return what_.c_str(); }

private:
    std::string what_;
    int err_;
};

////////////////////////////////////////////////////////////////////////////////
void translate(std::exception const& e)
{
    PyErr_SetString(PyExc_RuntimeError, e.what());
}

////////////////////////////////////////////////////////////////////////////////
class Gavc {
private:
    std::map<std::string, std::string> settings_;
    piel::cmd::GAVC::query_results query_results_;
    std::vector<std::string> versions_;

public:
    static constexpr auto PARAM_TOKEN            = "token";
    static constexpr auto PARAM_TOKEN_ENV        = "GAVC_SERVER_API_ACCESS_TOKEN";

    static constexpr auto PARAM_SERVER           = "server";
    static constexpr auto PARAM_SERVER_ENV       = "GAVC_SERVER_URL";

    static constexpr auto PARAM_REPOSITORY       = "repository";
    static constexpr auto PARAM_REPOSITORY_ENV   = "GAVC_SERVER_REPOSITORY";

    static constexpr auto PARAM_CACHE_PATH       = "cache-path";
    static constexpr auto PARAM_CACHE_PATH_ENV   = "GAVC_CACHE";

    static constexpr auto PARAM_DOWNLOAD         = "download";
    static constexpr auto PARAM_DELETE           = "delete";
    static constexpr auto PARAM_OUTPUT           = "output";
    static constexpr auto PARAM_DISABLE_CACHE    = "disable-cache";
    static constexpr auto PARAM_MAX_ATTEMPTS     = "max-attempts";
    static constexpr auto PARAM_RETRY_TIMEOUT    = "retry-timeout";
    static constexpr auto PARAM_FORCE_OFFLINE    = "force-offline";

protected:
    void fill_param_from_env(std::string param, std::string env, std::optional<std::string> default_value = std::nullopt) {
        auto val = ::getenv(env.c_str());
        if (val != nullptr) {
            settings_[param] = std::string(val);
        } else if (default_value) {
            settings_[param] = *default_value;
        }
    }

    void fill_default_params() {
        fill_param_from_env(PARAM_TOKEN,        PARAM_TOKEN_ENV);
        fill_param_from_env(PARAM_SERVER,       PARAM_SERVER_ENV);
        fill_param_from_env(PARAM_REPOSITORY,   PARAM_REPOSITORY_ENV);
        fill_param_from_env(PARAM_CACHE_PATH,   PARAM_CACHE_PATH_ENV,  piel::cmd::utils::get_default_cache_path());
    }

    bool is_known_param(std::string param) {
        static std::set<std::string> known_params {
            PARAM_TOKEN,
            PARAM_SERVER,
            PARAM_REPOSITORY,
            PARAM_CACHE_PATH,
            PARAM_DOWNLOAD,
            PARAM_DELETE,
            PARAM_OUTPUT,
            PARAM_DISABLE_CACHE,
            PARAM_MAX_ATTEMPTS,
            PARAM_RETRY_TIMEOUT,
            PARAM_FORCE_OFFLINE,
        };

        return known_params.find(param) != known_params.end();
    }

    bool string2bool(std::string value) {
        static std::set<std::string> negative_values {
            "",
            "0",
            "false",
            "False",
        };

        if (value.empty()) return false;

        return negative_values.find(value) == negative_values.end();
    }

    void update_versions() {
        // TODO Postprocess query_results
        //typedef std::map<std::filesystem::path,std::pair<std::string,std::string> > query_results;
        //query_results_.insert(std::make_pair(object_path, std::make_pair(object_classifier, version)));

        std::for_each(query_results_.begin(), query_results_.end(), [&](auto& it) {
            auto ver = it.second.second;

            auto vit = std::find_if(versions_.begin(), versions_.end(), [ver](const auto& it) { return ver == it; });
            if (vit == versions_.end())
                versions_.push_back(ver);
        });
    }

public:
    Gavc()
        : settings_()
        , query_results_()
        , versions_()
    {
        fill_default_params();
    }

    int set_param(std::string param, std::string value) {
        if (!is_known_param(param)) return UNKNOWN_PARAM;

        settings_[ param ] = value;

        return OK;
    }

    std::string const& get_param(std::string param, std::string default_value) {
        if (!is_known_param(param))
            throw Error(std::string("Unknown param: ") + param + "!", UNKNOWN_PARAM);

        auto it = settings_.find(param);
        if (it == settings_.end())
            return std::move(default_value);

        return it->second;
    }

    int perform(std::string query_str) {
        int result = QUERY_ERROR;

        // Parce query
        LOGT << "query to perform: " << query_str << ELOG;

        std::optional<art::lib::GavcQuery> parsed_query = art::lib::GavcQuery::parse(query_str);
        if (!parsed_query)
        {
            return result;
        }

        auto query_ = *parsed_query;

        auto server_api_access_token_    = get_param(PARAM_TOKEN, "");
        auto server_url_                 = get_param(PARAM_SERVER, "");
        auto server_repository_          = get_param(PARAM_REPOSITORY, "");
        auto cache_path_                 = get_param(PARAM_CACHE_PATH, "");
        auto output_file_                = get_param(PARAM_OUTPUT, "");

        auto max_attempts_               = std::stoi(get_param(PARAM_MAX_ATTEMPTS, "3"));
        auto retry_timeout_s_            = std::stoi(get_param(PARAM_RETRY_TIMEOUT, "15"));

        auto have_to_download_results_   = string2bool(get_param(PARAM_DOWNLOAD, ""));
        auto force_offline_              = string2bool(get_param(PARAM_FORCE_OFFLINE, ""));
        auto have_to_delete_results_     = string2bool(get_param(PARAM_DELETE, ""));
        auto disable_cache_              = string2bool(get_param(PARAM_DISABLE_CACHE, ""));

        auto notifications_file_         = "";

        try {
            if (disable_cache_) {
                piel::cmd::GAVC gavc(
                    server_api_access_token_,
                    server_url_,
                    server_repository_,
                    query_,
                    have_to_download_results_,
                    output_file_,
                    notifications_file_,
                    max_attempts_,
                    retry_timeout_s_,
                    force_offline_,
                    have_to_delete_results_
                );

                if (output_file_.empty()) {
                    gavc.set_path_to_download(std::filesystem::current_path());
                }

                gavc();

                versions_      = gavc.get_versions();
                query_results_ = gavc.get_query_results();
            }
            else {
                piel::cmd::GAVCCache gavccache(
                    server_api_access_token_,
                    server_url_,
                    server_repository_,
                    query_,
                    have_to_download_results_,
                    cache_path_,
                    output_file_,
                    notifications_file_,
                    max_attempts_,
                    retry_timeout_s_,
                    force_offline_,
                    have_to_delete_results_
                );

                if (output_file_.empty()) {
                    gavccache.set_path_to_download(std::filesystem::current_path());
                }

                gavccache();

                versions_       = gavccache.get_versions();
                query_results_  = gavccache.get_query_results();
            }

            result = OK;
        }
        catch (piel::cmd::errors::unable_to_parse_maven_metadata&) {
        }
        catch (piel::cmd::errors::no_server_maven_metadata& e) {
        }
        catch (piel::cmd::errors::error_processing_version& e) {
        }
        catch (piel::cmd::errors::cant_get_maven_metadata& ) {
        }
        catch (piel::cmd::errors::cant_find_version_for_query& ) {
        }
        catch (piel::cmd::errors::gavc_download_file_error& ) {
        }
        catch (piel::cmd::errors::gavc_delete_remote_file_error& e) {
        }
        catch (piel::cmd::errors::cache_no_cache_for_query& e) {
        }
        catch (piel::cmd::errors::cache_no_file_for_classifier& e) {
        }
        catch (piel::cmd::errors::cache_not_valid_file& e) {
        }

        return result;
    }

    std::vector<std::string> versions() const {
        return versions_;
    }

};

////////////////////////////////////////////////////////////////////////////////
BOOST_PYTHON_MODULE(libpython_pie)
{
    using namespace boost::python;

    register_exception_translator<Error>(&translate);

    class_<std::vector<std::string> >("StringVec")
        .def(vector_indexing_suite<std::vector<std::string> >());

    def("version", version);

    class_<Gavc>("Gavc")
        .def("set_param",   &Gavc::set_param, args("param", "value"))
        .def("get_param",   &Gavc::get_param, args("param", "default_value"),   return_value_policy<copy_const_reference>())
        .def("perform",     &Gavc::perform,   args("gavc"))
        .def("versions",    &Gavc::versions)
        ;
}
