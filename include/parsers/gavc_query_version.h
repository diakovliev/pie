#pragma once

#include <map>
#include <vector>
#include <string>

namespace art::lib::gavc {

    enum Ops {
        Op_const,
        Op_all,
        Op_latest,
        Op_oldest,
    };

    struct OpType: public std::pair<Ops,std::string>
    {
        OpType()                                : std::pair<Ops,std::string>(Op_const, "")      {}
        OpType(const std::string& val)          : std::pair<Ops,std::string>(Op_const, val)     {}
        OpType(Ops op, const std::string& val)  : std::pair<Ops,std::string>(op, val)           {}
        OpType(Ops op, char val)                : std::pair<Ops,std::string>()
        {
            first = op;
            second.push_back(val);
        }

        bool is_const() const {
            return first == Op_const;
        }
    };

    using VersionOps = std::vector<OpType>;

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

    std::optional<
        std::pair<
            std::optional<art::lib::gavc::VersionOps>,
            std::optional<art::lib::gavc::gavc_versions_range_data>
        >
    > parse_version(const std::string& input);

}//namespace parsers::gavc
