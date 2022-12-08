#include "Renderer.h"
#include "Vertex.h"
#include "Material.h"
#include "Light.h"
#include <algorithm>
#include <iterator>

#ifdef _DEBUG
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_dx11.h"
#include "ImGui/imgui_impl_win32.h"
#endif

Renderer::Renderer(std::shared_ptr<D3DResources> d3dResources, Mesh* cube) : m_d3dResources(d3dResources)
{
    this->cube = cube;
    cubeTransform = Transform();
}

void Renderer::Render()
{
    auto& em = ECS::EntityManager::GetInstance();

    auto context = m_d3dResources->GetContext();

    auto renderTarget = m_d3dResources->GetRenderTarget();
    auto depthStencilView = m_d3dResources->GetDepthStencilView();

    // Clear render target view and depth stencil view
    const float clearColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
    context->ClearRenderTargetView(renderTarget, clearColor);
    context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // Set render target
    context->OMSetRenderTargets(1, &renderTarget, depthStencilView);

    // Draw each entity
    // We need a mesh, a transform, and a material
    std::vector<int> meshTransformIDs = em.GetEntitiesWithComponents<Mesh, Transform, Material>();
    
    // Get our camera for rendering
    auto cameras = em.GetEntitiesWithComponents<Camera>();
    // Can't render without a camera
    if (cameras.size() == 0) return;
    int cameraIndex = cameras[0];
    Camera* camera = em.GetComponent<Camera>(cameraIndex);

    // Grab our light(s)
    auto lightEntities = em.GetEntitiesWithComponents<Light>();
    int lightIndex = 0;
    if (lightEntities.size() != 0) lightIndex = lightEntities[0];
    Light* light = em.GetComponent<Light>(lightIndex);

    Material* material = nullptr;
    SimpleVertexShader* vertexShader = nullptr;
    for (auto& i : meshTransformIDs)
    {
        material = em.GetComponent<Material>(i);
        auto pixelShader = material->pixelShader;
        pixelShader->SetShader();
        pixelShader->SetShaderResourceView("Albedo", material->albedo);
        pixelShader->SetShaderResourceView("Normals", material->normals);
        pixelShader->SetShaderResourceView("Metalness", material->metalness);
        pixelShader->SetShaderResourceView("Roughness", material->roughness);
        pixelShader->SetShaderResourceView("AO", material->ao);
        pixelShader->SetSamplerState("BasicSampler", material->samplerState);
        pixelShader->SetFloat3("camPosition", camera->GetTransform()->GetPosition());
        pixelShader->SetFloat3("tint", material->tint);
        if (light != nullptr)
        {
            pixelShader->SetFloat3("sunDir", light->dir);
            pixelShader->SetFloat3("sunColor", light->color);
            pixelShader->SetFloat("sunIntensity", light->intensity);
        }
        pixelShader->CopyAllBufferData();

        // Set up cbuffer data
        Transform* transform = em.GetComponent<Transform>(i);
        vertexShader = material->vertexShader;
        vertexShader->SetShader();
        vertexShader->SetMatrix4x4("view", camera->GetView());
        vertexShader->SetMatrix4x4("projection", camera->GetProjection());
        vertexShader->SetMatrix4x4("model", transform->GetWorldMatrix());
        vertexShader->SetMatrix4x4("modelInvTranspose", transform->GetWorldInverseTransposeMatrix());
        vertexShader->CopyAllBufferData();

        Mesh* mesh = em.GetComponent<Mesh>(i);
        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
        context->IASetIndexBuffer(mesh->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

        context->DrawIndexed(mesh->indices, 0, 0);

        {
            auto min = mesh->boundingMin;
            auto max = mesh->boundingMax;

            auto scale = transform->GetScale();

            cubeTransform.SetPosition(min.x, min.y, min.z);
            cubeTransform.SetScale(1.0f / scale.x, 1.0f / scale.y, 1.0f / scale.z);

            auto modelWorld = transform->GetWorldMatrix();
            auto cubeWorld = cubeTransform.GetWorldMatrix();
            DirectX::XMMATRIX cubeWorldMat = DirectX::XMLoadFloat4x4(&cubeWorld);
            DirectX::XMMATRIX modelWorldMat = DirectX::XMLoadFloat4x4(&modelWorld);
            DirectX::XMMATRIX cnew = cubeWorldMat * modelWorldMat;
            DirectX::XMFLOAT4X4 whatever;
            DirectX::XMStoreFloat4x4(&whatever, cnew);
            vertexShader->SetMatrix4x4("view", camera->GetView());
            vertexShader->SetMatrix4x4("projection", camera->GetProjection());
            vertexShader->SetMatrix4x4("model", whatever);
            vertexShader->SetMatrix4x4("modelInvTranspose", transform->GetWorldInverseTransposeMatrix());
            vertexShader->CopyAllBufferData();

            mesh = cube;
            stride = sizeof(Vertex);
            offset = 0;
            context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
            context->IASetIndexBuffer(mesh->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

            context->DrawIndexed(mesh->indices, 0, 0);

            cubeTransform.SetPosition(max.x, max.y, max.z);
            cubeTransform.SetScale(1.0f / scale.x, 1.0f / scale.y, 1.0f / scale.z);

            modelWorld = transform->GetWorldMatrix();
            cubeWorld = cubeTransform.GetWorldMatrix();
            cubeWorldMat = DirectX::XMLoadFloat4x4(&cubeWorld);
            modelWorldMat = DirectX::XMLoadFloat4x4(&modelWorld);
            cnew = cubeWorldMat * modelWorldMat;
            DirectX::XMStoreFloat4x4(&whatever, cnew);
            vertexShader->SetMatrix4x4("view", camera->GetView());
            vertexShader->SetMatrix4x4("projection", camera->GetProjection());
            vertexShader->SetMatrix4x4("model", whatever);
            vertexShader->SetMatrix4x4("modelInvTranspose", transform->GetWorldInverseTransposeMatrix());
            vertexShader->CopyAllBufferData();

            mesh = cube;
            stride = sizeof(Vertex);
            offset = 0;
            context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
            context->IASetIndexBuffer(mesh->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

            context->DrawIndexed(mesh->indices, 0, 0);
        }
    }

    if (material != nullptr)
    {
        auto pixelShader = material->pixelShader;
        pixelShader->SetShader();
        pixelShader->SetShaderResourceView("Albedo", material->albedo);
        pixelShader->SetShaderResourceView("Normals", material->normals);
        pixelShader->SetShaderResourceView("Metalness", material->metalness);
        pixelShader->SetShaderResourceView("Roughness", material->roughness);
        pixelShader->SetShaderResourceView("AO", material->ao);
        pixelShader->SetSamplerState("BasicSampler", material->samplerState);
        pixelShader->SetFloat3("camPosition", camera->GetTransform()->GetPosition());
        pixelShader->SetFloat3("tint", material->tint);
        if (light != nullptr)
        {
            pixelShader->SetFloat3("sunDir", light->dir);
            pixelShader->SetFloat3("sunColor", light->color);
            pixelShader->SetFloat("sunIntensity", light->intensity);
        }
        pixelShader->CopyAllBufferData();

        auto min = camera->GetTransform()->GetPosition();
        auto max = camera->GetTransform()->GetPosition();
        auto forward = camera->GetTransform()->GetForward();
        max.x += forward.x * -10;
        max.y += forward.y * -10;
        max.z += forward.z * -10;

        auto transform = camera->GetTransform();
        auto scale = transform->GetScale();

        cubeTransform.SetPosition(max.x, max.y, max.z);
        cubeTransform.SetScale(1, 1, 1);

        auto modelWorld = transform->GetWorldMatrix();
        auto cubeWorld = cubeTransform.GetWorldMatrix();
        DirectX::XMMATRIX cubeWorldMat = DirectX::XMLoadFloat4x4(&cubeWorld);
        DirectX::XMMATRIX modelWorldMat = DirectX::XMLoadFloat4x4(&modelWorld);
        DirectX::XMMATRIX cnew = cubeWorldMat * modelWorldMat;
        DirectX::XMFLOAT4X4 whatever;
        DirectX::XMStoreFloat4x4(&whatever, cnew);
        vertexShader = material->vertexShader;
        vertexShader->SetMatrix4x4("view", camera->GetView());
        vertexShader->SetMatrix4x4("projection", camera->GetProjection());
        vertexShader->SetMatrix4x4("model", cubeWorld);
        vertexShader->SetMatrix4x4("modelInvTranspose", transform->GetWorldInverseTransposeMatrix());
        vertexShader->CopyAllBufferData();

        Mesh* mesh = cube;
        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
        context->IASetIndexBuffer(mesh->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

        context->DrawIndexed(mesh->indices, 0, 0);
    }

#ifdef _DEBUG
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

    m_d3dResources->GetSwapChain()->Present(1, NULL);
}