#include <parsers/gavc_query.h>

namespace parsers::gavc {

    %%{
        machine query;
        write data;
    }%%

    std::optional<GavcQuery> parse_query(const std::string& input) {
        const char *p = input.c_str(), *pe = input.c_str() + input.size();
        int cs;

        GavcQuery result;

        %%{
            action on_group {
                result.group += fc;
            }
            action on_name {
                result.name += fc;
            }
            action on_version {
                if (!result.version)
                    result.version = std::string();
                *result.version += fc;
            }
            action on_classifier {
                if (!result.classifier)
                    result.classifier = std::string();
                *result.classifier += fc;
            }
            action on_extension {
                if (!result.extension)
                    result.extension = std::string();
                *result.extension += fc;
            }

            sepa        = ':';
            extension   = '@';
            datach      = ascii - ':';
            datach1     = ascii - ':' - '@';

            main :=
                ( datach @on_group )+ sepa
                ( datach @on_name )+
                (
                    sepa ( datach @on_version )+
                    (
                        sepa ( datach1 @on_classifier )+
                        (
                            extension ( datach @on_extension )+
                        )?
                    )?
                )?;

            write init;
            write exec;

        }%%

        if (cs < query_first_final)
            return std::nullopt;

        return result;
    }

}//namespace parsers::gavc
