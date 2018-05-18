#include <list>
#include <string>
#include <iostream>
#include <boost/lambda/bind.hpp>
#include "loggerout.h"
#include "baselogger.h"
#include "filelogger.h"
#include "consolelogger.h"
#include "commonlogger.h"
#include "nulllogger.h"
#include "../dispatcher/logdispatcher.h"
#include "../utils.h"

namespace piel { namespace lib { namespace logger_out {

using namespace piel::lib::logger;
using namespace piel::lib::logger_utils;

const char* LoggerOut::time_format = "%Y-%m-%d %I:%M:%S";
LogPtr LoggerOut::common_logger_(new CommonLogger());

LoggerOut::LoggerOut()
    : qthread_(QueuedThread<logger::LogMessage>::start())
{
    qthread_->on_message.connect(boost::bind(&LoggerOut::on_message, this, _1));
}

LoggerOut::~LoggerOut()
{
    qthread_->enqueue(logger::LogMessage(logger::QUIT));
    qthread_->join();
}

void LoggerOut::enqueue(const logger::LogMessage& v)
{
    qthread_->enqueue(v);
}

void LoggerOut::gen_message(logger::LogMessage& m)
{
    std::stringstream os;

    if (enable_logs_timestamps_) {
        os << "[" << time_to_str(time_format) << "]";
    }
    os << "[" << m.name << "][" << BaseLogger::get_log_type_str(m.type) << "] " << m.message;

    m.message = os.str();
}

void LoggerOut::create_log(const std::string& _name)
{
    std::string nName = normalize(_name);
    {
        LogPtr log {new NullLogger(_name)};
        log_ptr_list_.push_back(log);
    }

    if (!get_env(nName + ConsoleLogger::sfx, false) ||
         get_env(BaseLogger::default_logger + ConsoleLogger::sfx, true) ) {
        LogPtr log {new ConsoleLogger(_name)};
        log_ptr_list_.push_back(log);
    }

    if (!get_env(FileLogger::default_logger_filename, std::string("")).empty()) {
        if (get_env(nName + FileLogger::sfx, false) ||
            get_env(BaseLogger::default_logger + FileLogger::sfx, false)) {
            LogPtr log {new FileLogger(_name)};
            log_ptr_list_.push_back(log);

        }
    }

    enable_logs_timestamps_ = get_env(BaseLogger::default_logger + BaseLogger::timestamps_sfx, false);
}

void LoggerOut::remove_log(const std::string& _name)
{
    log_ptr_list_.remove_if(std::bind2nd(CheckEqualLog(), _name));
    log_ptr_list_.remove_if(std::bind2nd(CheckEqualLog(), _name+"_null"));
}

void LoggerOut::print(const logger::LogMessage& m)
{
    logger::LogMessage new_m(m);
    gen_message(new_m);

    for(LogPtrList::const_iterator i=log_ptr_list_.begin(), end = log_ptr_list_.end(); i!=end; ++i) {
        if ( (*i)->get_mame() != m.name ) continue;
        (*i)->print_message(new_m);
    }
}

int LoggerOut::on_message(const logger::LogMessage& m)
{
    int retVal = 0;
    switch (m.cmd) {
    case logger::LOG:
        print(m);
        break;
    case logger::CREATE_LOG:
        create_log(m.name);
        break;
    case logger::REMOVE_LOG:
        remove_log(m.name);
        break;
    case logger::QUIT:
        retVal = -1;
        break;
    default:
        break;
    }
    return retVal;
}

logger::LogPtr LoggerOut::common_logger()
{
    return common_logger_;
}

} } } // namespace piel::lib::logger_out
