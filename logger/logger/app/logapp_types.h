#pragma once

#include <memory>

namespace piel { namespace lib { namespace logger_app {

class SingleLevelLogProxy;
class LogApp;

using LogAppPtr                 = std::shared_ptr<LogApp>;
using SingleLevelLogProxyPtr    = std::shared_ptr<SingleLevelLogProxy>;

} } } // namespace piel::lib::logger_out
