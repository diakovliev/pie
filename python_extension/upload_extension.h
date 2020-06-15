#pragma once

#include <string>

#include "libpython_pie.h"
#include "base_extension.h"

class Upload: public BaseExtension {
public:
    static constexpr auto PARAM_FILELIST  = "filelist";

public:
    Upload();
    Upload(const Upload&);
    ~Upload() = default;

    int perform(std::string query_str);

protected:
    virtual bool is_known_param(std::string param);

};
