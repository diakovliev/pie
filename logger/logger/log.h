#pragma once

#include <string>
#include <list>
#include <memory>
#include "logmessage.h"

namespace piel { namespace lib { namespace logger {

class Log {
public:
    virtual bool is_debug_enabled() = 0;
    virtual bool is_error_enabled() = 0;
    virtual bool is_fatal_enabled() = 0;
    virtual bool is_info_enabled() = 0;
    virtual bool is_trace_enabled() = 0;
    virtual bool is_warn_enabled() = 0;

    virtual void trace(const std::string& var1) = 0;
    virtual void debug(const std::string& var1) = 0;
    virtual void info(const std::string& var1) = 0;
    virtual void warn(const std::string& var1) = 0;
    virtual void error(const std::string& var1) = 0;
    virtual void fatal(const std::string& var1) = 0;

    virtual void print_message(const LogMessage & m) = 0;
    virtual std::string get_mame() = 0;

    Log() {};
    virtual ~Log() {};

protected:
    virtual void print(const std::string& var1) = 0;
    virtual void print_warn(const std::string& var1) = 0;
    virtual void print_err(const std::string& var1) = 0;

protected:
    std::string name;
};

using LogPtr        = std::shared_ptr<Log>;
using LogPtrList    = std::list<LogPtr>;

} } } // namespace piel::lib::logger
