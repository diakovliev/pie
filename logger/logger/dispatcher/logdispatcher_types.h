#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace piel { namespace lib { namespace logger_dispatcher {

class LogDispatcher;

typedef boost::shared_ptr<LogDispatcher>    LogDispatcherPtr;
typedef boost::weak_ptr<LogDispatcher>      LogDispatcherWeakPtr;

} } } // namespace piel::lib::logger_dispatcher
