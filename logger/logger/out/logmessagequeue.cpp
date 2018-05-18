#include "../logmessage.h"
#include "logmessagequeue.h"

namespace piel { namespace lib { namespace logger_out {

LogMessageQueue::LogMessageQueue()
{

}

LogMessageQueue::~LogMessageQueue()
{

}

void LogMessageQueue::push(const logger::LogMessage& m)
{
    boost::unique_lock<boost::mutex> lock{mutex_};
    queue_.push(m);
    cond_.notify_all();
}

void LogMessageQueue::pop(logger::LogMessage& m)
{
    bool isEmpty;
    {
        boost::unique_lock<boost::mutex> lock{mutex_};
        isEmpty = queue_.empty();
    }
    if (isEmpty)
    {
        boost::unique_lock<boost::mutex> lock{mutex_};
        cond_.wait(lock);
    }
    {
        boost::unique_lock<boost::mutex> lock{mutex_};
        m = queue_.front();
        queue_.pop();
    }
}

} } } // namespace piel::lib::logger_dispatcher
