#pragma once

#include <optional>
#include <string>

namespace art::lib::gavc {

    enum RangeFlags {
        RangeFlags_exclude_all      = 0x00,
        RangeFlags_include_left     = 0x01,
        RangeFlags_include_right    = 0x02,
    };

    struct gavc_versions_range_data {
        std::string left;
        std::string right;
        unsigned char flags;
    };

}

namespace parsers::gavc {

    std::optional<art::lib::gavc::gavc_versions_range_data> parse_range(const std::string& input);

}//namespace parsers::gavc
