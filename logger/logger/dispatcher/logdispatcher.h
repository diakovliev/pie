#pragma once

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include "../singletone.h"
#include "../logmessage.h"
#include "logdispatcher_types.h"
#include "../out/loggerout_types.h"
#include "../out/logmessagequeue.h"
#include "../app/logapp_types.h"
#include <boost/signals2.hpp>

namespace piel { namespace lib { namespace logger_dispatcher {

class LogDispatcher
{
public:
    LogDispatcher();
    ~LogDispatcher();

     template<typename FuncObj>
     void connect(FuncObj obj) {
         enqueue_signal.connect(obj);
     }

     void enqueue(const logger::LogMessage& m);

protected:
    boost::signals2::signal<void (const logger::LogMessage& m)> enqueue_signal;

};


} } } // namespace piel::lib::logger_dispatcher
