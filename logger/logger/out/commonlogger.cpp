#include "../logbase.h"
#include "commonlogger.h"
#include <iostream>

namespace piel { namespace lib { namespace logger_out {

CommonLogger::CommonLogger()
    : LogBase("common")
{

}

void CommonLogger::print(const std::string& str)
{
    std::cout << str;
}

void CommonLogger::print_err(const std::string& str)
{
    std::cerr << str;
    std::cerr.flush();
}

} } } // namespace piel::lib::logger_out
