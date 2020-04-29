#pragma once

#include <string>
#include <memory>

#include "log.h"
#include "logmessage.h"

namespace piel { namespace lib { namespace logger {

class LogBase : public Log
{
public:
    virtual bool is_debug_enabled() { return is_debug_enabled_; }
    virtual bool is_error_enabled() { return is_error_enabled_; }
    virtual bool is_fatal_enabled() { return is_fatal_enabled_; }
    virtual bool is_info_enabled()  { return is_info_enabled_; }
    virtual bool is_trace_enabled() { return is_trace_enabled_; }
    virtual bool is_warn_enabled()  { return is_warn_enabled_; }

    virtual void trace(const std::string& var1) { if (is_trace_enabled()) print(var1); }
    virtual void debug(const std::string& var1) { if (is_debug_enabled()) print(var1); }
    virtual void info(const std::string& var1) { if (is_info_enabled()) print(var1); }
    virtual void warn(const std::string& var1) { if (is_warn_enabled()) print_warn(var1); }
    virtual void error(const std::string& var1) { if (is_error_enabled()) print_err(var1); }
    virtual void fatal(const std::string& var1) { if (is_fatal_enabled()) print_err(var1); }

    virtual void enable(const log_type& type) {}
    virtual void disable(const log_type& type) {}

    virtual void print_message(const LogMessage & m) { print(m.message); }

    virtual std::string get_mame() { return name_; }

    LogBase(const std::string& n);// : name(n) {}
    virtual ~LogBase();// {}

protected:
    virtual void print(const std::string& var1) {}
    virtual void print_warn(const std::string& var1) {print(var1);}
    virtual void print_err(const std::string& var1) {print(var1);}

protected:
    std::string name_;
    bool is_fatal_enabled_; // 0
    bool is_error_enabled_; // 1
    bool is_warn_enabled_;  // 2
    bool is_info_enabled_;  // 3
    bool is_debug_enabled_; // 4
    bool is_trace_enabled_; // 5
};

} } } // namespace piel::lib::logger
