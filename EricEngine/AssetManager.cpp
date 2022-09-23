#include "AssetManager.h"

AssetManager::AssetManager(std::shared_ptr<D3DResources> d3dResources) : m_d3dResources(d3dResources)
{
}

Microsoft::WRL::ComPtr<ID3D11PixelShader> AssetManager::GetPixelShader(std::string name)
{
    // If we already loaded it, simply return what we have
    if (m_pixelShaders.find(name) != m_pixelShaders.end()) return m_pixelShaders[name];

    // Otherwise, we need to load the pixel shader from disk
    Microsoft::WRL::ComPtr<ID3D11PixelShader> loadedShader;

    FILE* shader;
    BYTE* bytes;

    size_t destSize = 65536;
    size_t bytesRead = 0;
    bytes = new BYTE[destSize];

    std::string filename = GetExePath() + name + ".cso";
    fopen_s(&shader, filename.c_str(), "rb");
    bytesRead = fread_s(bytes, destSize, 1, 65536, shader);

    HRESULT hr = 0;
    auto device = m_d3dResources->GetDevice();

    hr = device->CreatePixelShader(bytes, bytesRead, nullptr, &loadedShader);

    delete[] bytes;
    fclose(shader);

    m_pixelShaders.insert({ name, loadedShader });
    return loadedShader;
}

Microsoft::WRL::ComPtr<ID3D11VertexShader> AssetManager::GetVertexShader(std::string name)
{
    // If we already loaded it, simply return what we have
    if (m_vertexShaders.find(name) != m_vertexShaders.end()) return m_vertexShaders[name];

    // Otherwise, we need to load the pixel shader from disk
    Microsoft::WRL::ComPtr<ID3D11VertexShader> loadedShader;

    FILE* shader;
    BYTE* bytes;

    size_t destSize = 65536;
    size_t bytesRead = 0;
    bytes = new BYTE[destSize];

    std::string filename = GetExePath() + name + ".cso";
    fopen_s(&shader, filename.c_str(), "rb");
    bytesRead = fread_s(bytes, destSize, 1, 65536, shader);

    HRESULT hr = 0;
    auto device = m_d3dResources->GetDevice();

    hr = device->CreateVertexShader(bytes, bytesRead, nullptr, &loadedShader);

    D3D11_INPUT_ELEMENT_DESC ieDesc[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,
        0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },

        { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT,
        0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
    hr = device->CreateInputLayout(ieDesc, ARRAYSIZE(ieDesc), bytes, bytesRead, &inputLayout);
    m_inputLayouts.insert({ name, inputLayout });

    delete[] bytes;
    fclose(shader);

    m_vertexShaders.insert({ name, loadedShader });
    return loadedShader;
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