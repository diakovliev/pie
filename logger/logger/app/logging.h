#pragma once

#include <string>

#include "logapp_types.h"
#include "logapp.h"

#include "../singletone.h"
#include "../out/loggerout_types.h"

namespace piel { namespace lib { namespace logger_app {

class Logging : public piel::lib::logger_utils::Singleton<Logging>
{
public:
    typedef Logging* RawPtr;

    Logging();
    ~Logging();
    static logger_app::LogAppPtr create_logger(std::string name);

private:
    logger_out::LoggerOutPtr         logger_out_thread_;

};

} } } // namespace piel::lib::logger_out

