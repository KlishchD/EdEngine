#include "OpenGLRenderingContext.h"
#include "OpenGLWindow.h"
#include "OpenGLShader.h"
#include "OpenGLShaderProgram.h"
#include "Core/Rendering/Framebuffer.h"
#include "Buffers/OpenGLVertexBuffer.h"
#include "Buffers/OpenGLIndexBuffer.h"
#include "Buffers/OpenGLUniformBuffer.h"
#include "Helpers/ImGuiHelper.h"
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>

static void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
  if (type == GL_DEBUG_TYPE_ERROR)
  {
    Window* window = (Window*)userParam;
    std::string str = std::string(message, length);
    ED_LOG(Rendering, err, "Window with title: {} type {}, severity {}, message = {}", window->GetTitle(), type, severity, str)
  }
}

void OpenGLRenderingContext::SetDefaultFramebuffer()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLRenderingContext::SetFramebuffer(std::shared_ptr<Framebuffer> framebuffer)
{
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->GetNativeResource<uint32_t>());
  glViewport(0, 0, framebuffer->GetWidth(), framebuffer->GetHeight());
}

void OpenGLRenderingContext::SetUniformBuffer(std::shared_ptr<UniformBuffer> buffer, uint32_t location)
{
  ED_ASSERT(location >= 0 && location < MaxUniformBufferLocations, "Invalid uniform binding location");

  m_UnifromBuffers[location] = buffer;

  glBindBufferRange(GL_UNIFORM_BUFFER, location, buffer->GetNativeResource<uint32_t>(), 0, buffer->GetSize());
}

void OpenGLRenderingContext::SetVertexBuffer(std::shared_ptr<VertexBuffer> buffer)
{
  if (m_VBO == buffer) return;

  if (m_VBO)
  {
    for (int32_t i = 0; i < m_VBO->GetLayout().GetElements().size(); ++i)
    {
      glDisableVertexAttribArray(i);
    }
  }

  m_VBO = buffer;

  glBindBuffer(GL_ARRAY_BUFFER, buffer->GetNativeResource<uint32_t>());

  uint32_t stride = 0;
  for (auto& element : buffer->GetLayout().GetElements())
  {
    stride += OpenGLTypes::ConvertShaderDataTypeSize(element.Type);
  }

  int32_t location = 0, offset = 0;
  for (const VertexBufferLayoutElement& element : buffer->GetLayout().GetElements())
  {
    glEnableVertexAttribArray(location);
    glVertexAttribPointer(location++, OpenGLTypes::ConvertShaderDataTypeCount(element.Type), OpenGLTypes::ConvertShaderDataTypeType(element.Type), element.Normalized, stride, (void*)offset);

    offset += OpenGLTypes::ConvertShaderDataTypeSize(element.Type);
  }
}

void OpenGLRenderingContext::SetIndexBuffer(std::shared_ptr<IndexBuffer> buffer)
{
  if (m_IBO == buffer) return;

  m_IBO = buffer;
  if (m_IBO)
  {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer->GetNativeResource<uint32_t>());
  }
  else
  {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  }
}

void OpenGLRenderingContext::SetShaderProgram(std::shared_ptr<ShaderProgram> program)
{
  if (m_Program == program) return;

  m_Program = program;
  m_ProgramID = program->GetNativeResource<uint32_t>();
  glUseProgram(m_ProgramID);
}

void OpenGLRenderingContext::SetShaderDataTexture(const std::string& name, std::shared_ptr<Texture> texture)
{
  glActiveTexture(GL_TEXTURE0 + m_LastTextureSlot);
  glBindTexture(OpenGLTypes::ConverTextureType(texture->GetTextureType()), texture->GetNativeResource<uint32_t>());

  const int32_t location = glGetUniformLocation(m_ProgramID, name.c_str());
  glUniform1i(location, m_LastTextureSlot);

  m_LastTextureSlot = (m_LastTextureSlot + 1) % MaxTextureSlots;
}

void OpenGLRenderingContext::SetShaderDataImage(const std::string& name, std::shared_ptr<Texture> texture)
{
  glBindImageTexture(m_LastTextureSlot, texture->GetNativeResource<uint32_t>(), 0, GL_FALSE, 0, GL_READ_WRITE, OpenGLTypes::ConvertPixelFormat(texture->GetPixelFormat()));
  m_LastTextureSlot = (m_LastTextureSlot + 1) % MaxTextureSlots;
}

void OpenGLRenderingContext::SetShaderDataInt(const std::string& name, int32_t value)
{
  const int32_t location = glGetUniformLocation(m_ProgramID, name.c_str());
  glUniform1i(location, value);
}

void OpenGLRenderingContext::SetShaderDataTexture(const char* name, std::shared_ptr<Texture> texture)
{
  glActiveTexture(GL_TEXTURE0 + m_LastTextureSlot);
  glBindTexture(OpenGLTypes::ConverTextureType(texture->GetTextureType()), texture->GetNativeResource<uint32_t>());

  const int32_t location = glGetUniformLocation(m_ProgramID, name);
  glUniform1i(location, m_LastTextureSlot);

  m_LastTextureSlot = (m_LastTextureSlot + 1) % MaxTextureSlots;
}

void OpenGLRenderingContext::SetShaderDataImage(const char* name, std::shared_ptr<Texture> texture)
{
  glBindImageTexture(0, texture->GetNativeResource<uint32_t>(), 0, GL_FALSE, 0, GL_READ_WRITE, OpenGLTypes::ConvertPixelFormat(texture->GetPixelFormat()));
}

void OpenGLRenderingContext::SetShaderDataInt(const char* name, int32_t value)
{
  const int32_t location = glGetUniformLocation(m_ProgramID, name);
  glUniform1i(location, value);
}

void OpenGLRenderingContext::SetShaderDataFloat(const char* name, float value)
{
  const int32_t location = glGetUniformLocation(m_ProgramID, name);
  glUniform1f(location, value);
}

void OpenGLRenderingContext::SetShaderDataFloat2(const char* name, glm::vec2 vector)
{
  const int32_t location = glGetUniformLocation(m_ProgramID, name);
  glUniform2f(location, vector.x, vector.y);
}

void OpenGLRenderingContext::SetShaderDataFloat2(const char* name, float x, float y)
{
  const int32_t location = glGetUniformLocation(m_ProgramID, name);
  glUniform2f(location, x, y);
}

void OpenGLRenderingContext::SetShaderDataFloat3(const char* name, float x, float y, float z)
{
  const int32_t location = glGetUniformLocation(m_ProgramID, name);
  glUniform3f(location, x, y, z);
}

void OpenGLRenderingContext::SetShaderDataFloat3(const char* name, glm::vec3 vector)
{
  const int32_t location = glGetUniformLocation(m_ProgramID, name);
  glUniform3f(location, vector.x, vector.y, vector.z);
}

void OpenGLRenderingContext::SetShaderDataFloat4(const char* name, float r, float g, float b, float a)
{
  const int32_t location = glGetUniformLocation(m_ProgramID, name);
  glUniform4f(location, r, g, b, a);
}

void OpenGLRenderingContext::SetShaderDataFloat4(const char* name, glm::vec4 vector)
{
  const int32_t location = glGetUniformLocation(m_ProgramID, name);
  glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
}

void OpenGLRenderingContext::SetShaderDataMat4(const char* name, const glm::mat4& matrix)
{
  const int32_t location = glGetUniformLocation(m_ProgramID, name);
  glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void OpenGLRenderingContext::SetShaderDataMat3(const char* name, const glm::mat3& matrix)
{
  const int32_t location = glGetUniformLocation(m_ProgramID, name);
  glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void OpenGLRenderingContext::SetShaderDataBool(const char* name, bool value)
{
  const int32_t location = glGetUniformLocation(m_ProgramID, name);
  glUniform1i(location, value);
}

void OpenGLRenderingContext::RunComputeShader(uint32_t sizeX, uint32_t sizeY, uint32_t sizeZ)
{
  glDispatchCompute(sizeX, sizeY, sizeZ);
}

void OpenGLRenderingContext::Barier(BarrierType type)
{
  glMemoryBarrier(OpenGLTypes::ConvertBarrierType(type));
}

void OpenGLRenderingContext::Draw(DrawMode drawMode)
{
  int32_t mode = OpenGLTypes::ConvertDrawMode(drawMode);

  if (m_IBO)
  {
    int32_t count = m_IBO->GetCount();
    glDrawElements(mode, count, GL_UNSIGNED_INT, nullptr);
  }
  else
  {
    glDrawArrays(mode, 0, m_VBO->GetCount());
  }
}

void OpenGLRenderingContext::EnableBlending(BlendFactor source, BlendFactor destination)
{
  glEnable(GL_BLEND);
  glBlendFunc(OpenGLTypes::ConvertBlendFactor(source), OpenGLTypes::ConvertBlendFactor(destination));
}

void OpenGLRenderingContext::SetBlending(BlendFactor source, BlendFactor destination)
{
  glBlendFunc(OpenGLTypes::ConvertBlendFactor(source), OpenGLTypes::ConvertBlendFactor(destination));
}

void OpenGLRenderingContext::DisableBlending()
{
  glDisable(GL_BLEND);
}

void OpenGLRenderingContext::EnableDethTest(DepthTestFunction function)
{
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(OpenGLTypes::ConvertDepthTestFunction(function));
}

void OpenGLRenderingContext::SetDethTestFunction(DepthTestFunction function)
{
  glDepthFunc(OpenGLTypes::ConvertDepthTestFunction(function));
}

void OpenGLRenderingContext::DisableDethTest()
{
  glDisable(GL_DEPTH_TEST);
}

void OpenGLRenderingContext::EnableFaceCulling()
{
  glEnable(GL_CULL_FACE);
}

void OpenGLRenderingContext::EnableFaceCulling(Face face)
{
  glEnable(GL_CULL_FACE);
  glCullFace(OpenGLTypes::ConvertFace(face));
}

void OpenGLRenderingContext::SetCullingFace(Face face)
{
  glCullFace(OpenGLTypes::ConvertFace(face));
}

void OpenGLRenderingContext::DisableFaceCulling()
{
  glDisable(GL_CULL_FACE);
}

void OpenGLRenderingContext::ClearDepthTarget()
{
  glClear(GL_DEPTH_BUFFER_BIT);
}

void OpenGLRenderingContext::ClearColorTarget()
{
  glClear(GL_COLOR_BUFFER_BIT);
}

void OpenGLRenderingContext::SetClearColor(float r, float g, float b, float a)
{
  glClearColor(r, g, b, a);
}

void OpenGLRenderingContext::SetClearColor(glm::vec4 color)
{
  glClearColor(color.r, color.g, color.b, color.a);
}

void OpenGLRenderingContext::BeginUIFrame()
{
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();
  ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
}

void OpenGLRenderingContext::EndUIFrame()
{
  ImGui::Render();

  ImGuiIO& io = ImGui::GetIO();
  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
  {
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    glfwMakeContextCurrent(m_Window);
  }

  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void OpenGLRenderingContext::Present()
{
  glfwSwapBuffers(m_Window);
}

void OpenGLRenderingContext::Close()
{
  if (m_Window)
  {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
  }
}

OpenGLRenderingContext::OpenGLRenderingContext(OpenGLWindow* window)
{
  ED_LOG(OpenGLRenderingContext, info, "Started creating rendering context");

  m_Window = (GLFWwindow*)window->GetNativeWindow();

  if (glewInit() != GLEW_OK)
  {
    glfwTerminate();
    ED_ASSERT(0, "Failed to initialize GLEW")
  }

#if defined(DEBUG) || defined(_DEBUG)
  glEnable(GL_DEBUG_OUTPUT);
#endif

  glDebugMessageCallback(MessageCallback, this);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  ImGuiHelper::CreateImGuiAndSetUpContext();

  ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
  ImGui_ImplOpenGL3_Init("#version 450");

  ED_LOG(OpenGLRenderingContext, info, "Finished creating rendering context");
}

OpenGLRenderingContext::~OpenGLRenderingContext()
{

}

void OpenGLRenderingContext::AddResourceForUploading(void* data, uint32_t size, uint32_t offset, void* descriptor, Resource* resource)
{
  ED_LOG(OpenGLRenderingContext, warn, "AddResourceForUploading is not supported for OpenGL");
}

void OpenGLRenderingContext::RetainUploadBuffer(UploadBuffer&& buffer)
{
  ED_LOG(OpenGLRenderingContext, warn, "RetainUploadBuffer is not supported for OpenGL");
}

void OpenGLRenderingContext::Update()
{

}

void OpenGLRenderingContext::SetShaderDataFloat(const std::string& name, float value)
{
  const int32_t location = glGetUniformLocation(m_ProgramID, name.c_str());
  glUniform1f(location, value);
}

void OpenGLRenderingContext::SetShaderDataFloat2(const std::string& name, glm::vec2 vector)
{
  const int32_t location = glGetUniformLocation(m_ProgramID, name.c_str());
  glUniform2f(location, vector.x, vector.y);
}

void OpenGLRenderingContext::SetShaderDataFloat2(const std::string& name, float x, float y)
{
  const int32_t location = glGetUniformLocation(m_ProgramID, name.c_str());
  glUniform2f(location, x, y);
}

void OpenGLRenderingContext::SetShaderDataFloat3(const std::string& name, float x, float y, float z)
{
  const int32_t location = glGetUniformLocation(m_ProgramID, name.c_str());
  glUniform3f(location, x, y, z);
}

void OpenGLRenderingContext::SetShaderDataFloat3(const std::string& name, glm::vec3 vector)
{
  const int32_t location = glGetUniformLocation(m_ProgramID, name.c_str());
  glUniform3f(location, vector.x, vector.y, vector.z);
}

void OpenGLRenderingContext::SetShaderDataFloat4(const std::string& name, float r, float g, float b, float a)
{
  const int32_t location = glGetUniformLocation(m_ProgramID, name.c_str());
  glUniform4f(location, r, g, b, a);
}

void OpenGLRenderingContext::SetShaderDataFloat4(const std::string& name, glm::vec4 vector)
{
  const int32_t location = glGetUniformLocation(m_ProgramID, name.c_str());
  glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
}

void OpenGLRenderingContext::SetShaderDataMat4(const std::string& name, const glm::mat4& matrix)
{
  const int32_t location = glGetUniformLocation(m_ProgramID, name.c_str());
  glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void OpenGLRenderingContext::SetShaderDataMat3(const std::string& name, const glm::mat3& matrix)
{
  const int32_t location = glGetUniformLocation(m_ProgramID, name.c_str());
  glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void OpenGLRenderingContext::SetShaderDataBool(const std::string& name, bool value)
{
  const int32_t location = glGetUniformLocation(m_ProgramID, name.c_str());
  glUniform1i(location, value);
}
