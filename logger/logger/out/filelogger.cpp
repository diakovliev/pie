#include "filelogger.h"
#include "../utils.h"
#include "loggerout.h"
#include <iostream>
#include <locale>
#include <ctime>

namespace piel { namespace lib { namespace logger_out {

using namespace piel::lib::logger;
using namespace piel::lib::logger_utils;

const char* FileLogger::sfx = "_file";
const char* FileLogger::time_format = "%Y-%m-%d %I-%M-%S";
std::ofstream    FileLogger::writer;

FileLogger::FileLogger(const std::string& name)
    : BaseLogger(name, sfx)
{
    init_log_file();
}

LogPtr FileLogger::commons_log()
{
    return LoggerOut::common_logger();
}

void FileLogger::init_log_file()
{
    if (!writer.is_open()) {
        std::string logFileName = get_env(default_logger_filename, std::string(""));
        if (!logFileName.empty()) {
            writer.open(logFileName.c_str());
            if (!writer.is_open()) {
               commons_log()->error(std::string("ERROR[FileLogger] Can't create log file: ") + logFileName + "\n");
            } else {
                writer << "--------------------------------------------------------------------------------\n";
                writer << " Log started at: " << time_to_str(time_format, std::time(nullptr)) << "\n";
                writer << "--------------------------------------------------------------------------------\n";
                writer << "\n";
                writer.flush();
            }
        }
    }
}

void FileLogger::print(const std::string& str) {
    if (writer.is_open()){
        writer << str;
        writer.flush();
    }
}

} } } // namespace piel::lib::logger_out
