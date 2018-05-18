#include <cstdarg>
#include <cstdio>
#include <locale>
#include <time.h>
#include <ctime>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <iostream>

#include "utils.h"

namespace piel { namespace lib { namespace logger_utils {

std::string time_to_str(char const * fmt, std::time_t t, std::string& str)
{
    char time_str[80];
    struct tm* ptm;
    ptm = std::localtime(&t);
    std::strftime(time_str, sizeof(time_str), fmt, ptm);
    str = time_str;
    return str;
}

std::string time_to_str(char const * fmt, std::time_t t)
{
    std::string str;
    return time_to_str(fmt, t, str);
}

std::string time_to_str(char const * fmt)
{
    std::string str;
    return time_to_str(fmt, std::time(nullptr), str);
}

std::string get_env(std::string name, std::string def)
{
    const char *env = ::getenv(name.c_str());
    if (env) return env;
    return def;
}

bool get_env(std::string name, bool def)
{
    const char *env = ::getenv(name.c_str());
    if (env) {
        std::string senv = boost::to_lower_copy<std::string>(env);
        if (senv == "true") return true;
        return false;
    }
    return def;
}

int get_env(std::string name, int def)
{
    const char *env = ::getenv(name.c_str());
    if (env) {
        return ::strtoul(env, NULL, 0);
    }
    return def;
}

std::string replace(std::string str, std::string from, std::string to)
{
    return boost::replace_all_copy(str, from, to);
}

std::string normalize(std::string loggerName)
{
    return replace(replace(loggerName, "-", "_"), " ", "_");
}

} } } // namespace piel::lib::logger_utils

