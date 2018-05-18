#pragma once
#include <string>
#include <ctime>

namespace piel { namespace lib { namespace logger_utils {

std::string time_to_str(char const * fmt, std::time_t t, std::string &str);
std::string time_to_str(char const * fmt, std::time_t t);
std::string time_to_str(char const * fmt);

std::string get_env(std::string name, std::string def);
bool get_env(std::string name, bool def);
int get_env(std::string name, int def);

std::string replace(std::string str, std::string from, std::string to);
std::string normalize(std::string loggerName);
void sendEmail(std::string to, std::string from, std::string subject, std::string message);
void sendEmail(std::string subject, std::string message);

} } } // namespace piel::lib::logger_utils
