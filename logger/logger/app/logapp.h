#pragma once

#include <string>
#include <sstream>
#include <boost/shared_ptr.hpp>
#include "logapp_types.h"
#include "../dispatcher/logdispatcher_types.h"

namespace piel { namespace lib { namespace logger_app {

class LogApp;

typedef LogApp& (*LogAppManipulator)(LogApp&);
typedef SingleLevelLogProxy& (*SingleLevelLogProxyManipulator)(SingleLevelLogProxy&);

LogApp& trace(LogApp& val);
LogApp& debug(LogApp& val);
LogApp& info(LogApp& val);
LogApp& warn(LogApp& val);
LogApp& error(LogApp& val);
LogApp& fatal(LogApp& val);
SingleLevelLogProxy& send(SingleLevelLogProxy& val);

class SingleLevelLogProxy
{
public:
    SingleLevelLogProxy(LogApp* log, const LogAppManipulator& manipulator)
        : log_(log)
        , manipulator_(manipulator)
    {
    }

    template<typename T>
    SingleLevelLogProxy& operator<<(T val)
    {
        (*log_) << val;
        return *this;
    }

    SingleLevelLogProxy& operator<<(SingleLevelLogProxyManipulator manipulator);

    bool is_fatal() const {
        return manipulator_ == logger_app::fatal;
    }

private:
    friend SingleLevelLogProxy& send(SingleLevelLogProxy& val);

    LogApp *log_;
    LogAppManipulator manipulator_;
};

class LogApp
{
public:
    SingleLevelLogProxyPtr trace();
    SingleLevelLogProxyPtr debug();
    SingleLevelLogProxyPtr info();
    SingleLevelLogProxyPtr warn();
    SingleLevelLogProxyPtr error();
    SingleLevelLogProxyPtr fatal();

    LogApp(const LogApp& l);
    LogApp(const std::string& _name, logger_dispatcher::LogDispatcherPtr d);
    ~LogApp();

    template<typename T>
    LogApp& operator<<(T val)
    {
        log_stream_ << val;
        return *this;
    }

    LogApp& operator<<(LogAppManipulator manipulator);

protected:
    void clear();

    void trace(const std::string& msg);
    void debug(const std::string& msg);
    void info(const std::string& msg);
    void warn(const std::string& msg);
    void error(const std::string& msg);
    void fatal(const std::string& msg);

    friend LogApp& trace(LogApp& val);
    friend LogApp& debug(LogApp& val);
    friend LogApp& info(LogApp& val);
    friend LogApp& warn(LogApp& val);
    friend LogApp& error(LogApp& val);
    friend LogApp& fatal(LogApp& val);

    std::stringstream                   log_stream_;
    logger_dispatcher::LogDispatcherPtr dispatcher_;
    std::string                         name_;

    SingleLevelLogProxyPtr              trace_;
    SingleLevelLogProxyPtr              debug_;
    SingleLevelLogProxyPtr              info_;
    SingleLevelLogProxyPtr              warn_;
    SingleLevelLogProxyPtr              error_;
    SingleLevelLogProxyPtr              fatal_;

};

namespace errors {

    // LogApp::fatal will always throw fatal_occurred finally
    class fatal_occurred: public std::exception
    {
    public:
        fatal_occurred(const std::string& what)
            : std::exception()
            , what_(what)
        {}

        virtual ~fatal_occurred() throw () {}

        virtual const char* what() const throw ()
        {
            return what_.c_str();
        }

    private:
        std::string what_;

    };

}

template<class LogPtr, class T>
struct inserter {
    static void insert(const LogPtr& p, T val) {
        p->operator <<(val);
    }
};

template<>
struct inserter<LogAppPtr,LogAppManipulator> {
    static void insert(const LogAppPtr& p, LogAppManipulator val) {
        p->operator <<(val);
        if (val == logger_app::fatal) {
            // Throw fatal exception
            throw errors::fatal_occurred("Logged fatal");
        }
    }
};

template<>
struct inserter<SingleLevelLogProxyPtr, SingleLevelLogProxyManipulator> {
    static void insert(const SingleLevelLogProxyPtr& p, SingleLevelLogProxyManipulator val) {
        p->operator <<(val);
        if (val == logger_app::send && p->is_fatal()) {
            // Throw fatal exception
            throw errors::fatal_occurred("Single level proxy logged fatal");
        }
    }
};

template<typename T>
const LogAppPtr& operator<<(const LogAppPtr& p, T val)
{
    inserter<LogAppPtr,T>::insert(p, val);
    return p;
}

template<typename T>
const SingleLevelLogProxyPtr& operator<<(const SingleLevelLogProxyPtr& p, T val)
{
    inserter<SingleLevelLogProxyPtr,T>::insert(p, val);
    return p;
}

} } } // namespace piel::lib::logger_out
