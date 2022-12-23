#pragma once
#include "SceneLoader.h"
#include "EntityManager.h"

#include <DirectXMath.h>
#include <string>

class SceneEditor
{
public:
    SceneEditor(SceneLoader* sceneLoader, AssetManager* assetManager);

    void Update(float dt);

private:
    SceneLoader* sceneLoader;
    AssetManager* assetManager;
    ECS::EntityManager* em;
    int selectedEntity;

    char meshName[32] = "cube.obj";

    std::string albedoName = "white_albedo.png";
    std::string normalsName = "white_albedo.png";
    std::string metalnessName = "white_albedo.png";
    std::string roughnessName = "white_albedo.png";
    std::string aoName = "white_albedo.png";
    char pixelShaderName[32] = "PixelShader";
    char vertexShaderName[32] = "VertexShader";

    DirectX::XMFLOAT3 pos = { 0, 0, 0 };
    DirectX::XMFLOAT3 scale = { 1, 1, 1 };
    DirectX::XMFLOAT3 pitchYawRoll = { 0, 0, 0 };

    DirectX::XMFLOAT3 dir = { 0, 0, 0 };
    DirectX::XMFLOAT3 color = { 0, 0, 0 };
    DirectX::XMFLOAT3 lightPos = { 0, 0, 0 };
    float range = 1.0f;
    int lightType = 0;
    float intensity;

    std::string selectedTexture = "black_roughness.png";
    std::string selectedMesh = "cube.obj";

    void SelectedEntityUI();
    void DisplayEntityComponents(int e);
    bool DisplayMeshDropdown();
    bool DisplayTextureDropdown(std::string dropdownName, std::string* dataString);
    void ReplaceMaterial(int entity, Material* newMat);
};

