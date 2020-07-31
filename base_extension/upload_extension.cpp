#include "upload_extension.h"

#include <logging.h>
#include <upload.h>
#include <uploadcommand.h>

#include <set>

namespace al = art::lib;

const int empty_classifiers_count_allowed = 1;

bool check_empty_classifiers_count(const al::ufs::UFSVector& c)
{
    bool ret_val = true;
    int count_empty_classifier = 0;
    for (auto it = c.begin(), end = c.end(); it != end && ret_val; ++it)
    {
        if (it->classifier.empty()) {
            count_empty_classifier++;
            LOGD << "Find empty classifier: " << al::ufs::to_string(*it) << "(" << count_empty_classifier << ")" << LOGE;
        }
        ret_val &= count_empty_classifier <= empty_classifiers_count_allowed;
    }
    return ret_val;
}

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

    auto classifier_vector = result_parse->get_data();
    if (!check_empty_classifiers_count(classifier_vector)) {
        return result;
    }

    try
    {
        piel::cmd::Upload upload;

        upload.set_server_url(server_url);
        upload.set_server_api_access_token(server_api_access_token);
        upload.set_server_repository(server_repository);
        upload.set_query(query);
        upload.set_classifiers(classifier_vector);

        upload();

        result = OK;
    }
    catch (const piel::cmd::errors::nothing_to_upload&) {
        throw Error("Nothing to upload!");
    }
    catch (const piel::cmd::errors::file_upload_error&) {
        throw Error("File upload error!");
    }
    catch (const piel::cmd::errors::pom_upload_error&) {
        throw Error("POM upload error!");
    }
    catch (const piel::cmd::errors::uploading_checksum_error& e) {
        throw Error("Checksum upload error!");
    }

    return result;
}
