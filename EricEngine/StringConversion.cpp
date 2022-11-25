#include "StringConversion.h"

namespace StringConversion
{
    // https://stackoverflow.com/questions/2573834/c-convert-string-or-char-to-wstring-or-wchar-t
    std::string WStringToString(std::wstring wstr)
    {
        //setup converter
        std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
        return myconv.to_bytes(wstr);
    }

    std::wstring StringToWString(std::string str)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
        return myconv.from_bytes(str);
    }
}