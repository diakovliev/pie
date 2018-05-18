#pragma once

#include <queue>
#include <string>
#include <sstream>
#include <ctime>

namespace piel { namespace lib { namespace logger {

class LogMessage;
typedef std::queue<LogMessage>     LogMessagesQueue;

typedef enum {
    FATAL,
    ERROR,
    WARN,
    INFO,
    DEBUG,
    TRACE
} log_type;

typedef enum {
    LOG,
    CREATE_LOG,
    REMOVE_LOG,
    QUIT
} log_cmd;

class LogMessage
{
public:
    LogMessage(const std::string& n, const logger::log_type& t, const std::string& m)
        : name(n)
        , message(m)
        , type(t)
        , cmd(LOG)
    {
        time = std::time(nullptr);
    }

    LogMessage(const LogMessage& s)
        : name(s.name)
        , message(s.message)
        , type(s.type)
        , cmd(s.cmd)
    {
        time = std::time(nullptr);
    }

    LogMessage(const std::string &n, const log_cmd& c)
        : name(n)
        , type()
        , cmd(c)
    {
        time = std::time(nullptr);
    }

    LogMessage(const log_cmd& c)
        : name()
        , message()
        , type()
        , cmd(c)
        , time()
    {
        time = std::time(nullptr);
    }

    LogMessage()
        : name()
        , message()
        , type()
        , cmd()
        , time()
    {

    }

    ~LogMessage()
    {

    }

    LogMessage& operator=(const LogMessage& rhs)
    {
        name    = rhs.name;
        message = rhs.message;
        type    = rhs.type;
        cmd     = rhs.cmd;
        time    = rhs.time;
        return *this;
    }

    std::string str() const
    {
        std::ostringstream os;

        switch (cmd)
        {
        case LOG:
            os << "[" << name << ";" << message << ";" << "LOG" << time << "]";
            break;
        case CREATE_LOG:
            os << "[" << name << ";" << "CREATE_LOG" << "]";
            break;
        case REMOVE_LOG:
            os << "[" << name << ";" << "REMOVE_LOG" << "]";
            break;
        case QUIT:
            os << "[" << name << ";" << "QUIT" << "]";
            break;
        default:
            os << "[null message]";
            break;
        }

        return os.str();
    }

    std::string         name;
    std::string         message;
    logger::log_type    type;
    log_cmd             cmd;
    std::time_t         time;
};

} } } // namespace piel::lib::logger
