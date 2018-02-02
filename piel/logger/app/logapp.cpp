#include <stdarg.h>

#include "logapp.h"
#include "../logmessage.h"
#include "../dispatcher/logdispatcher.h"

namespace piel { namespace lib { namespace logger_app {

using namespace piel::lib::logger;
using namespace piel::lib::logger_dispatcher;

const LogAppPtr& operator<< (const LogAppPtr& p, LogAppManipulator manipulator)
{
    manipulator(*p.get());
    return p;
}

void LogApp::trace(const std::string& var1)
{
    dispatcher_->enqueue(LogMessage(name_, TRACE, var1));
}

void LogApp::debug(const std::string& var1)
{
    dispatcher_->enqueue(LogMessage(name_, DEBUG, var1));
}

void LogApp::info(const std::string& var1)
{
    dispatcher_->enqueue(LogMessage(name_, INFO,  var1));
}

void LogApp::warn(const std::string& var1)
{
    dispatcher_->enqueue(LogMessage(name_, WARN,  var1));
}

void LogApp::error(const std::string& var1)
{
    dispatcher_->enqueue(LogMessage(name_, ERROR, var1));
}

void LogApp::fatal(const std::string& var1)
{
    dispatcher_->enqueue(LogMessage(name_, FATAL, var1));
}

LogApp& trace(LogApp& val)
{
    val.log_stream_ << std::endl;
    val.trace(val.log_stream_.str());
    val.clear();
    return val;
}

LogApp& debug(LogApp& val)
{
    val.log_stream_ << std::endl;
    val.debug(val.log_stream_.str());
    val.clear();
    return val;
}

LogApp& info(LogApp& val)
{
    val.log_stream_ << std::endl;
    val.info(val.log_stream_.str());
    val.clear();
    return val;
}

LogApp& warn(LogApp& val)
{
    val.log_stream_ << std::endl;
    val.warn(val.log_stream_.str());
    val.clear();
    return val;
}

LogApp& error(LogApp& val)
{
    val.log_stream_ << std::endl;
    val.error(val.log_stream_.str());
    val.clear();
    return val;
}

LogApp& fatal(LogApp& val)
{
    val.log_stream_ << std::endl;
    val.fatal(val.log_stream_.str());
    val.clear();
    return val;
}

LogApp::LogApp(const std::string& _name, LogDispatcherPtr d)
    : name_(_name)
    , log_stream_("")
{
    dispatcher_ = d;
}

LogApp::LogApp(const LogApp& l)
    : dispatcher_(l.dispatcher_)
    , name_(l.name_)
{
    log_stream_ << l.log_stream_.str();
}

LogApp::~LogApp()
{
    dispatcher_->enqueue(LogMessage(name_, REMOVE_LOG));
}

LogApp& LogApp::operator<< (LogAppManipulator manipulator)
{
    return manipulator(*this);
}

void LogApp::clear()
{
    log_stream_.str("");
}

} } } // namespace piel::lib::logger_out

