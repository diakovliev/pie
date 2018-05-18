#pragma once

#include <queue>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

#include "../logmessage.h"

namespace piel { namespace lib { namespace logger_out {

class LogMessageQueue {
public:
    LogMessageQueue();
    ~LogMessageQueue();

    void push(const logger::LogMessage& m);
    void pop(logger::LogMessage& m);

private:
    logger::LogMessagesQueue    queue_;
    boost::mutex                mutex_;
    boost::condition_variable   cond_;

};

} } } // namespace piel::lib::logger_app
