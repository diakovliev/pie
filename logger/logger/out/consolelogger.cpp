#include "consolelogger.h"
#include <iostream>

namespace piel { namespace lib { namespace logger_out {

const char* ConsoleLogger::sfx = "_console";

ConsoleLogger::ConsoleLogger(const std::string& name)
    : BaseLogger(name, sfx)
{

}

ConsoleLogger::~ConsoleLogger()
{

}

void ConsoleLogger::print(const std::string& str)
{
    std::cout << str;
    std::cout.flush();
}

void ConsoleLogger::print_err(const std::string& str)
{
    std::cerr << str;
    std::cerr.flush();
}

} } } // namespace piel::lib::logger_out
