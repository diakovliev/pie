#pragma once

#include <optional>
#include <string>
#include <map>

namespace parsers::properties {

    std::optional<std::pair<std::string, std::string> > parse_property(const std::string& input);

}//namespace parsers::properties
