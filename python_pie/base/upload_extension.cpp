#include "libpython_pie.h"
#include "upload_extension.h"
#include "pyutils.h"
#include "conversions.hpp"

#include <logging.h>
#include <upload.h>
#include <uploadcommand.h>

#include <set>

namespace al = art::lib;

Upload::Upload()
    : BaseExtension()
{
}

Upload::Upload(const Upload& src)
    : BaseExtension(src)
{
}

/*virtual*/ bool Upload::is_known_param(std::string param) {
    static std::set<std::string> known_params {
        PARAM_TOKEN,
        PARAM_SERVER,
        PARAM_REPOSITORY,
        PARAM_FILELIST,
    };

    return known_params.find(param) != known_params.end();
}

int Upload::perform(std::string query_str) {
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
    auto classifiers_str            = get_param(PARAM_FILELIST, "");

    // Gen file list
    LOGT << "classifiers_str:" << classifiers_str << ELOG;

    al::UploadFileSpec spec;

    std::optional<al::UploadFileSpec> result_parse = spec.parse(classifiers_str);
    if (!result_parse) {
        return result;
    }

    piel::cmd::Upload upload;

    upload.set_server_url(server_url);
    upload.set_server_api_access_token(server_api_access_token);
    upload.set_server_repository(server_repository);
    upload.set_query(query);
    upload.set_classifiers(result_parse->get_data());

    upload();

    result = OK;

    return result;
}

int Upload::performW(std::wstring query_str) {
    return perform(conversions::w2s(query_str));
}
