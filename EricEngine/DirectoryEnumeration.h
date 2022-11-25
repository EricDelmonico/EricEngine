#pragma once
#include <vector>
#include <string>

namespace DirectoryEnumeration
{
    std::wstring GetExePathLong();
    std::string GetExePath();
    std::vector<std::string>& GetFileList(const std::string& path);
}