#include "Renderer.h"
#include "Vertex.h"
#include <algorithm>
#include <iterator>

Renderer::Renderer(std::shared_ptr<D3DResources> d3dResources, std::shared_ptr<Camera> camera, AssetManager* assetManager) : m_d3dResources(d3dResources), m_camera(camera), m_assetManager(assetManager)
{
}

void Renderer::Render(ECS::EntityManager* em)
{
    auto context = m_d3dResources->GetContext();

    auto renderTarget = m_d3dResources->GetRenderTarget();
    auto depthStencilView = m_d3dResources->GetDepthStencilView();

    // Clear render target view and depth stencil view
    const float clearColor[] = { 0.0f, 0.75f, 0.75f, 1.0f };
    context->ClearRenderTargetView(renderTarget, clearColor);
    context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // Set render target
    context->OMSetRenderTargets(1, &renderTarget, depthStencilView);

    // Set up vertex shader
    auto vertexShader = m_assetManager->GetVertexShader(L"VertexShader");
    vertexShader->SetShader();

    // Set up pixel shader
    auto pixelShader = m_assetManager->GetPixelShader(L"PixelShader");
    pixelShader->SetShader();

    // Only need one view/projection for all models
    vertexShader->SetMatrix4x4("view", m_camera->GetView());
    vertexShader->SetMatrix4x4("projection", m_camera->GetProjection());

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> rockSandstoneSRV = m_assetManager->GetTexture(L"rock_sandstone_albedo.tif");
    Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState = m_assetManager->GetSamplerState();

    pixelShader->SetShaderResourceView("Albedo", rockSandstoneSRV);
    pixelShader->SetSamplerState("BasicSampler", samplerState);
    pixelShader->CopyAllBufferData();

    // Draw each entity
    // We need both a mesh and a transform
    std::vector<int> meshTransformIDs;
    auto& meshEntityIDs = em->componentEntityIDs[Mesh::id];
    auto& transformEntityIDs = em->componentEntityIDs[Transform::id];
    std::set_intersection(meshEntityIDs.begin(), meshEntityIDs.end(), transformEntityIDs.begin(), transformEntityIDs.end(), std::back_inserter(meshTransformIDs));
    for (auto& i : meshTransformIDs)
    {
        // Set up cbuffer data
        Transform* transform = em->GetComponent<Transform>(i);
        vertexShader->SetMatrix4x4("model", transform->GetWorldMatrix());
        vertexShader->SetMatrix4x4("modelInvTranspose", transform->GetWorldInverseTransposeMatrix());
        vertexShader->CopyAllBufferData();

        Mesh* mesh = em->GetComponent<Mesh>(i);
        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, mesh->vertexBuffer.GetAddressOf(), &stride, &offset);
        context->IASetIndexBuffer(mesh->indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

        context->DrawIndexed(mesh->indices, 0, 0);
    }

    m_d3dResources->GetSwapChain()->Present(1, NULL);
}