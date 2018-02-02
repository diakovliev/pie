#pragma once

#include <string>
#include <sstream>
#include <boost/shared_ptr.hpp>
#include "logapp_types.h"
#include "../dispatcher/logdispatcher_types.h"

namespace piel { namespace lib { namespace logger_app {

class LogApp;

typedef LogApp& (*LogAppManipulator)(LogApp&);

// Manipulators
LogApp& trace(LogApp& val);
LogApp& debug(LogApp& val);
LogApp& info(LogApp& val);
LogApp& warn(LogApp& val);
LogApp& error(LogApp& val);
LogApp& fatal(LogApp& val);

class LogApp
{
public:
    void trace(const std::string& msg);
    void debug(const std::string& msg);
    void info(const std::string& msg);
    void warn(const std::string& msg);
    void error(const std::string& msg);
    void fatal(const std::string& msg);

    LogApp(const LogApp& l);
    LogApp(const std::string& _name, logger_dispatcher::LogDispatcherPtr d);
    ~LogApp();

    template<typename T>
    LogApp& operator<<(T val)
    {
        log_stream_ << val;
        return *this;
    }

    LogApp& operator<< (LogAppManipulator manipulator);

protected:
    void clear();

    friend LogApp& trace(LogApp& val);
    friend LogApp& debug(LogApp& val);
    friend LogApp& info(LogApp& val);
    friend LogApp& warn(LogApp& val);
    friend LogApp& error(LogApp& val);
    friend LogApp& fatal(LogApp& val);

    std::stringstream                   log_stream_;
    logger_dispatcher::LogDispatcherPtr dispatcher_;
    std::string                         name_;

};

template<typename T>
const LogAppPtr& operator<<(const LogAppPtr& p, T val)
{
    p->operator <<(val);
    return p;
}

const LogAppPtr& operator<< (const LogAppPtr& p, LogAppManipulator manipulator);

} } } // namespace piel::lib::logger_out
