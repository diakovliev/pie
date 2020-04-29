#include "../app/logging.h"

#include <string>
#include "../dispatcher/logdispatcher_types.h"
#include "../dispatcher/logdispatcher.h"
#include "../app/logapp_types.h"
#include "../app/logapp.h"
#include "../out/loggerout_types.h"
#include "../out/loggerout.h"

namespace piel { namespace lib { namespace logger_app {

Logging::Logging()
{

}

Logging::~Logging()
{

}

/*static*/ logger_app::LogAppPtr Logging::create_logger(std::string name)
{
    RawPtr p = get_instance();
    logger::LogMessage m(name, logger::CREATE_LOG);

    if (!p->logger_out_thread_) {
        p->logger_out_thread_.reset(new logger_out::LoggerOut());
    }

    using namespace std::placeholders;

    logger_dispatcher::LogDispatcherPtr disp(new logger_dispatcher::LogDispatcher());
    disp->connect(std::bind(&logger_out::LoggerOut::enqueue, p->logger_out_thread_.get(), _1));
    disp->enqueue(m);

    logger_app::LogAppPtr res(new LogApp(name, disp));
    return res;
}

} } } // namespace piel::lib::logger_out

