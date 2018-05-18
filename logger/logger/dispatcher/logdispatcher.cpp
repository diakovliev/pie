#include "logdispatcher.h"

#include <iostream>
#include "../out/loggerout.h"
#include "../app/logapp.h"

namespace piel { namespace lib { namespace logger_dispatcher {

LogDispatcher::LogDispatcher()
{

}

LogDispatcher::~LogDispatcher()
{

}

void LogDispatcher::enqueue(const logger::LogMessage& m)
{
    enqueue_signal(m);
}

} } } // namespace piel::lib::logger_dispatcher
