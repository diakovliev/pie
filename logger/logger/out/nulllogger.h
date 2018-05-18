#pragma once

#include "../logbase.h"

namespace piel { namespace lib { namespace logger_out {

class NullLogger : public logger::LogBase
{
public:
    NullLogger(const std::string& name);
    virtual ~NullLogger();

protected:
    virtual void print(const std::string& str);

};

} } } // namespace piel::lib::logger_out
