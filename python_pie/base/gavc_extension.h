#pragma once

#include <gavccache.h>

#include <string>
#include <map>
#include <vector>

#include "libpython_pie.h"
#include "base_extension.h"

class Gavc: public BaseExtension {
public:
    static constexpr auto PARAM_DOWNLOAD         = "download";
    static constexpr auto PARAM_DELETE           = "delete";
    static constexpr auto PARAM_DELETE_VERSIONS  = "delete-versions";
    static constexpr auto PARAM_OUTPUT           = "output";
    static constexpr auto PARAM_DISABLE_CACHE    = "disable-cache";
    static constexpr auto PARAM_MAX_ATTEMPTS     = "max-attempts";
    static constexpr auto PARAM_RETRY_TIMEOUT    = "retry-timeout";
    static constexpr auto PARAM_FORCE_OFFLINE    = "force-offline";

public:
    Gavc();
    Gavc(const Gavc&);
    ~Gavc() = default;

    std::vector<std::string> versions() const;
    int perform(std::string query_str);

protected:
    virtual bool is_known_param(std::string param);
    void update_versions();

private:
    piel::cmd::GAVC::query_results      query_results_;
    std::vector<std::string>            versions_;

};
