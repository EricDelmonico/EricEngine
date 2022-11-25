#pragma once
#include <string>
#include <codecvt>

namespace StringConversion
{
    // https://stackoverflow.com/questions/2573834/c-convert-string-or-char-to-wstring-or-wchar-t
    std::string WStringToString(std::wstring wstr);
    std::wstring StringToWString(std::string str);
}