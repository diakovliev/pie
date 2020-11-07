#pragma once

#include <string>
#include <locale>
#include <codecvt>

namespace conversions {

    using convert_typeX = std::codecvt_utf8<wchar_t>;

    static inline std::wstring s2w(const std::string& str)
    {
        std::wstring_convert<convert_typeX, wchar_t> converterX;
        return converterX.from_bytes(str);
    }

    static inline std::string w2s(const std::wstring& wstr)
    {
        std::wstring_convert<convert_typeX, wchar_t> converterX;
        return converterX.to_bytes(wstr);
    }

}
