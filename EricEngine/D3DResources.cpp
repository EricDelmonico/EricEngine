#include "D3DResources.h"
#include <stdexcept>

D3DResources::D3DResources(int width, int height) : m_width(width), m_height(height)
{
}

D3DResources::~D3DResources()
{
}

HRESULT D3DResources::Initialize(HWND hWnd)
{
    HRESULT hr = 0;
    
    // Get interface to device and context
    {
        D3D_FEATURE_LEVEL levels[] = {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_9_2,
            D3D_FEATURE_LEVEL_9_1
        };

        // Add support for surfaces with a color channel ordering different from the API default
        UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(DEBUG) || defined(_DEBUG)
        deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        hr = D3D11CreateDevice(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            0,
            deviceFlags,
            levels,
            ARRAYSIZE(levels),
            D3D11_SDK_VERSION,
            &m_device,
            &m_featureLevel,
            &m_context);

        // If we failed to create a hardware device, create a warp device
        if (FAILED(hr))
        {
            hr = D3D11CreateDevice(
                nullptr,
                D3D_DRIVER_TYPE_WARP,
                NULL,
                deviceFlags,
                levels,
                ARRAYSIZE(levels),
                D3D11_SDK_VERSION,
                &m_device,
                &m_featureLevel,
                &m_context);

            if (FAILED(hr)) throw hr;
        }
    }

    // Create swap chain
    {
        DXGI_SWAP_CHAIN_DESC desc = {};
        desc.Windowed = TRUE;
        desc.BufferCount = 2;
        desc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        desc.SampleDesc.Count = 1;      //multisampling setting
        desc.SampleDesc.Quality = 0;    //vendor-specific flag
        desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        desc.OutputWindow = hWnd;

        // Create swap chain
        Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
        Microsoft::WRL::ComPtr<IDXGIFactory> factory;

        Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDevice;
        m_device.As(&dxgiDevice);

        hr = dxgiDevice->GetAdapter(&adapter);

        if (SUCCEEDED(hr))
        {
            adapter->GetParent(IID_PPV_ARGS(&factory));

            hr = factory->CreateSwapChain(m_device.Get(), &desc, &m_swapChain);
        }
    }

    // Get back buffer/render target from swap chain
    {
        hr = m_swapChain->GetBuffer(
            0,
            IID_PPV_ARGS(&m_backBuffer));

        hr = m_device->CreateRenderTargetView(
            m_backBuffer.Get(),
            nullptr,
            &m_renderTarget);
    }
    
    // Get back buffer desc to help create dsb/dsv and viewport
    D3D11_TEXTURE2D_DESC bbDesc = {};
    m_backBuffer->GetDesc(&bbDesc);

    // Create depth-stencil buffer and view
    {

        CD3D11_TEXTURE2D_DESC desc(
            DXGI_FORMAT_D24_UNORM_S8_UINT,
            bbDesc.Width,
            bbDesc.Height,
            1, // One texture
            1, // One mip level
            D3D11_BIND_DEPTH_STENCIL);

        m_device->CreateTexture2D(
            &desc,
            nullptr,
            &m_depthStencilBuffer);

        CD3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc(D3D11_DSV_DIMENSION_TEXTURE2D);

        m_device->CreateDepthStencilView(
            m_depthStencilBuffer.Get(),
            &dsvDesc,
            &m_depthStencilView);
    }

    // Create viewport
    {
        m_viewport = {};
        m_viewport.Height = (float)bbDesc.Height;
        m_viewport.Width = (float)bbDesc.Width;
        m_viewport.MinDepth = 0;
        m_viewport.MaxDepth = 1;

        m_context->RSSetViewports(1, &m_viewport);
    }

    m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
