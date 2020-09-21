#include <parsers/uploadfilesspec.h>

namespace parsers::upload {

    %%{
        machine ufs;
        write data;
    }%%

    std::optional<UFSVector> parse_upload_spec(const std::string& input) {
        const char *p = input.c_str(), *pe = input.c_str() + input.size();
        int cs;

        UFSVector result;
        bool has_classifier = false;

        %%{
            action on_elem {
                result.push_back(UploadSpec());
            }
            action on_sepa {
                has_classifier = true;
            }
            action on_vsepa {
                if (!has_classifier) {
                    result.back().classifier = std::nullopt;
                }
                has_classifier = false;
            }
            action on_classifier {
                if (!result.back().classifier) {
                    result.back().classifier = std::string();
                }
                *result.back().classifier += fc;
            }
            action on_ext {
                if (!result.back().extension) {
                    result.back().extension = std::string();
                }
                *result.back().extension += fc;
            }
            action on_file_name_enter {
                result.back().file_name = std::string();
            }
            action on_file_name {
                result.back().file_name += fc;
            }

            vsepa       = ',' @on_vsepa;
            sepa        = ':' @on_sepa;
            ext         = '.';
            datach      = ascii - ':' - '.' - ',';
            file_name   = ( ( datach @on_file_name )+ ) >on_file_name_enter;

            elem    =
                (
                    ( ( datach @on_classifier )+ sepa )? file_name
                    ( ext ( datach @on_ext )+ )?
                ) >on_elem;

            main :=
                elem ( vsepa elem )*;

            write init;
            write exec;

        }%%

        if (cs < ufs_first_final)
           return std::nullopt;

        return result;
    }

}//namespace parsers::upload
