#pragma once
#include <string>
#include <exception>
#include <iostream>
#include <fstream>
#include <vector>
#include <memory>

#include "Mesh.h"
#include "Transform.h"
#include "Material.h"
#include "Camera.h"
#include "Light.h"

#include "EntityManager.h"
#include "AssetManager.h"

class SceneLoader
{
private:
    ECS::EntityManager* em;
    AssetManager* am;

    template <typename ComponentType>
    void WriteComponent(ComponentType* component, std::ofstream& os);

    ECS::Component* ReadComponent(std::ifstream& in, int componentID);

    // WString methods adapted from
    // https://stackoverflow.com/questions/23399931/c-reading-string-from-binary-file-using-fstream
    void WriteWString(std::wstring text, std::ofstream& os)
    {
        size_t len = text.length();
        os.write((char*)(&len), sizeof(size_t));
        std::wstring textCopy = text;
        os.write((char*)(textCopy.c_str()), len * sizeof(wchar_t));
    }
    std::wstring ReadWString(std::ifstream& in)
    {
        std::wstring returnString;
        size_t len;
        in.read((char*)(&len), sizeof(size_t));
        wchar_t* temp = new wchar_t[len + 1];
        in.read((char*)(temp), len * sizeof(wchar_t));
        temp[len] = L'\0';
        returnString = temp;
        delete[] temp;
        return returnString;
    }
    std::string ReadString(std::ifstream& in)
    {
        std::string returnString;
        size_t len;
        in.read((char*)(&len), sizeof(size_t));
        char* temp = new char[len + 1];
        in.read(temp, len);
        temp[len] = L'\0';
        returnString = temp;
        delete[] temp;
        return returnString;
    }

    std::string GetExePath();

public:
    SceneLoader(AssetManager* am);

    void SaveScene(std::string name);
    void LoadScene(std::string name);
};

template<typename ComponentType>
inline void SceneLoader::WriteComponent(ComponentType* component, std::ofstream& os)
{
    if (component == nullptr) return;

    // Write the component ID, then write the component
    os.write((char*)(&ComponentType::id), sizeof(int));
    os.write((char*)(component), sizeof(ComponentType));
}

template <>
inline void SceneLoader::WriteComponent<Camera>(Camera* camera, std::ofstream& os)
{
    if (camera == nullptr) return;

    auto position = camera->GetTransform()->GetPosition();
    auto pitchYawRoll = camera->GetTransform()->GetPitchYawRoll();
    auto moveSpeed = camera->GetMoveSpeed();
    auto lookSpeed = camera->GetLookSpeed();
    auto fov = camera->GetFoV();
    auto aspectRatio = camera->GetAspectRatio();
    auto perspective = camera->IsPerspective();
    auto orthoSize = camera->GetOrthoSize();

    os.write((char*)(&Camera::id), sizeof(int));
    os.write((char*)(&position.x), sizeof(float));
    os.write((char*)(&position.y), sizeof(float));
    os.write((char*)(&position.z), sizeof(float));
    os.write((char*)(&moveSpeed), sizeof(float));
    os.write((char*)(&lookSpeed), sizeof(float));
    os.write((char*)(&fov), sizeof(float));
    os.write((char*)(&aspectRatio), sizeof(float));
    os.write((char*)(&orthoSize), sizeof(float));
    os.write((char*)(&pitchYawRoll.x), sizeof(float));
    os.write((char*)(&pitchYawRoll.y), sizeof(float));
    os.write((char*)(&pitchYawRoll.z), sizeof(float));
    os.write((char*)(&perspective), sizeof(bool));
}

template<>
inline void SceneLoader::WriteComponent<Mesh>(Mesh* mesh, std::ofstream& os)
{
    if (mesh == nullptr) return;

    // Write the component ID, then write the component
    os.write((char*)(&Mesh::id), sizeof(int));
    size_t nameLen = mesh->name.length();
    os.write((char*)(&nameLen), sizeof(size_t));
    os.write(mesh->name.c_str(), nameLen);
}

template<>
inline void SceneLoader::WriteComponent<Material>(Material* material, std::ofstream& os)
{
    if (material == nullptr) return;

    // Write the component ID, then write the component
    os.write((char*)(&Material::id), sizeof(int));
    WriteWString(material->albedoName, os);
    WriteWString(material->normalsName, os);
    WriteWString(material->metalnessName, os);
    WriteWString(material->roughnessName, os);
    WriteWString(material->aoName, os);
    WriteWString(material->pixelShaderName, os);
    WriteWString(material->vertexShaderName, os);
}

template <>
inline void SceneLoader::WriteComponent<Light>(Light* light, std::ofstream& os)
{
    if (light == nullptr) return;

    os.write((char*)(&Light::id), sizeof(int));
    os.write((char*)(&light->dir), sizeof(DirectX::XMFLOAT3));
    os.write((char*)(&light->color), sizeof(DirectX::XMFLOAT3));
    os.write((char*)(&light->intensity), sizeof(float));
}

template <>
inline void SceneLoader::WriteComponent<Transform>(Transform* transform, std::ofstream& os)
{
    if (transform == nullptr) return;

    os.write((char*)(&Transform::id), sizeof(int));
    os.write((char*)(&transform->position), sizeof(DirectX::XMFLOAT3));
    os.write((char*)(&transform->pitchYawRoll), sizeof(DirectX::XMFLOAT3));
    os.write((char*)(&transform->scale), sizeof(DirectX::XMFLOAT3));
    os.write((char*)(&transform->worldMatrix), sizeof(DirectX::XMFLOAT4X4));
    os.write((char*)(&transform->worldInverseTransposeMatrix), sizeof(DirectX::XMFLOAT4X4));
    os.write((char*)(&transform->matricesDirty), sizeof(bool));
}