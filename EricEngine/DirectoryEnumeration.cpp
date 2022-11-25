#include "DirectoryEnumeration.h"
#include <Windows.h>
#include "boost/filesystem.hpp"

namespace DirectoryEnumeration
{
    std::wstring GetExePathLong()
    {
        // Assume the path is just the "current directory" for now
        std::wstring path = L".\\";

        // Get the real, full path to this executable
        wchar_t currentDir[1024] = {};
        GetModuleFileName(0, currentDir, 1024);

        // Find the location of the last slash charaacter
        wchar_t* lastSlash = wcsrchr(currentDir, '\\');
        if (lastSlash)
        {
            // End the string at the last slash character, essentially
            // chopping off the exe's file name.  Remember, c-strings
            // are null-terminated, so putting a "zero" character in 
            // there simply denotes the end of the string.
            *lastSlash = 0;

            // Set the remainder as the path
            path = currentDir;
            path += L"\\";
        }

        // Toss back whatever we've found
        return path;
    }

    std::string GetExePath()
    {
        // Assume the path is just the "current directory" for now
        std::string path = ".\\";

        // Get the real, full path to this executable
        char currentDir[1024] = {};
        GetModuleFileNameA(0, currentDir, 1024);

        // Find the location of the last slash charaacter
        char* lastSlash = strrchr(currentDir, '\\');
        if (lastSlash)
        {
            // End the string at the last slash character, essentially
            // chopping off the exe's file name.  Remember, c-strings
            // are null-terminated, so putting a "zero" character in 
            // there simply denotes the end of the string.
            *lastSlash = 0;

            // Set the remainder as the path
            path = currentDir;
            path += "\\";
        }

        // Toss back whatever we've found
        return path;
    }

    std::vector<std::string> fileList;

    // Modified from a solution on https://gist.github.com/vivithemage/9517678
    std::vector<std::string>& GetFileList(const std::string& path)
    {
        fileList.clear();
        if (!path.empty())
        {
            namespace fs = boost::filesystem;

            fs::path apk_path(path);
            fs::recursive_directory_iterator end;

            for (fs::recursive_directory_iterator i(apk_path); i != end; ++i)
            {
                const fs::path cp = (*i);
                fileList.push_back(cp.filename().string());
            }
        }
        return fileList;
    }
}