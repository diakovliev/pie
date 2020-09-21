#include <parsers/gavc_query_versions_range.h>

namespace parsers::gavc {

    %%{
        machine range;
        write data;
    }%%

    std::optional<Range> parse_range(const std::string& input) {

        const char *p = input.c_str(), *pe = input.c_str() + input.size();
        int cs;

        Range result;
        result.flags = 0;

        %%{

            action on_include_left {
                result.flags |= RangeFlags_include_left;
            }
            action on_include_right {
                result.flags |= RangeFlags_include_right;
            }
            action on_left {
                result.left += fc;
            }
            action on_right {
                result.right += fc;
            }

            include_left = '[' @on_include_left;
            exclude_left = '(';

            include_right = ']' @on_include_right;
            exclude_right = ')';

            verch = (ascii - ',' - '[' - ']' - '(' - ')' );

            main :=
                ( include_left | exclude_left ) ( verch @on_left )+ ',' ( verch @on_right )+ ( include_right | exclude_right );

            write init;
            write exec;

        }%%

        if (cs < range_first_final)
            return std::nullopt;

        return result;
    }

}//namespace parsers::gavc
