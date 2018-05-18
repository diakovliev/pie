#pragma once

#include <queue>
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>

namespace piel { namespace lib { namespace logger_out {

class LoggerOut;
class LogMessageQueue;

typedef boost::shared_ptr<LoggerOut>        LoggerOutPtr;
typedef boost::weak_ptr<LoggerOut>          LoggerOutWeakPtr;

typedef boost::shared_ptr<LogMessageQueue>  LogMessageQueuePtr;
typedef boost::weak_ptr<LogMessageQueue>    LogMessageQueueWeakPtr;

typedef boost::shared_ptr<boost::thread>    ThreadPtr;

} } } // namespace piel::lib::logger_app
