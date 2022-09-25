#include "AssetManager.h"
#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

AssetManager::AssetManager(std::shared_ptr<D3DResources> d3dResources) : m_d3dResources(d3dResources)
{
}

std::shared_ptr<SimplePixelShader> AssetManager::GetPixelShader(std::wstring name)
{
    if (m_pixelShaders.find(name) != m_pixelShaders.end()) return m_pixelShaders[name];

    auto device = m_d3dResources->GetDeviceComPtr();
    auto context = m_d3dResources->GetContextComPtr();
    m_pixelShaders.insert({ name, std::make_shared<SimplePixelShader>(device, context, (AssetManager::GetExePathLong() + name + L".cso").c_str())});
    return m_pixelShaders[name];
}

std::shared_ptr<SimpleVertexShader> AssetManager::GetVertexShader(std::wstring name)
{
    if (m_vertexShaders.find(name) != m_vertexShaders.end()) return m_vertexShaders[name];

    auto device = m_d3dResources->GetDeviceComPtr();
    auto context = m_d3dResources->GetContextComPtr();
    m_vertexShaders.insert({ name, std::make_shared<SimpleVertexShader>(device, context, (AssetManager::GetExePathLong() + name + L".cso").c_str()) });
    return m_vertexShaders[name];
}

std::wstring AssetManager::GetExePathLong()
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

std::string AssetManager::GetExePath()
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

void AssetManager::ImportAsset()
{
    // Create an instance of the importer class
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(AssetManager::GetExePath() + "../../Assets/Models/cube.obj",
        aiProcess_CalcTangentSpace      |
        aiProcess_Triangulate           |
        aiProcess_JoinIdenticalVertices |
        aiProcess_SortByPType);
}