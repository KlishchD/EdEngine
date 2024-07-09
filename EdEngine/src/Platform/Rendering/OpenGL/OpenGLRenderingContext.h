#pragma once

#include "Core/Rendering/RenderingContex.h"
#include "Platform/Rendering/OpenGL/EdOpenGLRendering.h"

class OpenGLWindow;

class OpenGLRenderingContext : public RenderingContext
{
  static const inline uint8_t MaxUniformBufferLocations = 14;
public:
  OpenGLRenderingContext(OpenGLWindow* window);

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

  virtual ~OpenGLRenderingContext() override;


private:
  std::shared_ptr<VertexBuffer> m_VBO;
  std::shared_ptr<IndexBuffer> m_IBO;
  std::shared_ptr<UniformBuffer> m_UnifromBuffers[MaxUniformBufferLocations];

  std::shared_ptr<ShaderProgram> m_Program;
  int32_t m_ProgramID;

  int32_t m_LastTextureSlot = 0;
  const int32_t MaxTextureSlots = 16;

  struct GLFWwindow* m_Window;
};