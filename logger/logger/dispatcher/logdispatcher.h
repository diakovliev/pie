#pragma once

#include <string>
#include <memory>
#include <functional>
#include "../singletone.h"
#include "../logmessage.h"
#include "logdispatcher_types.h"
#include "../out/loggerout_types.h"
#include "../out/logmessagequeue.h"
#include "../app/logapp_types.h"

namespace piel { namespace lib { namespace logger_dispatcher {

class LogDispatcher
{
public:
    LogDispatcher();
    ~LogDispatcher();

     template<typename FuncObj>
     void connect(FuncObj obj) {
         enqueue_signal = obj;
     }

     void enqueue(const logger::LogMessage& m);

protected:
     std::function<void (const logger::LogMessage& m)> enqueue_signal;

};


} } } // namespace piel::lib::logger_dispatcher
