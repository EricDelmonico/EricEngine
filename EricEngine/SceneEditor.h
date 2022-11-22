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

    char albedoName[32] = "black_roughness.png";
    char normalsName[32] = "black_roughness.png";
    char metalnessName[32] = "black_roughness.png";
    char roughnessName[32] = "black_roughness.png";
    char aoName[32] = "white_roughness.png";
    char pixelShaderName[32] = "PixelShader";
    char vertexShaderName[32] = "VertexShader";

    DirectX::XMFLOAT3 pos = { 0, 0, 0 };
    DirectX::XMFLOAT3 scale = { 1, 1, 1 };
    DirectX::XMFLOAT3 pitchYawRoll = { 0, 0, 0 };

    DirectX::XMFLOAT3 dir = { 0, 0, 0 };
    DirectX::XMFLOAT3 color = { 0, 0, 0 };
    float intensity;

    void SelectedEntityUI();
    void DisplayEntityComponents(int e);

    std::string WStringToString(std::wstring wstr);
    std::wstring StringToWString(std::string str);
};

