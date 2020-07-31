#pragma once

#include <string>
#include <map>
#include <vector>

#include "libpython_pie.h"
#include "base_extension.h"

class Cache: public BaseExtension {
public:
    Cache();
    Cache(const Cache&);
    ~Cache() = default;

    int init();
    int clean(int max_age);

protected:
    virtual bool is_known_param(std::string param);

};
