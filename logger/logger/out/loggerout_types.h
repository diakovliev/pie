#pragma once

#include <queue>
#include <memory>
#include <thread>

namespace piel { namespace lib { namespace logger_out {

class LoggerOut;
class LogMessageQueue;

using LoggerOutPtr              = std::shared_ptr<LoggerOut>;
using LoggerOutWeakPtr          = std::weak_ptr<LoggerOut>;

using LogMessageQueuePtr        = std::shared_ptr<LogMessageQueue>;
using LogMessageQueueWeakPtr    = std::weak_ptr<LogMessageQueue>;

using ThreadPtr                 = std::shared_ptr<std::thread>;

} } } // namespace piel::lib::logger_app
