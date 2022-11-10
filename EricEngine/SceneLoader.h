#pragma once
#include <string>
#include <exception>
#include <iostream>
#include <fstream>
#include <vector>

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

    void DeleteLoadedComponents();

    template <typename ComponentType>
    void WriteComponent(ComponentType* component, std::ofstream& os);

    ECS::Component* ReadComponent(std::ifstream& in, int componentID);

    template <class ComponentType>
    ComponentType* ReadComponent(std::ifstream& in);

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
    ~SceneLoader();

    void SaveScene(std::string name);
    void LoadScene(std::string name);

    std::vector<ECS::Component*> loadedComponents;
};

template<typename ComponentType>
inline void SceneLoader::WriteComponent(ComponentType* component, std::ofstream& os)
{
    if (component == nullptr) return;

    // Write the component ID, then write the component
    os.write((char*)(&ComponentType::id), sizeof(int));
    os.write((char*)(component), sizeof(ComponentType));
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

template <class ComponentType>
ComponentType* SceneLoader::ReadComponent(std::ifstream& in)
{
    int componentSize = sizeof(ComponentType);
    ComponentType* component = new ComponentType();
    in.read((char*)(component), componentSize);
    return component;
}