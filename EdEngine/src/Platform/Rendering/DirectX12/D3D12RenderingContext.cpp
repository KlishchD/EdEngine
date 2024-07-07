#include "D3D12RenderingContext.h"
#include "D3D12Window.h"
#include "Helpers/StringHelper.h"
#include "D3D12Fence.h"

D3D12RenderingContext::D3D12RenderingContext(D3D12Window* window) : m_Window(window)
{
  CreateDevice();
  CreateCommandQueue();
  CreateSwapChain();
  CreateCommandList();
  gContext = this;
}

void D3D12RenderingContext::SetDefaultFramebuffer()
{

}

void D3D12RenderingContext::SetFramebuffer(std::shared_ptr<Framebuffer> framebuffer)
{

}

void D3D12RenderingContext::SetUniformBuffer(std::shared_ptr<UniformBuffer> buffer, uint32_t location)
{

}

void D3D12RenderingContext::SetVertexBuffer(std::shared_ptr<VertexBuffer> buffer)
{

}

void D3D12RenderingContext::SetIndexBuffer(std::shared_ptr<IndexBuffer> buffer)
{

}

void D3D12RenderingContext::SetShaderProgram(std::shared_ptr<ShaderProgram> program)
{

}

void D3D12RenderingContext::SetShaderDataTexture(const std::string& name, std::shared_ptr<Texture> texture)
{

}

void D3D12RenderingContext::SetShaderDataTexture(const char* name, std::shared_ptr<Texture> texture)
{

}

void D3D12RenderingContext::SetShaderDataImage(const std::string& name, std::shared_ptr<Texture> texture)
{

}

void D3D12RenderingContext::SetShaderDataImage(const char* name, std::shared_ptr<Texture> texture)
{

}

void D3D12RenderingContext::SetShaderDataInt(const std::string& name, int32_t value)
{

}

void D3D12RenderingContext::SetShaderDataInt(const char* name, int32_t value)
{

}

void D3D12RenderingContext::SetShaderDataFloat(const std::string& name, float value)
{

}

void D3D12RenderingContext::SetShaderDataFloat(const char* name, float value)
{

}

void D3D12RenderingContext::SetShaderDataFloat2(const std::string& name, glm::vec2 vector)
{

}

void D3D12RenderingContext::SetShaderDataFloat2(const std::string& name, float x, float y)
{

}

void D3D12RenderingContext::SetShaderDataFloat2(const char* name, glm::vec2 vector)
{

}

void D3D12RenderingContext::SetShaderDataFloat2(const char* name, float x, float y)
{

}

void D3D12RenderingContext::SetShaderDataFloat3(const std::string& name, float x, float y, float z)
{

}

void D3D12RenderingContext::SetShaderDataFloat3(const std::string& name, glm::vec3 vector)
{

}

void D3D12RenderingContext::SetShaderDataFloat3(const char* name, float x, float y, float z)
{

}

void D3D12RenderingContext::SetShaderDataFloat3(const char* name, glm::vec3 vector)
{

}

void D3D12RenderingContext::SetShaderDataFloat4(const std::string& name, float r, float g, float b, float a)
{

}

void D3D12RenderingContext::SetShaderDataFloat4(const std::string& name, glm::vec4 vector)
{

}

void D3D12RenderingContext::SetShaderDataFloat4(const char* name, float r, float g, float b, float a)
{

}

void D3D12RenderingContext::SetShaderDataFloat4(const char* name, glm::vec4 vector)
{

}

void D3D12RenderingContext::SetShaderDataMat4(const std::string& name, const glm::mat4& matrix)
{

}

void D3D12RenderingContext::SetShaderDataMat4(const char* name, const glm::mat4& matrix)
{

}

void D3D12RenderingContext::SetShaderDataMat3(const std::string& name, const glm::mat3& matrix)
{

}

void D3D12RenderingContext::SetShaderDataMat3(const char* name, const glm::mat3& matrix)
{

}

void D3D12RenderingContext::SetShaderDataBool(const std::string& name, bool value)
{

}

void D3D12RenderingContext::SetShaderDataBool(const char* name, bool value)
{

}

void D3D12RenderingContext::RunComputeShader(uint32_t sizeX, uint32_t sizeY, uint32_t sizeZ)
{

}

void D3D12RenderingContext::Barier(BarrierType type)
{

}

void D3D12RenderingContext::Draw(DrawMode drawMode /*= DrawMode::Triangles*/)
{

}

void D3D12RenderingContext::EnableBlending(BlendFactor source, BlendFactor destination)
{

}

void D3D12RenderingContext::SetBlending(BlendFactor source, BlendFactor destination)
{

}

void D3D12RenderingContext::DisableBlending()
{

}

void D3D12RenderingContext::EnableDethTest(DepthTestFunction function)
{

}

void D3D12RenderingContext::SetDethTestFunction(DepthTestFunction function)
{

}

void D3D12RenderingContext::DisableDethTest()
{

}

void D3D12RenderingContext::EnableFaceCulling()
{

}

void D3D12RenderingContext::EnableFaceCulling(Face face)
{

}

void D3D12RenderingContext::SetCullingFace(Face face)
{

}

void D3D12RenderingContext::DisableFaceCulling()
{

}

void D3D12RenderingContext::ClearDepthTarget()
{

}

void D3D12RenderingContext::ClearColorTarget()
{

}

void D3D12RenderingContext::SetClearColor(float r, float g, float b, float a)
{

}

void D3D12RenderingContext::SetClearColor(glm::vec4 color)
{

}

void D3D12RenderingContext::BeginUIFrame()
{

}

void D3D12RenderingContext::EndUIFrame()
{

}

void D3D12RenderingContext::Present()
{
  D3D::Check(m_CommandAllocator->Reset());

  D3D::Check(m_CommandList->Reset(m_CommandAllocator.Get(), nullptr));

  uint32_t backBufferIndex = m_SwapChain->GetCurrentBackBufferIndex();

  {
    D3D12_RESOURCE_BARRIER barrier = D3D12Helper::TransitionBarrier(m_BackBufferRenderTargetResource[backBufferIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_CommandList->ResourceBarrier(1, &barrier);
  }

  static glm::vec4 colors;

  colors.r = (rand() % 256) / 256.0f;
  colors.g = (rand() % 256) / 256.0f;
  colors.b = (rand() % 256) / 256.0f;
  colors.a = 1.0f;

  D3D12_CPU_DESCRIPTOR_HANDLE handle{ m_RenderTargetDescriptorHeap->GetCPUDescriptorHandleForHeapStart().ptr + m_RenderTargetDescriptorSize * backBufferIndex };
  m_CommandList->ClearRenderTargetView(handle, glm::value_ptr(colors), 0, nullptr);

  {
    D3D12_RESOURCE_BARRIER barrier = D3D12Helper::TransitionBarrier(m_BackBufferRenderTargetResource[backBufferIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    m_CommandList->ResourceBarrier(1, &barrier);
  }

  D3D::Check(m_CommandList->Close());

  ID3D12CommandList* commandLists = m_CommandList.Get();
  m_CommandQueue->ExecuteCommandLists(1, &commandLists);

  static D3D12Fence fence;
  fence.Wait(m_CommandQueue);

  m_SwapChain->Present(0, 0);
}

void D3D12RenderingContext::Close()
{

}

Microsoft::WRL::ComPtr<ID3D12Device> D3D12RenderingContext::GetDevice() const
{
  return m_Device;
}

void D3D12RenderingContext::CreateDevice()
{
  uint32_t factoryFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
  factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

  D3D::Check(CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&m_Factory)));

  ED_ASSERT(m_Factory, "Failed to create factory");

  for (uint32_t i = 0; SUCCEEDED(m_Factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&m_Adapter))); ++i)
  {
    D3D::Check(D3D12CreateDevice(m_Adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_Device)));

    if (m_Device)
    {
      break;
    }
  }

  ED_ASSERT(m_Device, "Failed to create device");

#if defined(DEBUG) || defined(_DEBUG)
  LogAdapterInformation();
#endif
}

void D3D12RenderingContext::CreateCommandQueue()
{
  D3D12_COMMAND_QUEUE_DESC commandQueueDescription {};
  commandQueueDescription.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
  commandQueueDescription.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
  commandQueueDescription.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
  commandQueueDescription.NodeMask = 0;
  D3D::Check(m_Device->CreateCommandQueue(&commandQueueDescription, IID_PPV_ARGS(&m_CommandQueue)));

  ED_ASSERT(m_CommandQueue, "Failed to create command queue");
}

void D3D12RenderingContext::CreateSwapChain()
{
  HWND hWnd = glfwGetWin32Window((GLFWwindow*)m_Window->GetNativeWindow());

  DXGI_SWAP_CHAIN_DESC1 swapChainDescription {};
  swapChainDescription.Width = m_Window->GetWidth();
  swapChainDescription.Height = m_Window->GetHeight();
  swapChainDescription.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  swapChainDescription.Stereo = false;
  swapChainDescription.SampleDesc.Count = 1;
  swapChainDescription.SampleDesc.Quality = 0;
  swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  swapChainDescription.BufferCount = BackBufferCount;
  swapChainDescription.Scaling = DXGI_SCALING_NONE;
  swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
  swapChainDescription.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
  swapChainDescription.Flags = 0; // TODO: think of adding DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING for variable refresh rate monitors

  Microsoft::WRL::ComPtr<IDXGISwapChain1> swapChain;
  D3D::Check(m_Factory->CreateSwapChainForHwnd(m_CommandQueue.Get(), hWnd, &swapChainDescription, nullptr, nullptr, &swapChain));
  D3D::Check(swapChain.As<IDXGISwapChain4>(&m_SwapChain));

  ED_ASSERT(m_SwapChain, "Failed to create swap chain");

  D3D12_DESCRIPTOR_HEAP_DESC  renderTargetViewsDescriptorHeapDescription {};
  renderTargetViewsDescriptorHeapDescription.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
  renderTargetViewsDescriptorHeapDescription.NumDescriptors = BackBufferCount;
  renderTargetViewsDescriptorHeapDescription.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
  renderTargetViewsDescriptorHeapDescription.NodeMask = 0;
  m_Device->CreateDescriptorHeap(&renderTargetViewsDescriptorHeapDescription, IID_PPV_ARGS(&m_RenderTargetDescriptorHeap));

  ED_ASSERT(m_RenderTargetDescriptorHeap, "Failed to create render target descriptor heap");

  m_RenderTargetDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

  for (uint32_t i = 0; i < BackBufferCount; ++i)
  {
    m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_BackBufferRenderTargetResource[i]));
    ED_ASSERT(m_BackBufferRenderTargetResource[i], "Failed to get back buffer render target {} resource", i);

    D3D12_RENDER_TARGET_VIEW_DESC renderTargetViewDescription {};
    renderTargetViewDescription.Format = swapChainDescription.Format;
    renderTargetViewDescription.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDescription.Texture2D.MipSlice = 0;
    renderTargetViewDescription.Texture2D.PlaneSlice = 0;

    D3D12_CPU_DESCRIPTOR_HANDLE renderTargetViewHandle { m_RenderTargetDescriptorHeap->GetCPUDescriptorHandleForHeapStart().ptr + m_RenderTargetDescriptorSize * i };
    m_Device->CreateRenderTargetView(m_BackBufferRenderTargetResource[i].Get(), &renderTargetViewDescription, renderTargetViewHandle);
  }
}

void D3D12RenderingContext::CreateCommandList()
{
  D3D::Check(m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator)));

  ED_ASSERT(m_CommandAllocator, "Failed to create command allocator");

  D3D::Check(m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_CommandList)));

  ED_ASSERT(m_CommandList, "Failed to create command list");

  D3D::Check(m_CommandList->Close());
}

void D3D12RenderingContext::LogAdapterInformation()
{
  DXGI_ADAPTER_DESC adapterDescription;
  D3D::Check(m_Adapter->GetDesc(&adapterDescription));

  ED_LOG(D3D12RenderingContext, info, "Adapter description: {}", StringHelper::WCHARToString<128>(adapterDescription.Description));
  ED_LOG(D3D12RenderingContext, info, "Adapter vendorID: {}", adapterDescription.VendorId);
  ED_LOG(D3D12RenderingContext, info, "Adapter deviceID: {}", adapterDescription.DeviceId);

  ED_LOG(D3D12RenderingContext, info, "Adapter dedicated video memory: {:L} GB", adapterDescription.DedicatedVideoMemory / (1 << 20));
  ED_LOG(D3D12RenderingContext, info, "Adapter dedicated system memory: {:L} GB", adapterDescription.DedicatedSystemMemory / (1 << 20));
  ED_LOG(D3D12RenderingContext, info, "Adapter shared system memory: {:L} GB", adapterDescription.SharedSystemMemory / (1 << 20));
}
