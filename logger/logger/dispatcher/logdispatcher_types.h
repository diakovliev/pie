#pragma once

#include <memory>

namespace piel { namespace lib { namespace logger_dispatcher {

class LogDispatcher;

using LogDispatcherPtr      = std::shared_ptr<LogDispatcher>;
using LogDispatcherWeakPtr  = std::weak_ptr<LogDispatcher>;

} } } // namespace piel::lib::logger_dispatcher
