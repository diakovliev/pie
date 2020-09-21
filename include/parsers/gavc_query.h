#pragma once

#include <optional>
#include <string>

namespace parsers::gavc {

    struct GavcQuery {
        std::string group;
        std::string name;
        std::optional<std::string> version;
        std::optional<std::string> classifier;
        std::optional<std::string> extension;
    };

    std::optional<GavcQuery> parse_query(const std::string& input);

}//namespace parsers::gavc
