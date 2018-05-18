#pragma once

#include <string>
#include <queue>
#include <boost/weak_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/thread/thread.hpp>

#include "../queuedthread.hpp"
#include "../singletone.h"
#include "../log.h"
#include "../logmessage.h"
#include "commonlogger.h"
#include "loggerout_types.h"
#include "logmessagequeue.h"

namespace piel { namespace lib { namespace logger_out {

LogMessageQueueWeakPtr createOutputLogger();

class LoggerOut {
protected:
    struct CheckEqualLog : public std::binary_function<logger::LogPtr, std::string, bool>
    {
        bool operator()(const logger::LogPtr &log, const std::string &name) const
        {
            return log->get_mame() == name;
        }
    };

protected:
    static const char*      time_format;

    void create_log(const std::string& name);
    void remove_log(const std::string& name);
    void print(const logger::LogMessage& m);
    bool process_queue();
    void gen_message(logger::LogMessage& m);

public:
    LoggerOut();
    virtual ~LoggerOut();

    int on_message(const logger::LogMessage& m);
    void enqueue(const logger::LogMessage& v);

    static logger::LogPtr common_logger();

private:
    QueuedThread<logger::LogMessage>::ThisPtr qthread_;
    static logger::LogPtr   common_logger_;

    logger::LogPtrList      log_ptr_list_;
    ThreadPtr               thread_ptr_;
    bool                    enable_logs_timestamps_;
    LogMessageQueuePtr      queue_ptr_;

};

} } } // namespace piel::lib::logger_app
