#pragma once

#include <optional>
#include <vector>
#include <string>

namespace parsers::upload {

    struct UploadSpec {
        std::string file_name;
        std::optional<std::string> classifier;
        std::optional<std::string> extension;
    };

    using UFSVector = std::vector<UploadSpec>;

    std::optional<UFSVector> parse_upload_spec(const std::string& input);

}//namespace parsers::upload
