#include "Renderer.h"
#include "VertexPositionColor.h"

Renderer::Renderer(std::shared_ptr<D3DResources> d3dResources, std::shared_ptr<Camera> camera, AssetManager* assetManager) : m_d3dResources(d3dResources), m_camera(camera), m_assetManager(assetManager), m_externalData({})
{
    // Set up cbuffer
    CD3D11_BUFFER_DESC cbufferDesc(sizeof(ExternalData), D3D11_BIND_CONSTANT_BUFFER);
    D3D11_SUBRESOURCE_DATA cbufferData = {};
    cbufferData.pSysMem = &m_externalData;
    cbufferData.SysMemPitch = 0;
    cbufferData.SysMemSlicePitch = 0;

    d3dResources->GetDevice()->CreateBuffer(&cbufferDesc, &cbufferData, m_cbuffer.GetAddressOf());
}

void Renderer::Render(std::vector<std::shared_ptr<Entity>> entities)
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
    context->VSSetShader(m_assetManager->GetVertexShader("VertexShader").Get(), nullptr, 0);

    // Set up pixel shader
    context->PSSetShader(m_assetManager->GetPixelShader("PixelShader").Get(), nullptr, 0);
    
    // Set up IA for every mesh
    UINT stride = sizeof(VertexPositionColor);
    UINT offset = 0;
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->IASetInputLayout(m_assetManager->GetInputLayout("VertexShader").Get());

    // Only need one view/projection for all models
    m_externalData.View = m_camera->GetView();
    m_externalData.Projection = m_camera->GetProjection();

    // Draw each entity
    for (auto& entity : entities)
    {
        // Set up cbuffer data
        Transform* transform = entity->GetTransform();
        m_externalData.Model = transform->GetWorldMatrix();
        context->UpdateSubresource(m_cbuffer.Get(), 0, nullptr, &m_externalData, 0, 0);

        // Send cbuffer data to vertex shader
        context->VSSetConstantBuffers(0, 1, m_cbuffer.GetAddressOf());

        auto mesh = entity->GetMesh();
        context->IASetVertexBuffers(0, 1, mesh->GetVertexBuffer().GetAddressOf(), &stride, &offset);
        context->IASetIndexBuffer(mesh->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);

        context->DrawIndexed(mesh->GetNumIndices(), 0, 0);
    }

    m_d3dResources->GetSwapChain()->Present(1, NULL);
}

void Renderer::CreateViewAndPerspective()
{
    // Use DirectXMath to create view and perspective matrices.

    DirectX::XMVECTOR eye = DirectX::XMVectorSet(0.0f, 0.0f, 5.0f, 0.0f);
    DirectX::XMVECTOR at = DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f);
    DirectX::XMVECTOR up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    DirectX::XMStoreFloat4x4(
        &m_externalData.View,
        DirectX::XMMatrixLookAtRH(
            eye,
            at,
            up
        )
    );

    float aspectRatioX = 16.0f / 9.0f;
    float aspectRatioY = 1.0f;

    DirectX::XMStoreFloat4x4(
        &m_externalData.Projection,
        DirectX::XMMatrixPerspectiveFovRH(
            2.0f * std::atan(std::tan(DirectX::XMConvertToRadians(70) * 0.5f) / aspectRatioY),
            aspectRatioX,
            0.01f,
            100.0f
        )
    );
}