#include "SceneEditor.h"

#include "Mesh.h"
#include "Material.h"
#include "Transform.h"
#include "Light.h"

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
    if (ImGui::CollapsingHeader("SelectedEntityComponents"))
    {
        if (selectedEntity < 0) selectedEntity = 0;
        if (selectedEntity >= MAX_ENTITIES) selectedEntity = MAX_ENTITIES - 1;
        // If we selected an invalid entity, find a valid one
        while (!em->entities[selectedEntity]) selectedEntity = (selectedEntity + 1) % MAX_ENTITIES;
        SelectedEntityUI();
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
        if (ImGui::CollapsingHeader(std::to_string(i).c_str()))
        {
            // Display all the entity's components
            DisplayEntityComponents(i);
        }
    }

    ImGui::End();
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
        ImGui::InputText("Mesh file: ", meshName, 32);
        if (ImGui::Button("Add Mesh"))
        {
            std::string meshNameStr = meshName;
            mesh = assetManager->GetMesh(meshNameStr);
            if (mesh != nullptr)
            {
                em->AddComponent<Mesh>(selectedEntity, mesh);
            }
        }
    }

    if (material == nullptr)
    {
        ImGui::InputText("Albedo Texture: ", albedoName, 32);
        ImGui::InputText("Normals Texture: ", normalsName, 32);
        ImGui::InputText("Metalness Texture: ", metalnessName, 32);
        ImGui::InputText("Roughness Texture: ", roughnessName, 32);
        ImGui::InputText("AmbientOcclusion Texture: ", aoName, 32);
        ImGui::InputText("PixelShader Name: ", pixelShaderName, 32);
        ImGui::InputText("VertexShader Name: ", vertexShaderName, 32);

        if (ImGui::Button("Add Material"))
        {
            Material* material = new Material();

            material->albedoName = StringToWString(albedoName);
            material->normalsName = StringToWString(normalsName);
            material->metalnessName = StringToWString(metalnessName);
            material->roughnessName = StringToWString(roughnessName);
            material->aoName = StringToWString(aoName);
            material->pixelShaderName = StringToWString(pixelShaderName);
            material->vertexShaderName = StringToWString(vertexShaderName);

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
                material->pixelShader = ps.get();
                material->vertexShader = vs.get();
                em->AddComponent<Material>(selectedEntity, material);
            }
        }
    }

    if (transform == nullptr)
    {
        ImGui::DragFloat3("Position: ", &pos.x);
        ImGui::DragFloat3("Scale: ", &scale.x);
        ImGui::DragFloat3("PitchYawRoll: ", &pitchYawRoll.x);

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
        ImGui::DragFloat3("Dir: ", &dir.x);
        ImGui::ColorPicker3("Color: ", &color.x);
        ImGui::DragFloat("Intensity: ", &intensity);

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
        if (ImGui::CollapsingHeader("Mesh"))
        {
            ImGui::Text(mesh->name.c_str());
        }
    }
    if (material != nullptr)
    {
        if (ImGui::CollapsingHeader("Material"))
        {
            ImGui::Text(WStringToString(L"Albedo: " + material->albedoName).c_str());
            ImGui::Text(WStringToString(L"Normals: " + material->normalsName).c_str());
            ImGui::Text(WStringToString(L"Metalness: " + material->metalnessName).c_str());
            ImGui::Text(WStringToString(L"Roughness: " + material->roughnessName).c_str());
            ImGui::Text(WStringToString(L"AO: " + material->aoName).c_str());
            ImGui::Text(WStringToString(L"PixelShader: " + material->pixelShaderName).c_str());
            ImGui::Text(WStringToString(L"VertexShader: " + material->vertexShaderName).c_str());
        }
    }
    if (transform != nullptr)
    {
        if (ImGui::CollapsingHeader("Transform"))
        {
            auto position = transform->GetPosition();
            if (ImGui::DragFloat3("Position: ", &position.x))
            {
                transform->SetPosition(position.x, position.y, position.z);
            }

            auto pitchYawRoll = transform->GetPitchYawRoll();
            if (ImGui::DragFloat3("PitchYawRoll: ", &pitchYawRoll.x))
            {
                transform->SetPitchYawRoll(pitchYawRoll.x, pitchYawRoll.y, pitchYawRoll.z);
            }

            auto scale = transform->GetScale();
            if (ImGui::DragFloat3("Scale: ", &scale.x))
            {
                transform->SetScale(scale.x, scale.y, scale.z);
            }
        }
    }
    if (light != nullptr)
    {
        if (ImGui::CollapsingHeader("Light"))
        {
            auto dir = light->dir;
            if (ImGui::DragFloat3("Dir: ", &dir.x))
            {
                light->dir = dir;
            }

            auto color = light->color;
            if (ImGui::ColorPicker3("Color: ", &color.x))
            {
                light->color = color;
            }

            auto intensity = light->intensity;
            if (ImGui::DragFloat("Intensity: ", &intensity))
            {
                light->intensity = intensity;
            }
        }
    }
}

// https://stackoverflow.com/questions/2573834/c-convert-string-or-char-to-wstring-or-wchar-t
std::string SceneEditor::WStringToString(std::wstring wstr)
{
    //setup converter
    std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
    return myconv.to_bytes(wstr);
}

std::wstring SceneEditor::StringToWString(std::string str)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
    return myconv.from_bytes(str);
}
