#pragma once

#include <exception>
#include <sstream>

namespace piel::cmd::errors {

    static inline std::string format_exceptions_stack(const std::exception& e, int level = 0) {
        std::string ret(e.what());
        try {
            std::rethrow_if_nested(e);
        } catch (const std::exception& e) {
            ret += " <== " + format_exceptions_stack(e, level + 1);
        }
        return ret;
    }

}//namespace piel::cmd::errors
