#include <parsers/gavc_query_version.h>

namespace parsers::gavc {

    using namespace art::lib::gavc;

    %%{
        machine version;
        write data;
    }%%

    std::optional<VersionOps> parse_version(const std::string& input) {
        const char *p = input.c_str(), *pe = input.c_str() + input.size();
        int cs;

        VersionOps result;
        std::string buffer;

        %%{
            action on_verch {
                buffer += fc;
            }
            action on_all {
                if (!buffer.empty()) {
                    result.push_back(OpType(buffer));
                    buffer = std::string();
                }
                result.push_back(OpType(Op_all, fc));
            }
            action on_latest {
                if (!buffer.empty()) {
                    result.push_back(OpType(buffer));
                    buffer = std::string();
                }
                result.push_back(OpType(Op_latest, fc));
            }
            action on_oldest {
                if (!buffer.empty()) {
                    result.push_back(OpType(buffer));
                    buffer = std::string();
                }
                result.push_back(OpType(Op_oldest, fc));
            }

            all     = '*' @on_all;
            latest  = '+' @on_latest;
            oldest  = '-' @on_oldest;
            verch   = ( ascii - '*' - '+' - '-' );

            main :=
                ( verch @on_verch |all|latest|oldest )+;

            write init;
            write exec;
        }%%

        if (cs < version_first_final)
            return std::nullopt;

        if (!buffer.empty())
            result.push_back(OpType(Op_const, buffer));

        return result;
    }

}//namespace parsers::gavc
