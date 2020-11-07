#pragma once

#include <string>
#include <map>

class BaseExtension {
public:
    static constexpr auto PARAM_TOKEN            = "token";
    static constexpr auto PARAM_TOKEN_ENV        = "GAVC_SERVER_API_ACCESS_TOKEN";

    static constexpr auto PARAM_SERVER           = "server";
    static constexpr auto PARAM_SERVER_ENV       = "GAVC_SERVER_URL";

    static constexpr auto PARAM_REPOSITORY       = "repository";
    static constexpr auto PARAM_REPOSITORY_ENV   = "GAVC_SERVER_REPOSITORY";

    static constexpr auto PARAM_CACHE_PATH       = "cache-path";
    static constexpr auto PARAM_CACHE_PATH_ENV   = "GAVC_CACHE";

    BaseExtension();
    BaseExtension(const BaseExtension&);
    ~BaseExtension() = default;

    int set_param(std::string param, std::string value);
    std::string const& get_param(std::string param, std::string default_value);

    int set_paramW(std::wstring param, std::wstring value);
    std::string const& get_paramW(std::wstring param, std::wstring default_value);

protected:
    void fill_param_from_env(std::string param, std::string env, std::optional<std::string> default_value = std::nullopt);
    void fill_default_params();
    virtual bool is_known_param(std::string param) { return true; };

private:
    std::map<std::string, std::string> parameters_;

};
