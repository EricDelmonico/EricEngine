#include "SceneEditor.h"

#include "Mesh.h"
#include "Material.h"
#include "Transform.h"
#include "Light.h"
#include "DirectoryEnumeration.h"
#include "StringConversion.h"

// ImGui
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"

#include <string>
#include <codecvt>

SceneEditor::SceneEditor(SceneLoader* sceneLoader, AssetManager* assetManager)
    : sceneLoader(sceneLoader), assetManager(assetManager)
{
    em = &ECS::EntityManager::GetInstance();
    selectedEntity = 0;
}

void SceneEditor::Update(float dt)
{
    ImGui::Begin("Editor");

    std::string numEntities = "# of entities: " + std::to_string(em->entityCount);
    ImGui::Text(numEntities.c_str());

    ImGui::InputInt("Selected Entity: ", &selectedEntity);
    if (selectedEntity < 0) selectedEntity = 0;
    if (selectedEntity >= MAX_ENTITIES) selectedEntity = MAX_ENTITIES - 1;
    // If we selected an invalid entity, find a valid one
    while (!em->entities[selectedEntity]) selectedEntity = (selectedEntity + 1) % MAX_ENTITIES;
    if (ImGui::TreeNode("SelectedEntityComponents"))
    {
        SelectedEntityUI();
        ImGui::TreePop();
    }

    if (ImGui::Button("Add Entity"))
    {
        em->RegisterNewEntity();
    }

    // For every entity
    for (int i = 0; i < MAX_ENTITIES; i++)
    {
        if (!em->entities[i]) continue;
        // Display every entity
        std::string entityString = "Entity " + std::to_string(i);
        if (ImGui::TreeNode(entityString.c_str()))
        {
            // Display all the entity's components
            DisplayEntityComponents(i);
            ImGui::TreePop();
        }
    }

    ImGui::End();
}

bool SceneEditor::DisplayMeshDropdown()
{
    bool anythingSelected = false;
    auto exePath = DirectoryEnumeration::GetExePath();
    std::vector<std::string>& files = DirectoryEnumeration::GetFileList(exePath + "../../Assets/Models/");
    if (ImGui::BeginCombo("Meshes: ", "cam.obj"))
    {
        for (auto& f : files)
        {
            const bool selected = (selectedMesh == f);
            if (ImGui::Selectable(f.c_str(), &selected))
            {
                selectedMesh = f;
                anythingSelected = true;
            }

            if (selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    return anythingSelected;
}

bool SceneEditor::DisplayTextureDropdown(std::string dropdownName, std::string* dataString)
{
    bool anythingSelected = false;
    auto exePath = DirectoryEnumeration::GetExePath();
    std::vector<std::string>& files = DirectoryEnumeration::GetFileList(exePath + "../../Assets/Textures/");
    if (ImGui::BeginCombo(dropdownName.c_str(), dataString->c_str()))
    {
        for (auto& f : files)
        {
            const bool selected = (selectedMesh == f);
            if (ImGui::Selectable(f.c_str(), &selected))
            {
                *dataString = f;
                anythingSelected = true;
            }

            if (selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    return anythingSelected;
}

void SceneEditor::ReplaceMaterial(int entity, Material* newMat)
{
    em->RemoveComponent<Material>(entity);
    em->AddComponent<Material>(entity, newMat);

}

void SceneEditor::SelectedEntityUI()
{
    Mesh* mesh = nullptr;
    if (em->EntityHasComponent(Mesh::id, selectedEntity)) mesh = em->GetComponent<Mesh>(selectedEntity);

    Material* material = nullptr;
    if (em->EntityHasComponent(Material::id, selectedEntity)) material = em->GetComponent<Material>(selectedEntity);

    Transform* transform = nullptr;
    if (em->EntityHasComponent(Transform::id, selectedEntity)) transform = em->GetComponent<Transform>(selectedEntity);

    Light* light = nullptr;
    if (em->EntityHasComponent(Light::id, selectedEntity)) light = em->GetComponent<Light>(selectedEntity);

    // Display any existing components
    DisplayEntityComponents(selectedEntity);

    // For non-existing components--allow the user to add them
    if (mesh == nullptr)
    {
        if (DisplayMeshDropdown())
        {
            em->AddComponent<Mesh>(selectedEntity, assetManager->GetMesh(selectedMesh));
        }
    }

    if (material == nullptr)
    {
        DisplayTextureDropdown("Albedo", &albedoName);
        DisplayTextureDropdown("Normals", &normalsName);
        DisplayTextureDropdown("Metalness", &metalnessName);
        DisplayTextureDropdown("Roughness", &roughnessName);
        DisplayTextureDropdown("AmbientOcclusion", &aoName);
        ImGui::InputText("PixelShader Name: ", pixelShaderName, 32);
        ImGui::InputText("VertexShader Name: ", vertexShaderName, 32);

        if (ImGui::Button("Add Material"))
        {
            Material* material = new Material();

            material->albedoName = albedoName;
            material->normalsName = normalsName;
            material->metalnessName = metalnessName;
            material->roughnessName = roughnessName;
            material->aoName = aoName;
            material->pixelShaderName = pixelShaderName;
            material->vertexShaderName = vertexShaderName;

            auto albedo = assetManager->GetTexture(material->albedoName);
            auto normals = assetManager->GetTexture(material->normalsName);
            auto metalness = assetManager->GetTexture(material->metalnessName);
            auto roughness = assetManager->GetTexture(material->roughnessName);
            auto ao = assetManager->GetTexture(material->aoName);
            auto ps = assetManager->GetPixelShader(material->pixelShaderName);
            auto vs = assetManager->GetVertexShader(material->vertexShaderName);

            if (albedo == nullptr
                || normals == nullptr
                || metalness == nullptr
                || roughness == nullptr
                || ao == nullptr
                || ps == nullptr
                || vs == nullptr)
            {
                delete material;
            }
            else
            {
                material->albedo = albedo;
                material->normals = normals;
                material->metalness = metalness;
                material->roughness = roughness;
                material->ao = ao;
                material->pixelShader = ps;
                material->vertexShader = vs;
                em->AddComponent<Material>(selectedEntity, material);
            }
        }
    }

    if (transform == nullptr)
    {
        ImGui::DragFloat3("Position: ", &pos.x, 0.05f);
        ImGui::DragFloat3("Scale: ", &scale.x, 0.05f);
        ImGui::DragFloat3("PitchYawRoll: ", &pitchYawRoll.x, 3.14f / 360.0f);

        if (ImGui::Button("Add Transform"))
        {
            Transform* transform = new Transform();

            transform->SetPosition(pos.x, pos.y, pos.z);
            transform->SetScale(scale.x, scale.y, scale.z);
            transform->SetPitchYawRoll(pitchYawRoll.x, pitchYawRoll.y, pitchYawRoll.z);

            em->AddComponent<Transform>(selectedEntity, transform);
        }
    }

    if (light == nullptr)
    {
        ImGui::DragFloat3("Dir: ", &dir.x, 3.14f / 360);
        ImGui::ColorPicker3("Color: ", &color.x);
        ImGui::DragFloat("Intensity: ", &intensity, 0.05f);

        if (ImGui::Button("Add Light"))
        {
            Light* light = new Light();

            light->dir = dir;
            light->color = color;
            light->intensity = intensity;

            em->AddComponent<Light>(selectedEntity, light);
        }
    }
}

void SceneEditor::DisplayEntityComponents(int e)
{

    Mesh* mesh = nullptr;
    if (em->EntityHasComponent(Mesh::id, e)) mesh = em->GetComponent<Mesh>(e);

    Material* material = nullptr;
    if (em->EntityHasComponent(Material::id, e)) material = em->GetComponent<Material>(e);

    Transform* transform = nullptr;
    if (em->EntityHasComponent(Transform::id, e)) transform = em->GetComponent<Transform>(e);

    Light* light = nullptr;
    if (em->EntityHasComponent(Light::id, e)) light = em->GetComponent<Light>(e);

    if (mesh != nullptr)
    {
        if (ImGui::TreeNode("Mesh"))
        {
            ImGui::Text(mesh->name.c_str());
            if (DisplayMeshDropdown())
            {
                em->RemoveComponent<Mesh>(e);
                em->AddComponent<Mesh>(e, assetManager->GetMesh(selectedMesh));
            }
            if (ImGui::Button("Remove Mesh"))
            {
                em->RemoveComponent<Mesh>(e);
            }
            ImGui::TreePop();
        }
    }
    if (material != nullptr)
    {
        if (ImGui::TreeNode("Material"))
        {
            bool changedTexture = false;
            changedTexture |= DisplayTextureDropdown("Albedo", &material->albedoName);
            changedTexture |= DisplayTextureDropdown("Normals", &material->normalsName);
            changedTexture |= DisplayTextureDropdown("Metalness", &material->metalnessName);
            changedTexture |= DisplayTextureDropdown("Roughness", &material->roughnessName);
            changedTexture |= DisplayTextureDropdown("AmbientOcclusion", &material->aoName);
            if (changedTexture)
            {
                Material* mat = new Material();

                mat->albedoName = material->albedoName;
                mat->normalsName = material->normalsName;
                mat->metalnessName = material->metalnessName;
                mat->roughnessName = material->roughnessName;
                mat->aoName = material->aoName;
                mat->pixelShaderName = material->pixelShaderName;
                mat->vertexShaderName = material->vertexShaderName;

                mat->albedo = assetManager->GetTexture(mat->albedoName);
                mat->normals = assetManager->GetTexture(mat->normalsName);
                mat->metalness = assetManager->GetTexture(mat->metalnessName);
                mat->roughness = assetManager->GetTexture(mat->roughnessName);
                mat->ao = assetManager->GetTexture(mat->aoName);
                mat->pixelShader = assetManager->GetPixelShader(mat->pixelShaderName);
                mat->vertexShader = assetManager->GetVertexShader(mat->vertexShaderName);

                ReplaceMaterial(e, mat);
                ImGui::TreePop();
                return;
            }
            ImGui::Text(("PixelShader: " + material->pixelShaderName).c_str());
            ImGui::Text(("VertexShader: " + material->vertexShaderName).c_str());
            if (ImGui::Button("Remove Material"))
            {
                em->RemoveComponent<Material>(e);
            }
            ImGui::TreePop();
        }
    }
    if (transform != nullptr)
    {
        if (ImGui::TreeNode("Transform"))
        {
            auto position = transform->GetPosition();
            if (ImGui::DragFloat3("Position: ", &position.x, 0.05f))
            {
                transform->SetPosition(position.x, position.y, position.z);
            }

            auto scale = transform->GetScale();
            if (ImGui::DragFloat3("Scale: ", &scale.x, 0.05f))
            {
                transform->SetScale(scale.x, scale.y, scale.z);
            }

            auto pitchYawRoll = transform->GetPitchYawRoll();
            if (ImGui::DragFloat3("PitchYawRoll: ", &pitchYawRoll.x, 3.14f / 360.0f))
            {
                transform->SetPitchYawRoll(pitchYawRoll.x, pitchYawRoll.y, pitchYawRoll.z);
            }

            if (ImGui::Button("Remove Transform"))
            {
                em->RemoveComponent<Transform>(e);
            }
            ImGui::TreePop();
        }
    }
    if (light != nullptr)
    {
        if (ImGui::TreeNode("Light"))
        {
            auto dir = light->dir;
            if (ImGui::DragFloat3("Dir: ", &dir.x, 3.14f / 360))
            {
                light->dir = dir;
            }

            auto color = light->color;
            if (ImGui::ColorPicker3("Color: ", &color.x))
            {
                light->color = color;
            }

            auto intensity = light->intensity;
            if (ImGui::DragFloat("Intensity: ", &intensity, 0.05f))
            {
                light->intensity = intensity;
            }

            if (ImGui::Button("Remove Light"))
            {
                em->RemoveComponent<Light>(e);
            }
            ImGui::TreePop();
        }
    }
}
