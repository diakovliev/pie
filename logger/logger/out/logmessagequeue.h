#pragma once

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

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
    std::mutex                  mutex_;
    std::condition_variable     cond_;

};

} } } // namespace piel::lib::logger_app
