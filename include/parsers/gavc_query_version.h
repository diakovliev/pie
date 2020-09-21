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
    };

    using VersionOps = std::vector<OpType>;
}

namespace parsers::gavc {

    std::optional<art::lib::gavc::VersionOps> parse_version(const std::string& input);

}//namespace parsers::gavc
