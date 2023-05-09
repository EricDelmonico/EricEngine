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

Renderer::Renderer(std::shared_ptr<D3DResources> d3dResources, AssetManager* assetManager) : m_d3dResources(d3dResources), m_assetManager(assetManager)
{
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
    auto lightEntities = em.GetEntitiesWithComponents<LightComponent>();
    for (int i = 0; i < lightEntities.size() && i < MAX_LIGHTS; i++)
    {
        lights[i] = em.GetComponent<LightComponent>(lightEntities[i])->data;
    }

    for (auto& i : meshTransformIDs)
    {
        Material* material = em.GetComponent<Material>(i);
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
        if (lights != nullptr)
        {
            pixelShader->SetData("lights", &lights, sizeof(Light) * MAX_LIGHTS);
        }
        pixelShader->CopyAllBufferData();

        // Set up cbuffer data
        Transform* transform = em.GetComponent<Transform>(i);
        auto vertexShader = material->vertexShader;
        vertexShader->SetShader();
        vertexShader->SetMatrix4x4("view", camera->GetView());
        vertexShader->SetMatrix4x4("projection", camera->GetProjection());
        vertexShader->SetMatrix4x4("model", transform->GetWorldMatrix());
        vertexShader->SetMatrix4x4("modelInvTranspose", transform->GetWorldInverseTransposeMatrix());
        vertexShader->CopyAllBufferData();

        Mesh* mesh = em.GetComponent<Mesh>(i);
        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, m_assetManager->GetVertexBuffer(mesh->name).GetAddressOf(), &stride, &offset);
        context->IASetIndexBuffer(m_assetManager->GetIndexBuffer(mesh->name).Get(), DXGI_FORMAT_R32_UINT, 0);

        context->DrawIndexed(mesh->indices, 0, 0);
    }

#ifdef _DEBUG
    ImGui::EndFrame();
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif

    m_d3dResources->GetSwapChain()->Present(1, NULL);
}