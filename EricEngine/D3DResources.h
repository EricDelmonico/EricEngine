#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <wrl/client.h>
#include <dxgi.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "dxgi.lib")

class D3DResources
{
public:
    D3DResources(int width, int height);
    ~D3DResources();
    HRESULT Initialize(HWND hWnd);

    ID3D11Device* GetDevice() { return m_device.Get(); }
    ID3D11DeviceContext* GetContext() { return m_context.Get(); }

    Microsoft::WRL::ComPtr<ID3D11Device> GetDeviceComPtr() { return m_device; }
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetContextComPtr() { return m_context; }

    ID3D11RenderTargetView* GetRenderTarget() { return m_renderTarget.Get(); }
    ID3D11DepthStencilView* GetDepthStencilView() { return m_depthStencilView.Get(); }

    IDXGISwapChain* GetSwapChain() { return m_swapChain.Get(); }

private:
    Microsoft::WRL::ComPtr<ID3D11Device> m_device;
    Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_context;
    Microsoft::WRL::ComPtr<IDXGIAdapter> m_deviceAdapter;
    Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_backBuffer;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTarget;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthStencilBuffer;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;

    D3D11_VIEWPORT m_viewport;

    int m_width, m_height;

    D3D_FEATURE_LEVEL m_featureLevel;
};

