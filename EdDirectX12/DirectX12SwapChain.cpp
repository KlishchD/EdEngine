#include "EdDirectX12.h"
#include "Window.h"

#define GetSwapChain() reinterpret_cast<IDXGISwapChain4*>(m_NativeHandle)

SwapChain::SwapChain(CommandQueue* queue, Window* window, PixelFormat format, u32 width, u32 height) : m_NativeHandle(0)
{
    DXGI_SWAP_CHAIN_DESC1 description;
    description.Width = width;
    description.Height = height;
    description.Format = DirectX12Types::ConvertPixelFormat(format);
    description.Stereo = false;
    description.SampleDesc.Count = 1;
    description.SampleDesc.Quality = 0;
    description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    description.BufferCount = 2;
    description.Scaling = DXGI_SCALING_NONE;
    description.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    description.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    description.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    HWND hWnd = (HWND)window->GetPlatformNativeWindow();
    D3D::Check(g_Factory->CreateSwapChainForHwnd(queue->GetNativeHandle<ID3D12CommandQueue>(), hWnd, &description, nullptr, nullptr, reinterpret_cast<IDXGISwapChain1**>(&m_NativeHandle)));

    for (u32 index = 0; index < description.BufferCount; ++index)
    {
        void* resource;
        D3D::Check(GetSwapChain()->GetBuffer(index, __uuidof(ID3D12Resource), &resource));

        m_Targets.Add(new Resource(static_cast<ResourceFlags>(RF_HolderResource | RF_AllowRenderTarget), ResourceState::Common, reinterpret_cast<uptr>(resource), format, width, height, 1, 1, "Display Buffer"));
    }
}

void SwapChain::Present()
{
    D3D::Check(GetSwapChain()->Present(0, DXGI_PRESENT_ALLOW_TEARING));
}

u32 SwapChain::GetActiveBackBufferIndex() const
{
    return GetSwapChain()->GetCurrentBackBufferIndex();
}

SwapChain::~SwapChain()
{
    for (Resource* resource : m_Targets)
    {
        delete resource;
    }

    GetSwapChain()->Release();
}
