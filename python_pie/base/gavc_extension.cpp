#include "libpython_pie.h"
#include "gavc_extension.h"
#include "error.h"
#include "conversions.hpp"

#include <utils.h>
#include <logging.h>

#include <set>
#include <filesystem>
#include <sstream>

Gavc::Gavc()
    : BaseExtension()
    , query_results_()
    , versions_()
{
}

Gavc::Gavc(const Gavc& src)
    : BaseExtension(src)
    , query_results_(src.query_results_)
    , versions_(src.versions_)
{
}

/*virtual*/ bool Gavc::is_known_param(std::string param) {
    static std::set<std::string> known_params {
        PARAM_TOKEN,
        PARAM_SERVER,
        PARAM_REPOSITORY,
        PARAM_CACHE_PATH,
        PARAM_DOWNLOAD,
        PARAM_DELETE,
        PARAM_DELETE_VERSIONS,
        PARAM_OUTPUT,
        PARAM_DISABLE_CACHE,
        PARAM_MAX_ATTEMPTS,
        PARAM_RETRY_TIMEOUT,
        PARAM_FORCE_OFFLINE,
    };

    return known_params.find(param) != known_params.end();
}

void Gavc::update_versions() {
    std::for_each(query_results_.begin(), query_results_.end(), [&](auto& it) {
        auto ver = it.second.second;

        auto vit = std::find_if(versions_.begin(), versions_.end(), [ver](const auto& it) { return ver == it; });
        if (vit == versions_.end())
            versions_.push_back(ver);
    });
}

int Gavc::perform(std::string query_str) {
    int result = QUERY_ERROR;

    // Parce query
    LOGT << "query to perform: " << query_str << ELOG;

    std::optional<art::lib::GavcQuery> parsed_query = art::lib::GavcQuery::parse(query_str);
    if (!parsed_query)
    {
        return result;
    }

    auto query                      = *parsed_query;

    auto server_api_access_token    = get_param(PARAM_TOKEN, "");
    auto server_url                 = get_param(PARAM_SERVER, "");
    auto server_repository          = get_param(PARAM_REPOSITORY, "");
    auto cache_path                 = get_param(PARAM_CACHE_PATH, "");
    auto output_file                = get_param(PARAM_OUTPUT, "");

    auto max_attempts               = std::stoi(get_param(PARAM_MAX_ATTEMPTS, "3"));
    auto retry_timeout_s            = std::stoi(get_param(PARAM_RETRY_TIMEOUT, "15"));

    auto have_to_download_results   = piel::lib::string2bool(get_param(PARAM_DOWNLOAD, ""));
    auto force_offline              = piel::lib::string2bool(get_param(PARAM_FORCE_OFFLINE, ""));
    auto have_to_delete_results     = piel::lib::string2bool(get_param(PARAM_DELETE, ""));
    auto have_to_delete_versions    = piel::lib::string2bool(get_param(PARAM_DELETE_VERSIONS, ""));
    auto disable_cache              = piel::lib::string2bool(get_param(PARAM_DISABLE_CACHE, ""));

    auto notifications_file         = std::string();

    auto cout_stub  = std::make_unique<std::ostringstream>();
    auto cerr_stub  = std::make_unique<std::ostringstream>();
    auto cin_stub   = std::make_unique<std::istringstream>();

    piel::cmd::QueryContext context(server_api_access_token,
        server_url,
        server_repository,
        query);

    piel::cmd::DownloadOperationParameters params(have_to_download_results,
        have_to_delete_results,
        have_to_delete_versions,
        max_attempts,
        retry_timeout_s);

    if (disable_cache) {
        piel::cmd::GAVC gavc(&context,
                         &params,
                         output_file,
                         notifications_file);

        // Suppress output
        gavc.setup_iostreams(cout_stub.get(), cerr_stub.get(), cin_stub.get());

        if (output_file.empty()) {
            gavc.set_path_to_download(std::filesystem::current_path());
        }

        gavc();

        versions_      = gavc.get_versions();
        query_results_ = gavc.get_query_results();
    }
    else {
        piel::cmd::GAVCCache gavccache(&context,
                         &params,
                         cache_path,
                         output_file,
                         notifications_file,
                         force_offline);

        // Suppress output
        gavccache.setup_iostreams(cout_stub.get(), cerr_stub.get(), cin_stub.get());

        if (output_file.empty()) {
            gavccache.set_path_to_download(std::filesystem::current_path());
        }

        gavccache();

        versions_       = gavccache.get_versions();
        query_results_  = gavccache.get_query_results();
    }

    result = OK;

    return result;
}

std::vector<std::string> Gavc::versions() const {
    return versions_;
}

int Gavc::performW(std::wstring query_str) {
    return perform(conversions::w2s(query_str));
}
