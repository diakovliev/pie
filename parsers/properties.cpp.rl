#include <parsers/properties.h>

namespace parsers::properties {

    %%{
        machine property;
        write data;
    }%%

    std::optional<std::pair<std::string, std::string> > parse_property(const std::string& input) {
        const char *p = input.c_str(), *pe = input.c_str() + input.size();
        int cs;

        std::pair<std::string, std::string> result;
        bool is_comment = false;

        %%{
            action on_first {
                result.first += fc;
            }
            action on_second {
                result.second += fc;
            }
            action on_comment {
                is_comment = true;
            }

            commentch   = '#';
            assign      = '=';
            identch     = ascii - space - assign - commentch;

            comment     = ( space* commentch ascii*) @on_comment;
            value       = space* ( identch @on_first )+ space* assign ( ascii @on_second )*;

            main        := value | comment;

            write init;
            write exec;

        }%%

        if (cs < property_first_final)
            return std::nullopt;

        if (is_comment)
            return std::nullopt;

        return result;
    }

}//namespace parsers::properties
