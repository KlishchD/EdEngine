#include "OpenGLRenderingContex.h"
#include "Core/Rendering/Framebuffers/Framebuffer.h"
#include "Core/Rendering/Framebuffers/CubeFramebuffer.h"
#include "Core/Ed.h"
#include "Core/Macros.h"
#include "Buffers/OpenGLVertexBuffer.h"
#include "Buffers/OpenGLIndexBuffer.h"
#include "OpenGLTypes.h"
#include "OpenGLShader.h"
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>
#include "Core/Window.h"

void OpenGLRenderingContext::SetDefaultFramebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLRenderingContext::SetFramebuffer(const std::shared_ptr<BaseFramebuffer>& framebuffer)
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->GetID());
	glViewport(0, 0, framebuffer->GetWidth(), framebuffer->GetHeight());
}

void OpenGLRenderingContext::SetVertexBuffer(const std::shared_ptr<class VertexBuffer>& buffer)
{
	m_VBO = buffer;

	glBindBuffer(GL_ARRAY_BUFFER, std::static_pointer_cast<OpenGLVertexBuffer>(buffer)->GetID());

	uint32_t stride = 0;
	for (auto& element : buffer->GetLayout().GetElements()) 
	{
		stride += OpenGLTypes::ConvertShaderDataTypeSize(element.Type);
	}

	for (int32_t i = 0; i < 10; ++i) 
	{
		glDisableVertexAttribArray(i);
	}

	int32_t location = 0, offset = 0;
	for (const VertexBufferLayoutElement& element : buffer->GetLayout().GetElements()) 
	{
		glEnableVertexAttribArray(location);
		glVertexAttribPointer(location++, OpenGLTypes::ConvertShaderDataTypeCount(element.Type), OpenGLTypes::ConvertShaderDataTypeType(element.Type), element.Normalized, stride, (void*)offset);
	
		offset += OpenGLTypes::ConvertShaderDataTypeSize(element.Type);
	}
}

void OpenGLRenderingContext::SetIndexBuffer(const std::shared_ptr<class IndexBuffer>& buffer)
{
	m_IBO = buffer;
	if (m_IBO)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, std::static_pointer_cast<OpenGLIndexBuffer>(buffer)->GetID());
	}
	else
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

void OpenGLRenderingContext::SetShader(const std::shared_ptr<Shader>& shader)
{
	m_Shader = shader;
	m_ShaderID = std::static_pointer_cast<OpenGLShader>(shader)->GetID();
	glUseProgram(m_ShaderID);
}

void OpenGLRenderingContext::SetShaderDataTexture(const std::string& name, const std::shared_ptr<Texture>& texture)
{
	glActiveTexture(GL_TEXTURE0 + m_LastTextureSlot);
	glBindTexture(OpenGLTypes::ConverTextureType(texture->GetType()), texture->GetID());

	const int32_t location = glGetUniformLocation(m_ShaderID, name.c_str());
	glUniform1i(location, m_LastTextureSlot);
	
	m_LastTextureSlot = (m_LastTextureSlot + 1) % MaxTextureSlots; // > 1
}

void OpenGLRenderingContext::SetShaderDataInt(const std::string& name, int32_t value)
{
	const int32_t location = glGetUniformLocation(m_ShaderID, name.c_str());
	glUniform1i(location, value);
}

void OpenGLRenderingContext::SetShaderDataTexture(const char* name, const std::shared_ptr<Texture>& texture)
{
	glActiveTexture(GL_TEXTURE0 + m_LastTextureSlot);
	glBindTexture(OpenGLTypes::ConverTextureType(texture->GetType()), texture->GetID());

	const int32_t location = glGetUniformLocation(m_ShaderID, name);
	glUniform1i(location, m_LastTextureSlot);
	
	m_LastTextureSlot = (m_LastTextureSlot + 1) % MaxTextureSlots; // > 1
}

void OpenGLRenderingContext::SetShaderDataInt(const char* name, int32_t value)
{
	const int32_t location = glGetUniformLocation(m_ShaderID, name);
	glUniform1i(location, value);
}

void OpenGLRenderingContext::SetShaderDataFloat(const char* name, float value)
{
	const int32_t location = glGetUniformLocation(m_ShaderID, name);
	glUniform1f(location, value);
}

void OpenGLRenderingContext::SetShaderDataFloat2(const char* name, glm::vec2 vector)
{
	const int32_t location = glGetUniformLocation(m_ShaderID, name);
	glUniform2f(location, vector.x, vector.y);
}

void OpenGLRenderingContext::SetShaderDataFloat2(const char* name, float x, float y)
{
	const int32_t location = glGetUniformLocation(m_ShaderID, name);
	glUniform2f(location, x, y);
}

void OpenGLRenderingContext::SetShaderDataFloat3(const char* name, float x, float y, float z)
{
	const int32_t location = glGetUniformLocation(m_ShaderID, name);
	glUniform3f(location, x, y, z);
}

void OpenGLRenderingContext::SetShaderDataFloat3(const char* name, glm::vec3 vector)
{
	const int32_t location = glGetUniformLocation(m_ShaderID, name);
	glUniform3f(location, vector.x, vector.y, vector.z);
}

void OpenGLRenderingContext::SetShaderDataFloat4(const char* name, float r, float g, float b, float a)
{
	const int32_t location = glGetUniformLocation(m_ShaderID, name);
	glUniform4f(location, r, g, b, a);
}

void OpenGLRenderingContext::SetShaderDataFloat4(const char* name, glm::vec4 vector)
{
	const int32_t location = glGetUniformLocation(m_ShaderID, name);
	glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
}

void OpenGLRenderingContext::SetShaderDataMat4(const char* name, const glm::mat4& matrix)
{
	const int32_t location = glGetUniformLocation(m_ShaderID, name);
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void OpenGLRenderingContext::SetShaderDataMat3(const char* name, const glm::mat3& matrix)
{
	const int32_t location = glGetUniformLocation(m_ShaderID, name);
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void OpenGLRenderingContext::SetShaderDataBool(const char* name, bool value)
{
	const int32_t location = glGetUniformLocation(m_ShaderID, name);
	glUniform1i(location, value);
}

void OpenGLRenderingContext::Draw()
{
	if (m_IBO) 
	{
		int32_t count = m_IBO->GetCount();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}
	else
	{
		glDrawArrays(GL_TRIANGLES, 0, m_VBO->GetCount());
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

void OpenGLRenderingContext::SwapBuffers()
{
	glfwSwapBuffers(m_Window);
}

OpenGLRenderingContext::OpenGLRenderingContext(Window* window)
{
	ED_LOG(OpenGLRenderingContext, info, "Started creating rendering context");

	m_Window = (GLFWwindow*) window->GetNativeWindow();

	ED_LOG(OpenGLRenderingContext, info, "Finished creating rendering context");
}

OpenGLRenderingContext::~OpenGLRenderingContext()
{

}

void OpenGLRenderingContext::SetShaderDataFloat(const std::string& name, float value)
{
	const int32_t location = glGetUniformLocation(m_ShaderID, name.c_str());
	glUniform1f(location, value);
}

void OpenGLRenderingContext::SetShaderDataFloat2(const std::string& name, glm::vec2 vector)
{
	const int32_t location = glGetUniformLocation(m_ShaderID, name.c_str());
	glUniform2f(location, vector.x, vector.y);
}

void OpenGLRenderingContext::SetShaderDataFloat2(const std::string& name, float x, float y)
{
	const int32_t location = glGetUniformLocation(m_ShaderID, name.c_str());
	glUniform2f(location, x, y);
}

void OpenGLRenderingContext::SetShaderDataFloat3(const std::string& name, float x, float y, float z)
{
	const int32_t location = glGetUniformLocation(m_ShaderID, name.c_str());
	glUniform3f(location, x, y, z);
}

void OpenGLRenderingContext::SetShaderDataFloat3(const std::string& name, glm::vec3 vector)
{
	const int32_t location = glGetUniformLocation(m_ShaderID, name.c_str());
	glUniform3f(location, vector.x, vector.y, vector.z);
}

void OpenGLRenderingContext::SetShaderDataFloat4(const std::string& name, float r, float g, float b, float a)
{
	const int32_t location = glGetUniformLocation(m_ShaderID, name.c_str());
	glUniform4f(location, r, g, b, a);
}

void OpenGLRenderingContext::SetShaderDataFloat4(const std::string& name, glm::vec4 vector)
{
	const int32_t location = glGetUniformLocation(m_ShaderID, name.c_str());
	glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
}

void OpenGLRenderingContext::SetShaderDataMat4(const std::string& name, const glm::mat4& matrix)
{
	const int32_t location = glGetUniformLocation(m_ShaderID, name.c_str());
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void OpenGLRenderingContext::SetShaderDataMat3(const std::string& name, const glm::mat3& matrix)
{
	const int32_t location = glGetUniformLocation(m_ShaderID, name.c_str());
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void OpenGLRenderingContext::SetShaderDataBool(const std::string& name, bool value)
{
	const int32_t location = glGetUniformLocation(m_ShaderID, name.c_str());
	glUniform1i(location, value);
}
