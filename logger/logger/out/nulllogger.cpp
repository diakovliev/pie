#include "../logbase.h"
#include "nulllogger.h"
#include <iostream>

namespace piel { namespace lib { namespace logger_out {

NullLogger::NullLogger(const std::string &name)
    : LogBase(name+"_null")
{

}

NullLogger::~NullLogger()
{

}

void NullLogger::print(const std::string& str)
{

}

} } } // namespace piel::lib::logger_out
