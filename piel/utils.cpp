#include "utils.h"

#include <set>

namespace piel::lib {

////////////////////////////////////////////////////////////////////////////////
/*static*/ bool string2bool(std::string value) {
    static std::set<std::string> negative_values {
        "",
        "0",
        "false",
        "False",
    };

    if (value.empty()) return false;

    return negative_values.find(value) == negative_values.end();
}

}
