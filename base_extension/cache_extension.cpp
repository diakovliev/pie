#include "libpython_pie.h"
#include "cache_extension.h"
#include "utils.h"

#include <logging.h>

#include <set>

#include <gavccache.h>
#include <gavccacheinit.h>
#include <gavccacheclean.h>

Cache::Cache()
    : BaseExtension()
{
}

Cache::Cache(const Cache& src)
    : BaseExtension(src)
{
}

/*virtual*/ bool Cache::is_known_param(std::string param) {
    static std::set<std::string> known_params {
        PARAM_CACHE_PATH,
    };

    return known_params.find(param) != known_params.end();
}

int Cache::init() {
    int result = QUERY_ERROR;

    auto cache_path = get_param(PARAM_CACHE_PATH, "");

    piel::cmd::GAVCCacheInit gavccachecheinit(cache_path);

    gavccachecheinit();

    result = OK;

    return result;
}

int Cache::clean(int max_age) {
    int result = QUERY_ERROR;

    auto cache_path = get_param(PARAM_CACHE_PATH, "");

    try {
        piel::cmd::GAVCCacheClean gavccacheclean(
                         cache_path,
                         max_age);

        gavccacheclean();

        result = OK;
    }
    catch (piel::cmd::errors::cache_folder_does_not_exist& e) {
        throw Error("Cache folder not exists!");
    }

    return result;
}
