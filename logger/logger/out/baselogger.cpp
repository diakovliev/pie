#include "../logbase.h"
#include "baselogger.h"
#include "../utils.h"
#include <stdarg.h>

#define DEF_LOGGER "default_logger"

namespace piel { namespace lib { namespace logger_out {

using namespace piel::lib::logger;
using namespace piel::lib::logger_utils;

const std::string BaseLogger::default_logger          = DEF_LOGGER;
char const * const BaseLogger::default_logger_filename = DEF_LOGGER"_filename";

char const * const BaseLogger::timestamps_sfx   = "_timestamps";

char const * const BaseLogger::level_sfx = "_level";
char const * const BaseLogger::trace_sfx = "_trace";
char const * const BaseLogger::debug_sfx = "_debug";
char const * const BaseLogger::info_sfx = "_info";
char const * const BaseLogger::warn_sfx = "_warn";
char const * const BaseLogger::error_sfx = "_error";
char const * const BaseLogger::fatal_sfx = "_fatal";

const int BaseLogger::fatal_lvl = 0;
const int BaseLogger::error_lvl = 1;
const int BaseLogger::warn_lvl = 2;
const int BaseLogger::info_lvl = 3;
const int BaseLogger::debug_lvl = 4;
const int BaseLogger::trace_lvl = 5;

char const * const BaseLogger::trace_m = "T";
char const * const BaseLogger::debug_m = "D";
char const * const BaseLogger::info_m = "I";
char const * const BaseLogger::warn_m = "W";
char const * const BaseLogger::error_m = "E";
char const * const BaseLogger::fatal_m = "F";
char const * const BaseLogger::null_m  = "?";

BaseLogger::BaseLogger(const std::string &_name, const std::string &sfx)
    : LogBase(_name)
    , log_level_(0)
{
    int logLevelAll = get_env(default_logger + level_sfx, info_lvl);

    bool is_trace_enabled_all = get_env(default_logger + trace_sfx, false);
    bool is_debug_enabled_all = get_env(default_logger + debug_sfx, false);
    bool is_info_enabled_all = get_env(default_logger + info_sfx, false);
    bool is_warn_enabled_all = get_env(default_logger + warn_sfx, false);
    bool is_error_enabled_all = get_env(default_logger + error_sfx, false);
    bool is_fatal_enabled_all = get_env(default_logger + fatal_sfx, false);

    std::string const pName = default_logger + sfx;
    logLevelAll = get_env(pName + level_sfx, logLevelAll);

    is_trace_enabled_all = get_env(pName + trace_sfx, is_trace_enabled_all);
    is_debug_enabled_all = get_env(pName + debug_sfx, is_debug_enabled_all);
    is_info_enabled_all = get_env(pName + info_sfx, is_info_enabled_all);
    is_warn_enabled_all = get_env(pName + warn_sfx, is_warn_enabled_all);
    is_error_enabled_all = get_env(pName + error_sfx, is_error_enabled_all);
    is_fatal_enabled_all = get_env(pName + fatal_sfx, is_fatal_enabled_all);

    std::string const nName = normalize(name_) + sfx;
    log_level_ = get_env(nName + level_sfx, logLevelAll);

    is_trace_enabled_ = get_env(nName + trace_sfx, is_trace_enabled_all);
    is_debug_enabled_ = get_env(nName + debug_sfx, is_debug_enabled_all);
    is_info_enabled_ = get_env(nName + info_sfx, is_info_enabled_all);
    is_warn_enabled_ = get_env(nName + warn_sfx, is_warn_enabled_all);
    is_error_enabled_ = get_env(nName + error_sfx, is_error_enabled_all);
    is_fatal_enabled_ = get_env(nName + fatal_sfx, is_fatal_enabled_all);
}

BaseLogger::~BaseLogger() {}

bool BaseLogger::is_fatal_enabled() { return is_fatal_enabled_ || is_trace_enabled_ || log_level_ >= fatal_lvl; }
bool BaseLogger::is_error_enabled() { return is_error_enabled_ || is_trace_enabled_ || log_level_ >= error_lvl; }
bool BaseLogger::is_warn_enabled()  { return is_warn_enabled_ || is_trace_enabled_ || log_level_ >= warn_lvl; }
bool BaseLogger::is_info_enabled()  { return is_info_enabled_ || is_trace_enabled_ || log_level_ >= info_lvl; }
bool BaseLogger::is_debug_enabled() { return is_debug_enabled_ || is_trace_enabled_ || log_level_ >= debug_lvl; }
bool BaseLogger::is_trace_enabled() { return is_trace_enabled_ || log_level_ >= trace_lvl; }

std::string BaseLogger::str() const
{
    std::stringstream os;
    os << name_ << "(" << normalize(name_) << ") level:" << log_level_
       << " F:" << is_fatal_enabled_
       << " E:" << is_error_enabled_
       << " W:" << is_warn_enabled_
       << " I:" << is_info_enabled_
       << " D:" << is_debug_enabled_
       << " T:" << is_trace_enabled_;
    return os.str();
}

void BaseLogger::enable(const logger::log_type& type)
{
    switch (type) {
    case logger::TRACE: is_trace_enabled_ = true; break;
    case logger::DEBUG: is_debug_enabled_ = true; break;
    case logger::INFO:  is_info_enabled_  = true; break;
    case logger::WARN:  is_warn_enabled_  = true; break;
    case logger::ERROR: is_error_enabled_ = true; break;
    case logger::FATAL: is_fatal_enabled_ = true; break;
    }
}

void BaseLogger::disable(const logger::log_type& type)
{
    switch (type) {
    case logger::TRACE: is_trace_enabled_ = false; break;
    case logger::DEBUG: is_debug_enabled_ = false; break;
    case logger::INFO:  is_info_enabled_  = false; break;
    case logger::WARN:  is_warn_enabled_  = false; break;
    case logger::ERROR: is_error_enabled_ = false; break;
    case logger::FATAL: is_fatal_enabled_ = false; break;
    }
}

void BaseLogger::print(const logger::log_type& type, const std::string& var1)
{
    switch (type) {
    case logger::TRACE: trace(var1); break;
    case logger::DEBUG: debug(var1); break;
    case logger::INFO:  info(var1);  break;
    case logger::WARN:  warn(var1);  break;
    case logger::ERROR: error(var1); break;
    case logger::FATAL: fatal(var1); break;
    }
}

void BaseLogger::print_message(const logger::LogMessage& m)
{
    print(m.type, m.message);
}

const char *BaseLogger::get_log_type_str(const logger::log_type& type)
{
    switch (type) {
    case logger::TRACE: return trace_m;
    case logger::DEBUG: return debug_m;
    case logger::INFO:  return info_m;
    case logger::WARN:  return warn_m;
    case logger::ERROR: return error_m;
    case logger::FATAL: return fatal_m;
    }
    return null_m;
}


} } } // namespace piel::lib::logger_out
