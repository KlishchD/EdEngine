#include "EdOpenGL.h"
#include "RenderingContex.h"

#include "GLFW/glfw3.h"

#include "Framebuffer.h"
#include "Buffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Window.h"

#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

constexpr i32 MaxTextureSlots = 16;

static GLFWwindow* m_Window = nullptr;
static u32 m_ShaderHandle = 0;
static i32 m_LastTextureSlot = 0;

static OpenGLNativeHandlesPool<1000, OpenGLHandleType::Texture2D>* Texture2DHandles;
static OpenGLNativeHandlesPool<20, OpenGLHandleType::TextureArray>* TextureArrayHandles;
static OpenGLNativeHandlesPool<1000, OpenGLHandleType::Buffer>* BufferHandles;
static OpenGLNativeHandlesPool<20, OpenGLHandleType::Sampler>* SamplerHandles;
static OpenGLNativeHandlesPool<20, OpenGLHandleType::Framebuffer>* FramebufferHandles;

uptr GetNextHandle(OpenGLHandleType type)
{
	switch (type)
	{
	case OpenGLHandleType::Texture2D: return Texture2DHandles->GetNextHandle();
	case OpenGLHandleType::TextureArray: return TextureArrayHandles->GetNextHandle();
	case OpenGLHandleType::Buffer: return BufferHandles->GetNextHandle();
	case OpenGLHandleType::Sampler: return SamplerHandles->GetNextHandle();
	case OpenGLHandleType::Framebuffer: return FramebufferHandles->GetNextHandle();
	default:
		ED_ASSERT(0, "Attempted to get next handle of unsupported type.");
	}

	return 0;
}

void ReturnHandle(OpenGLHandleType type, uptr handle)
{
    switch (type)
    {
    case OpenGLHandleType::Texture2D: Texture2DHandles->ReturnHandle(handle); break;
    case OpenGLHandleType::TextureArray: TextureArrayHandles->ReturnHandle(handle); break;
    case OpenGLHandleType::Buffer: BufferHandles->ReturnHandle(handle); break;
    case OpenGLHandleType::Sampler: SamplerHandles->ReturnHandle(handle); break;
    case OpenGLHandleType::Framebuffer: FramebufferHandles->ReturnHandle(handle); break;
    default:
        ED_ASSERT(0, "Attempted to return handle of unsupported type.");
    }
}

static void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    if (type == GL_DEBUG_TYPE_ERROR)
    {
        Window* window = (Window*)userParam;
        std::string str = std::string(message, length);
        ED_LOG(Rendering, err, "Window with title: {} type {}, severity {}, message = {}", window->GetTitle(), type, severity, str)
    }
}

void RenderingContext::SetDefaultFramebuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderingContext::SetFramebuffer(Framebuffer* framebuffer)
{
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->GetNativeHandle());
	glViewport(0, 0, framebuffer->GetWidth(), framebuffer->GetHeight()); // TODO: Extract this ;)
}

void RenderingContext::SetVertexBufferLayout(const VertexBufferLayout& layout)
{
	ED_ASSERT(m_VertexBuffer, "Can not set layout without vertex buffer set");

    for (i32 i = 0; i < m_Layout.GetElements().size(); ++i)
    {
        glDisableVertexAttribArray(i);
    }

	m_Layout = layout;

    u32 stride = 0;
    for (auto& element : m_Layout.GetElements())
    {
        stride += RenderTypes::ConvertShaderDataTypeSize(element.Type);
    }

    u64 location = 0, offset = 0;
    for (const VertexBufferLayoutElement& element : m_Layout.GetElements())
    {
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location++, RenderTypes::ConvertShaderDataTypeCount(element.Type), OpenGLTypes::ConvertShaderDataTypeType(element.Type), element.Normalized, stride, (void*)offset);

        offset += RenderTypes::ConvertShaderDataTypeSize(element.Type);
    }
}
void RenderingContext::SetBuffer(BufferType type, Buffer* buffer)
{
	switch (type)
	{
	case BufferType::Vertex:
	{
		ED_ASSERT(buffer, "Can not set nullptr buffer");

		if (m_VertexBuffer != buffer)
		{
			m_VertexBuffer = buffer;
			glBindBuffer(GL_ARRAY_BUFFER, buffer->GetNativeHandle());
		}
		break;
	}
	case BufferType::Index:
	{
		if (m_IndexBuffer != buffer)
		{
			m_IndexBuffer = buffer;
			u32 handle = buffer ? buffer->GetNativeHandle() : 0;
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle);
		}
		break;
	}
	case BufferType::Uniform:
	{
		/*
		ED_ASSERT(location >= 0 && location < MaxUniformBufferLocations, "Invalid uniform binding location");

		m_UnifromBuffers[location] = buffer;

		std::shared_ptr<OpenGLUniformBuffer> castedBuffer = std::static_pointer_cast<OpenGLUniformBuffer>(buffer);
		glBindBufferRange(GL_UNIFORM_BUFFER, location, castedBuffer->GetID(), 0, castedBuffer->GetSize());
		*/

		ED_ASSERT_CONTEXT(OpenGLAPI, 0, "Uniform buffer is not supported at the moment.");

		break;
	}
	default:
		ED_ASSERT_CONTEXT(OpenGLAPI, 0, "Attempted to set unsupported buffer type.");
	}
}

void RenderingContext::SetShader(Shader* shader)
{
	ED_ASSERT(shader, "Can not set nullprt shader.");

	if (m_Shader == shader) return;

	m_Shader = shader;
	m_ShaderHandle = m_Shader->GetNativeHandle();
	
	glUseProgram(m_ShaderHandle);
}

void RenderingContext::SetShaderDataTexture(const c8* name, Texture* texture)
{
	glActiveTexture(GL_TEXTURE0 + m_LastTextureSlot);
	glBindTexture(OpenGLTypes::ConvertTextureType(texture->GetType()), texture->GetNativeHandle());

	const i32 location = glGetUniformLocation(m_ShaderHandle, name);
	glUniform1i(location, m_LastTextureSlot);
	
	m_LastTextureSlot = (m_LastTextureSlot + 1) % MaxTextureSlots;
}

void RenderingContext::SetShaderDataImage(const c8* name, Texture* texture)
{
 	glBindImageTexture(0, texture->GetNativeHandle(), 0, GL_FALSE, 0, GL_READ_WRITE, OpenGLTypes::ConvertPixelFormat(texture->GetFormat()));
}

void RenderingContext::SetShaderDataInt(const c8* name, i32 value)
{
	const i32 location = glGetUniformLocation(m_ShaderHandle, name);
	glUniform1i(location, value);
}

void RenderingContext::SetShaderDataFloat(const c8* name, f32 value)
{
	const i32 location = glGetUniformLocation(m_ShaderHandle, name);
	glUniform1f(location, value);
}

void RenderingContext::SetShaderDataFloat2(const c8* name, glm::vec2 vector)
{
	const i32 location = glGetUniformLocation(m_ShaderHandle, name);
	glUniform2f(location, vector.x, vector.y);
}

void RenderingContext::SetShaderDataFloat2(const c8* name, f32 x, f32 y)
{
	const i32 location = glGetUniformLocation(m_ShaderHandle, name);
	glUniform2f(location, x, y);
}

void RenderingContext::SetShaderDataFloat3(const c8* name, f32 x, f32 y, f32 z)
{
	const i32 location = glGetUniformLocation(m_ShaderHandle, name);
	glUniform3f(location, x, y, z);
}

void RenderingContext::SetShaderDataFloat3(const c8* name, glm::vec3 vector)
{
	const i32 location = glGetUniformLocation(m_ShaderHandle, name);
	glUniform3f(location, vector.x, vector.y, vector.z);
}

void RenderingContext::SetShaderDataFloat4(const c8* name, f32 r, f32 g, f32 b, f32 a)
{
	const i32 location = glGetUniformLocation(m_ShaderHandle, name);
	glUniform4f(location, r, g, b, a);
}

void RenderingContext::SetShaderDataFloat4(const c8* name, glm::vec4 vector)
{
	const i32 location = glGetUniformLocation(m_ShaderHandle, name);
	glUniform4f(location, vector.x, vector.y, vector.z, vector.w);
}

void RenderingContext::SetShaderDataMat4(const c8* name, const glm::mat4& matrix)
{
	const i32 location = glGetUniformLocation(m_ShaderHandle, name);
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void RenderingContext::SetShaderDataMat3(const c8* name, const glm::mat3& matrix)
{
	const i32 location = glGetUniformLocation(m_ShaderHandle, name);
	glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void RenderingContext::SetShaderDataBool(const c8* name, bool value)
{
	const i32 location = glGetUniformLocation(m_ShaderHandle, name);
	glUniform1i(location, value);
}

void RenderingContext::RunComputeShader(u32 sizeX, u32 sizeY, u32 sizeZ)
{
	glDispatchCompute(sizeX, sizeY, sizeZ);
}

void RenderingContext::Barier(BarrierType type)
{
	glMemoryBarrier(OpenGLTypes::ConvertBarrierType(type));
}

void RenderingContext::Draw(DrawMode drawMode)
{
	i32 mode = OpenGLTypes::ConvertDrawMode(drawMode);

	if (m_IndexBuffer) 
	{
		i32 count = m_IndexBuffer->GetSize() / sizeof(u32);
		glDrawElements(mode, count, GL_UNSIGNED_INT, nullptr);
	}
	else
	{
		u32 size = m_Layout.GetSize();

		ED_ASSERT(size, "Can not draw, vertex layout was not set or was set incorrect one.");

		u32 count = m_VertexBuffer->GetSize() / size;
		glDrawArrays(mode, 0, count);
	}
}

void RenderingContext::EnableBlending(BlendFactor source, BlendFactor destination)
{
	glEnable(GL_BLEND);
	glBlendFunc(OpenGLTypes::ConvertBlendFactor(source), OpenGLTypes::ConvertBlendFactor(destination));
}

void RenderingContext::SetBlending(BlendFactor source, BlendFactor destination)
{
	glBlendFunc(OpenGLTypes::ConvertBlendFactor(source), OpenGLTypes::ConvertBlendFactor(destination));
}

void RenderingContext::DisableBlending()
{
	glDisable(GL_BLEND);
}

void RenderingContext::EnableDethTest(DepthTestFunction function)
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(OpenGLTypes::ConvertDepthTestFunction(function));
}

void RenderingContext::SetDethTestFunction(DepthTestFunction function)
{
	glDepthFunc(OpenGLTypes::ConvertDepthTestFunction(function));
}

void RenderingContext::DisableDethTest()
{
	glDisable(GL_DEPTH_TEST);
}

void RenderingContext::EnableFaceCulling()
{
	glEnable(GL_CULL_FACE);
}

void RenderingContext::EnableFaceCulling(Face face)
{
	glEnable(GL_CULL_FACE);
	glCullFace(OpenGLTypes::ConvertFace(face));
}

void RenderingContext::SetCullingFace(Face face)
{
	glCullFace(OpenGLTypes::ConvertFace(face));
}

void RenderingContext::DisableFaceCulling()
{
	glDisable(GL_CULL_FACE);
}

void RenderingContext::ClearDepthTarget()
{
	glClear(GL_DEPTH_BUFFER_BIT);
}

void RenderingContext::ClearColorTarget()
{
	glClear(GL_COLOR_BUFFER_BIT);
}

void RenderingContext::SetClearColor(f32 r, f32 g, f32 b, f32 a)
{
	glClearColor(r, g, b, a);
}

void RenderingContext::SetClearColor(glm::vec4 color)
{
	glClearColor(color.r, color.g, color.b, color.a);
}

void RenderingContext::CreateImGuiContext()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
    ImGui_ImplOpenGL3_Init("#version 460");
}

void RenderingContext::DeleteImGuiContext()
{
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
}

void RenderingContext::BeginUIFrame()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());
}

void RenderingContext::EndUIFrame()
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

void RenderingContext::Present()
{
	glfwSwapBuffers(m_Window);
}

void RenderingContext::Initialize(Window* window)
{
	ED_ASSERT_CONTEXT(OpenGLAPI, window, "Can not set nullptr window.");
	m_Window = (GLFWwindow*)window->GetNativeWindow();

    if (glewInit() != GLEW_OK)
    {
        glfwTerminate();
        ED_ASSERT(0, "Failed to initialize GLEW")
    }

#ifdef DEBUG_BUILD
    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, this);
#endif

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	Texture2DHandles = new OpenGLNativeHandlesPool<1000, OpenGLHandleType::Texture2D>();
	Texture2DHandles->Initialize();
	TextureArrayHandles = new OpenGLNativeHandlesPool<20, OpenGLHandleType::TextureArray>();
	TextureArrayHandles->Initialize();
	BufferHandles = new OpenGLNativeHandlesPool<1000, OpenGLHandleType::Buffer>();
	BufferHandles->Initialize();
	SamplerHandles = new OpenGLNativeHandlesPool<20, OpenGLHandleType::Sampler>();
	SamplerHandles->Initialize();
	FramebufferHandles = new OpenGLNativeHandlesPool<20, OpenGLHandleType::Framebuffer>();
	FramebufferHandles->Initialize();
}

RenderingContext::~RenderingContext()
{
	DeallocateResources();
	s_Context = nullptr;

	delete Texture2DHandles;
	delete TextureArrayHandles;
	delete BufferHandles;
	delete SamplerHandles;
	delete FramebufferHandles;
}