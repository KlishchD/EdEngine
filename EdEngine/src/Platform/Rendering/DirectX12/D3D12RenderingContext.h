#pragma once

#include "Platform/Rendering/DirectX12/EdD3D12Rendering.h"
#include "Core/Rendering/RenderingContex.h"

class D3D12Window;
class D3D12Resource;

class D3D12RenderingContext : public RenderingContext
{
public:
  D3D12RenderingContext(D3D12Window* window);

  virtual void SetDefaultFramebuffer() override;
  virtual void SetFramebuffer(std::shared_ptr<Framebuffer> framebuffer) override;

  virtual void SetUniformBuffer(std::shared_ptr<UniformBuffer> buffer, uint32_t location) override;
  virtual void SetVertexBuffer(std::shared_ptr<VertexBuffer> buffer) override;
  virtual void SetIndexBuffer(std::shared_ptr<IndexBuffer> buffer) override;

  virtual void SetShaderProgram(std::shared_ptr<ShaderProgram> program) override;

  virtual void SetShaderDataTexture(const std::string& name, std::shared_ptr<Texture> texture) override;
  virtual void SetShaderDataImage(const std::string& name, std::shared_ptr<Texture> texture) override;
  virtual void SetShaderDataInt(const std::string& name, int32_t value) override;
  virtual void SetShaderDataFloat(const std::string& name, float value) override;
  virtual void SetShaderDataFloat2(const std::string& name, glm::vec2 vector) override;
  virtual void SetShaderDataFloat2(const std::string& name, float x, float y) override;
  virtual void SetShaderDataFloat3(const std::string& name, float x, float y, float z) override;
  virtual void SetShaderDataFloat3(const std::string& name, glm::vec3 vector) override;
  virtual void SetShaderDataFloat4(const std::string& name, float r, float g, float b, float a) override;
  virtual void SetShaderDataFloat4(const std::string& name, glm::vec4 vector) override;
  virtual void SetShaderDataMat4(const std::string& name, const glm::mat4& matrix) override;
  virtual void SetShaderDataMat3(const std::string& name, const glm::mat3& matrix) override;
  virtual void SetShaderDataBool(const std::string& name, bool value) override;

  virtual void SetShaderDataTexture(const char* name, std::shared_ptr<Texture> texture) override;
  virtual void SetShaderDataImage(const char* name, std::shared_ptr<Texture> texture) override;
  virtual void SetShaderDataInt(const char* name, int32_t value) override;
  virtual void SetShaderDataFloat(const char* name, float value) override;
  virtual void SetShaderDataFloat2(const char* name, glm::vec2 vector) override;
  virtual void SetShaderDataFloat2(const char* name, float x, float y) override;
  virtual void SetShaderDataFloat3(const char* name, float x, float y, float z) override;
  virtual void SetShaderDataFloat3(const char* name, glm::vec3 vector) override;
  virtual void SetShaderDataFloat4(const char* name, float r, float g, float b, float a) override;
  virtual void SetShaderDataFloat4(const char* name, glm::vec4 vector) override;
  virtual void SetShaderDataMat4(const char* name, const glm::mat4& matrix) override;
  virtual void SetShaderDataMat3(const char* name, const glm::mat3& matrix) override;
  virtual void SetShaderDataBool(const char* name, bool value) override;

  virtual void RunComputeShader(uint32_t sizeX, uint32_t sizeY, uint32_t sizeZ) override;
  virtual void Barier(BarrierType type) override;

  virtual void Draw(DrawMode drawMode = DrawMode::Triangles) override;

  virtual void EnableBlending(BlendFactor source, BlendFactor destination) override;
  virtual void SetBlending(BlendFactor source, BlendFactor destination) override;
  virtual void DisableBlending() override;

  virtual void EnableDethTest(DepthTestFunction function) override;
  virtual void SetDethTestFunction(DepthTestFunction function) override;
  virtual void DisableDethTest() override;

  virtual void EnableFaceCulling() override;
  virtual void EnableFaceCulling(Face face) override;
  virtual void SetCullingFace(Face face) override;
  virtual void DisableFaceCulling() override;

  virtual void ClearDepthTarget() override;
  virtual void ClearColorTarget() override;
  virtual void SetClearColor(float r, float g, float b, float a) override;
  virtual void SetClearColor(glm::vec4 color) override;

  virtual void BeginUIFrame() override;
  virtual void EndUIFrame() override;

  virtual void Update() override;
  virtual void Present() override;

  virtual void Close() override;

  void AddResourceForUploading(void* data, uint32_t size, D3D12_RESOURCE_DESC* descriptor, D3D12Resource* resoruce);

  Microsoft::WRL::ComPtr<ID3D12Device> GetDevice() const;
protected:
  void CreateDevice();
  void CreateCommandQueue();
  void CreateSwapChain();
  void CreateCommandList();
  void SetupImGUI();

  void LogAdapterInformation();

protected:
  D3D12Window* m_Window;

  static const uint32_t BackBufferCount = 2; // TODO: think of adding triple buffering option

  Microsoft::WRL::ComPtr<IDXGIFactory6> m_Factory;
  Microsoft::WRL::ComPtr<IDXGIAdapter> m_Adapter;
  Microsoft::WRL::ComPtr<ID3D12Device> m_Device;
  Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue;
  Microsoft::WRL::ComPtr<IDXGISwapChain4> m_SwapChain;
  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RTVDescriptorHeap;
  Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_CVBSRVDescriptorHeap;
  Microsoft::WRL::ComPtr<ID3D12Resource1> m_BackBufferRenderTargetResource[BackBufferCount];
  Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
  Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList1> m_CommandList;

  Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CopyCommandQueue;
  Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList1> m_CopyCommandList;
  Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CopyCommandAllocator;

  Microsoft::WRL::ComPtr<ID3D12Resource1> uploadBuffer;

  struct UploadResourceDescription
  {
    void* Data;
    uint32_t Size;
    D3D12_RESOURCE_DESC* GPUResourceDescription;
    D3D12Resource* Resource;
  };

  std::vector<UploadResourceDescription> m_UploadResourceDescriptions;

  uint32_t m_RenderTargetDescriptorSize = 0;
};
