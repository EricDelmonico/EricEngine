#include "SceneLoader.h"

SceneLoader::SceneLoader(AssetManager* am) : am(am)
{
    this->em = &ECS::EntityManager::GetInstance();
}

SceneLoader::~SceneLoader()
{
    DeleteLoadedComponents();
}

void SceneLoader::SaveScene(std::string name)
{
    // Grab the array of entities for convenience
    auto* entities = em->entities;

    std::ofstream os;
    os.open(GetExePath() + "../../Levels/" + name, std::ios::binary | std::ios::out);

    // Write total entity amount
    int count = em->entityCount;
    os.write((char*)(&count), sizeof(int));

    // Write every component for every existing entity
    for (int i = 0; i < MAX_ENTITIES; i++)
    {
        if (!entities[i]) continue;

        // Count the number of components we have
        int components = 0;
        Mesh* mesh = em->GetComponent<Mesh>(i);
        Material* material = em->GetComponent<Material>(i);
        Camera* camera = em->GetComponent<Camera>(i);
        Transform* transform = em->GetComponent<Transform>(i);
        Light* light = em->GetComponent<Light>(i);

        if (mesh != nullptr) components++;
        if (material != nullptr) components++;
        if (camera != nullptr) components++;
        if (transform != nullptr) components++;
        if (light != nullptr) components++;

        // Write the number of components, then write each component
        os.write((char*)(&components), sizeof(int));
        WriteComponent<Mesh>(mesh, os);
        WriteComponent<Material>(material, os);
        WriteComponent<Camera>(camera, os);
        WriteComponent<Transform>(transform, os);
        WriteComponent<Light>(light, os);
    }

    os.close();
}

void SceneLoader::LoadScene(std::string name)
{
    em->DeregisterAllEntities();
    if (loadedComponents.size() > 0)
    {
        DeleteLoadedComponents();
    }

    std::ifstream in;
    in.open(GetExePath() + "../../Levels/" + name, std::ios::binary | std::ios::in);
    
    int entityCount = -1;
    in.read((char*)(&entityCount), sizeof(int));

    // Read entity by entity.
    // Entities are written like:
    // #ofcomponents_componentID_component
    // With no spacing between data.
    for (int i = 0; i < entityCount; i++)
    {
        // Register this entity
        int e = em->RegisterNewEntity();

        // Get total number of components
        int numComponents = -1;
        in.read((char*)(&numComponents), sizeof(int));

        for (int c = 0; c < numComponents; c++)
        {
            // Read in the component ID
            int componentID = INVALID_COMPONENT;
            in.read((char*)(&componentID), sizeof(int));

            // Read in the component itself
            ECS::Component* component = ReadComponent(in, componentID);

            // Add the component to its entity
            em->AddComponent(componentID, e, component);
        }
    }

    in.close();
}

void SceneLoader::DeleteLoadedComponents()
{
    for (auto& c : loadedComponents)
    {
        delete c;
    }
    loadedComponents.clear();
}

ECS::Component* SceneLoader::ReadComponent(std::ifstream& in, int componentID)
{
    // Handle any special cases
    if (componentID == Mesh::id)
    {
        // Just grab the mesh using its name
        return am->GetMesh(ReadString(in));
    }

    if (componentID == Transform::id)
    {
        Transform* transform = new Transform();
        in.read((char*)(&transform->position), sizeof(DirectX::XMFLOAT3));
        in.read((char*)(&transform->pitchYawRoll), sizeof(DirectX::XMFLOAT3));
        in.read((char*)(&transform->scale), sizeof(DirectX::XMFLOAT3));
        in.read((char*)(&transform->worldMatrix), sizeof(DirectX::XMFLOAT4X4));
        in.read((char*)(&transform->worldInverseTransposeMatrix), sizeof(DirectX::XMFLOAT4X4));
        in.read((char*)(&transform->matricesDirty), sizeof(bool));
        loadedComponents.push_back(transform);
        return transform;
    }

    if (componentID == Material::id)
    {
        Material* material = new Material();
        material->albedoName = ReadWString(in);
        material->normalsName = ReadWString(in);
        material->metalnessName = ReadWString(in);
        material->roughnessName = ReadWString(in);
        material->aoName = ReadWString(in);
        material->pixelShaderName = ReadWString(in);
        material->vertexShaderName = ReadWString(in);

        material->albedo = am->GetTexture(material->albedoName);
        material->metalness = am->GetTexture(material->metalnessName);
        material->normals = am->GetTexture(material->normalsName);
        material->roughness = am->GetTexture(material->roughnessName);
        material->ao = am->GetTexture(material->aoName);

        material->pixelShader = am->GetPixelShader(material->pixelShaderName).get();
        material->vertexShader = am->GetVertexShader(material->vertexShaderName).get();

        material->samplerState = am->GetSamplerState();

        loadedComponents.push_back(material);
        return material;
    }

    if (componentID == Camera::id)
    {
        float floatParams[11];
        bool perspective;

        for (int i = 0; i < 11; i++)
        {
            in.read((char*)(&(floatParams[i])), sizeof(float));
        }
        in.read((char*)(&perspective), sizeof(bool));
        auto cam = new Camera(
            floatParams[0],
            floatParams[1],
            floatParams[2],
            floatParams[3],
            floatParams[4],
            floatParams[5],
            floatParams[6],
            floatParams[7],
            perspective,
            floatParams[8],
            floatParams[9],
            floatParams[10]);
        loadedComponents.push_back(cam);
        return cam;
    }

    if (componentID == Light::id)
    {
        Light* light = new Light();
        in.read((char*)(&light->dir), sizeof(DirectX::XMFLOAT3));
        in.read((char*)(&light->color), sizeof(DirectX::XMFLOAT3));
        in.read((char*)(&light->intensity), sizeof(float));
        loadedComponents.push_back(light);
        return light;
    }

    throw;
}

std::string SceneLoader::GetExePath()
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
