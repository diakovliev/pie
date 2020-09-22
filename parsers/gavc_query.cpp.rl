#include <parsers/gavc_query.h>

//! Versions based queries
//
//  '*' - all
//  '+' - latest
//  '-' - oldest
//
// prefix(+|-|*\.)+suffix
//  - calculation from left to right
//    (+|-|*\.)(+|-) == (+|-) (single element)
//    (+|-|*\.)* == * (set)
//
// Pairs conversion matrix:
//     -------------
//     | + | - | * |
// -----------------
// | + | + | - | + |
// -----------------
// | - | - | - | - |
// -----------------
// | * | + | - | * |
// -----------------

namespace parsers::gavc {

    using namespace art::lib::gavc;

    %%{
        machine query;
        write data;
    }%%

    std::optional<gavc_data> parse_query(const std::string& input) {
        const char *p = input.c_str(), *pe = input.c_str() + input.size();
        int cs;

        gavc_data result;

        %%{
            action on_group {
                result.group += fc;
            }
            action on_name {
                result.name += fc;
            }
            action on_version {
                result.version += fc;
            }
            action on_classifier {
                result.classifier += fc;
            }
            action on_extension {
                result.extension += fc;
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
