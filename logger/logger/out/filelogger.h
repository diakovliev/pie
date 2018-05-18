#pragma once

#include "../logbase.h"
#include "baselogger.h"
#include <fstream>

namespace piel { namespace lib { namespace logger_out {

class FileLogger : public BaseLogger
{
private:
    static const char*      time_format;
    static std::ofstream    writer;

    static logger::LogPtr commons_log();
    static void init_log_file();

protected:
    virtual void print(const std::string& str);

public:
    static const char* sfx;

    FileLogger(const std::string& name);
    virtual ~FileLogger() {}
};

} } } // namespace piel::lib::logger_out
