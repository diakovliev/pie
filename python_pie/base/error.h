#pragma once

#include <string>
#include <exception>

////////////////////////////////////////////////////////////////////////////////
struct Error: public std::exception {
    Error(std::string what, int err = 0): what_(std::move(what)), err_(err) { }

    char const* what() throw() { return what_.c_str(); }

private:
    std::string what_;
    int err_;
};
