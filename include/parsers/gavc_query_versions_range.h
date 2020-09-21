#pragma once

#include <optional>
#include <string>

namespace parsers::gavc {

    enum RangeFlags {
        RangeFlags_exclude_all      = 0x00,
        RangeFlags_include_left     = 0x01,
        RangeFlags_include_right    = 0x02,
    };

    struct Range {
        std::string left;
        std::string right;
        unsigned char flags;
    };

    std::optional<Range> parse_range(const std::string& input);

}//namespace parsers::gavc
