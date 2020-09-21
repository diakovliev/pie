#pragma once

#include <optional>
#include <string>

namespace art::lib::gavc {

    struct gavc_data {
        std::string group;
        std::string name;
        std::string version;
        std::string classifier;
        std::string extension;
    };

}

namespace parsers::gavc {

    std::optional<art::lib::gavc::gavc_data> parse_query(const std::string& input);

}//namespace parsers::gavc
