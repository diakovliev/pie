#pragma once

#include <string>

#include "../logbase.h"
#include "../logmessage.h"

namespace errors {
    struct not_implemented {};
}

namespace piel { namespace lib { namespace logger_out {

class BaseLogger : public logger::LogBase {
public:

    static const std::string default_logger;
    static char const * const default_logger_filename;
    static char const * const timestamps_sfx;

    virtual void enable(const logger::log_type &type);
    virtual void disable(const logger::log_type &type);
    virtual void print(const logger::log_type& type, const std::string& var1);
    virtual void print_message(const logger::LogMessage &m);

    virtual bool is_debug_enabled();
    virtual bool is_error_enabled();
    virtual bool is_fatal_enabled();
    virtual bool is_info_enabled();
    virtual bool is_trace_enabled();
    virtual bool is_warn_enabled();

    static const char *get_log_type_str(const logger::log_type& type);

    BaseLogger(const std::string& name, const std::string& sfx);
    virtual ~BaseLogger();

    std::string str() const;

private:
    static char const * const level_sfx;
    static char const * const trace_sfx;
    static char const * const debug_sfx;
    static char const * const info_sfx;
    static char const * const warn_sfx;
    static char const * const error_sfx;
    static char const * const fatal_sfx;

    static const int fatal_lvl;
    static const int error_lvl;
    static const int warn_lvl;
    static const int info_lvl;
    static const int debug_lvl;
    static const int trace_lvl;

    static char const * const trace_m;
    static char const * const debug_m;
    static char const * const info_m;
    static char const * const warn_m;
    static char const * const error_m;
    static char const * const fatal_m;
    static char const * const null_m;

    int log_level_;

};

} } } // namespace piel::lib::logger_out
