#include "logbase.h"

#include <iostream>

namespace piel { namespace lib { namespace logger {

LogBase::LogBase(const std::string& n)
    : name_(n)
    , is_fatal_enabled_(false)
    , is_error_enabled_(false)
    , is_warn_enabled_(false)
    , is_info_enabled_(false)
    , is_debug_enabled_(false)
    , is_trace_enabled_(false)
{

}

LogBase::~LogBase()
{

}

} } } // namespace piel::lib::logger
