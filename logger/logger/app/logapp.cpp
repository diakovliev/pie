#include <stdarg.h>

#include "logapp.h"
#include "../logmessage.h"
#include "../dispatcher/logdispatcher.h"

namespace piel { namespace lib { namespace logger_app {

using namespace piel::lib::logger;
using namespace piel::lib::logger_dispatcher;

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

SingleLevelLogProxy& send(SingleLevelLogProxy& val)
{
    (*val.log_) << val.manipulator_;
    return val;
}

LogApp::LogApp(const std::string& _name, LogDispatcherPtr d)
    : log_stream_("")
    , name_(_name)
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

SingleLevelLogProxy& SingleLevelLogProxy::operator<<(SingleLevelLogProxyManipulator manipulator)
{
    return manipulator(*this);
}

void LogApp::clear()
{
    log_stream_.str("");
}

SingleLevelLogProxyPtr LogApp::trace()
{
    if (!trace_)
        trace_ = SingleLevelLogProxyPtr(new SingleLevelLogProxy(this, logger_app::trace));

    return trace_;
}

SingleLevelLogProxyPtr LogApp::debug()
{
    if (!debug_)
        debug_ = SingleLevelLogProxyPtr(new SingleLevelLogProxy(this, logger_app::debug));

    return debug_;
}

SingleLevelLogProxyPtr LogApp::info()
{
    if (!info_)
        info_ = SingleLevelLogProxyPtr(new SingleLevelLogProxy(this, logger_app::info));

    return info_;
}

SingleLevelLogProxyPtr LogApp::warn()
{
    if (!warn_)
        warn_ = SingleLevelLogProxyPtr(new SingleLevelLogProxy(this, logger_app::warn));

    return warn_;
}

SingleLevelLogProxyPtr LogApp::error()
{
    if (!error_)
        error_ = SingleLevelLogProxyPtr(new SingleLevelLogProxy(this, logger_app::error));

    return error_;
}

SingleLevelLogProxyPtr LogApp::fatal()
{
    if (!fatal_)
        fatal_ = SingleLevelLogProxyPtr(new SingleLevelLogProxy(this, logger_app::fatal));

    return fatal_;
}

} } } // namespace piel::lib::logger_out
