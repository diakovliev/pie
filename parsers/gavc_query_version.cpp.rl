#include <parsers/gavc_query_version.h>

namespace parsers::gavc {

    using namespace art::lib::gavc;

    %%{
        machine version;
        write data;
    }%%

    std::optional<
        std::pair<
            std::optional<VersionOps>,
            std::optional<gavc_versions_range_data>
        >
    > parse_version(const std::string& input) {
        const char *p = input.c_str(), *pe = input.c_str() + input.size();
        int cs;

        std::string buffer;

        VersionOps ops;
        gavc_versions_range_data range;
        range.flags = RangeFlags_exclude_all;

        bool has_range = false;
        bool has_error = false;

        %%{
            action on_verch {
                buffer += fc;
            }
            action on_all {
                if (!buffer.empty()) {
                    ops.push_back(OpType(buffer));
                    buffer = std::string();
                }

                if (!ops.empty() && !ops.back().is_const()) {
                    has_error = true;
                    fbreak;
                }

                ops.push_back(OpType(Op_all, fc));
            }
            action on_latest {
                if (!buffer.empty()) {
                    ops.push_back(OpType(buffer));
                    buffer = std::string();
                }

                if (!ops.empty() && !ops.back().is_const()) {
                    has_error = true;
                    fbreak;
                }

                ops.push_back(OpType(Op_latest, fc));
            }
            action on_oldest {
                if (!buffer.empty()) {
                    ops.push_back(OpType(buffer));
                    buffer = std::string();
                }

                if (!ops.empty() && !ops.back().is_const()) {
                    has_error = true;
                    fbreak;
                }

                ops.push_back(OpType(Op_oldest, fc));
            }

            action enter_range {
                has_range = true;
            }
            action on_include_left {
                range.flags |= RangeFlags_include_left;
            }
            action on_include_right {
                range.flags |= RangeFlags_include_right;
            }
            action on_left {
                range.left += fc;
            }
            action on_right {
                range.right += fc;
            }

            all     = '*' @on_all;
            latest  = '+' @on_latest;
            oldest  = '-' @on_oldest;
            verch   = ( ascii - '*' - '+' - '-' - ',' - '[' - ']' - '(' - ')' );

            version =
                ( ( verch @on_verch |all|latest|oldest )+ );

            include_left = '[' @on_include_left;
            exclude_left = '(';

            include_right = ']' @on_include_right;
            exclude_right = ')';

            rverch = (ascii - ',' - '[' - ']' - '(' - ')' );

            range =
                ( ( include_left | exclude_left ) ( rverch @on_left )+ ',' ( rverch @on_right )+ ( include_right | exclude_right ) ) >enter_range;

            main := version range?;

            write init;
            write exec;
        }%%

        if (has_error || cs < version_first_final)
            return std::nullopt;

        if (!buffer.empty())
            ops.push_back(OpType(Op_const, buffer));

        if (!has_range)
            return std::make_pair(ops, std::nullopt);

        return std::make_pair(ops, range);
    }

}//namespace parsers::gavc
