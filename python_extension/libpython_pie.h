#pragma once

#include <boost/python.hpp>

#define LIBPYTHON_PIE_VERSION 1

////////////////////////////////////////////////////////////////////////////////
enum Errors {
    OK = 0,
    UNKNOWN_PARAM,
    QUERY_ERROR,
};

////////////////////////////////////////////////////////////////////////////////
int version();

////////////////////////////////////////////////////////////////////////////////
struct Utils {
    static bool string2bool(std::string value);
};

////////////////////////////////////////////////////////////////////////////////
struct Error: public std::exception {
    Error(std::string what, int err = 0): what_(std::move(what)), err_(err) { }

    char const* what() throw() { return what_.c_str(); }

private:
    std::string what_;
    int err_;
};