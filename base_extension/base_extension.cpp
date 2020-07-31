#include "libpython_pie.h"
#include "base_extension.h"
#include <gavccache.h>

BaseExtension::BaseExtension()
    : parameters_() 
{
    fill_default_params();
}

BaseExtension::BaseExtension(const BaseExtension& src)
    : parameters_(src.parameters_)
{
}

void BaseExtension::fill_param_from_env(std::string param, std::string env, std::optional<std::string> default_value) {
    auto val = ::getenv(env.c_str());
    if (val != nullptr) {
        parameters_[param] = std::string(val);
    } else if (default_value) {
        parameters_[param] = *default_value;
    }
}

void BaseExtension::fill_default_params() {
    if (is_known_param(PARAM_TOKEN))        fill_param_from_env(PARAM_TOKEN,        PARAM_TOKEN_ENV);
    if (is_known_param(PARAM_SERVER))       fill_param_from_env(PARAM_SERVER,       PARAM_SERVER_ENV);
    if (is_known_param(PARAM_REPOSITORY))   fill_param_from_env(PARAM_REPOSITORY,   PARAM_REPOSITORY_ENV);
    if (is_known_param(PARAM_CACHE_PATH))   fill_param_from_env(PARAM_CACHE_PATH,   PARAM_CACHE_PATH_ENV,  piel::cmd::utils::get_default_cache_path());
}

int BaseExtension::set_param(std::string param, std::string value) {
    if (!is_known_param(param)) return UNKNOWN_PARAM;

    parameters_[ param ] = value;

    return OK;
}

std::string const& BaseExtension::get_param(std::string param, std::string default_value) {
    if (!is_known_param(param))
        throw Error(std::string("Unknown param: ") + param + "!", UNKNOWN_PARAM);

    auto it = parameters_.find(param);
    if (it == parameters_.end())
        return std::move(default_value);

    return it->second;
}
