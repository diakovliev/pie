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

        %%{
            action on_elem {
                result.push_back(UploadSpec());
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
            action on_file_name {
                result.back().file_name += fc;
            }

            vsepa       = ',';
            sepa        = ':';
            ext         = '.';
            datach      = ascii - ':' - '.' - ',';
            fdatach     = ascii - ',';
            file_name   = ( ( fdatach @on_file_name )+ );

            elem =
                (
                    ( ( datach @on_classifier )+ )? ( ext ( datach @on_ext )+ )? sepa file_name
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
