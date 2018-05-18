#pragma once

#include "../logbase.h"
#include "baselogger.h"

namespace piel { namespace lib { namespace logger_out {

class CommonLogger : public logger::LogBase {
public:
    CommonLogger();
    virtual ~CommonLogger() {}

protected:
    virtual void print(const std::string& str);
    virtual void print_err(const std::string& str);

};

} } } // namespace piel::lib::logger_out
