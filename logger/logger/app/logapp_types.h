#pragma once

#include <boost/shared_ptr.hpp>

namespace piel { namespace lib { namespace logger_app {

class SingleLevelLogProxy;
class LogApp;

typedef boost::shared_ptr<LogApp>               LogAppPtr;
typedef boost::shared_ptr<SingleLevelLogProxy>  SingleLevelLogProxyPtr;

} } } // namespace piel::lib::logger_out
