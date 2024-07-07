#include "OpenGLWindow.h"
#include "OpenGLRenderingContext.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include "Helpers/PlatformHelper.h"

static void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	if (type == GL_DEBUG_TYPE_ERROR)
	{
		Window* window = (Window*)userParam;
		std::string str = std::string(message, length);
		ED_LOG(Rendering, err, "Window with title: {} type {}, severity {}, message = {}", window->GetTitle(), type, severity, str)
	}
}

OpenGLWindow::OpenGLWindow(WindowSpecification specification) : GLFWWindow(specification)
{

}

void OpenGLWindow::Initialize(WindowSpecification specification)
{
	ED_LOG(Window, info, "Stared creating window");

	ED_ASSERT(glfwInit() == GLFW_TRUE, "Failed to initialize GLFW");

	m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), NULL, NULL);

	if (!m_Window)
	{
		glfwTerminate();
		ED_ASSERT(0, "Failed to initialize GLFW")
	}

	glfwMakeContextCurrent(m_Window);

	if (glewInit() != GLEW_OK)
	{
		glfwTerminate();
		ED_ASSERT(0, "Failed to initialize GLEW")
	}

	GLFWWindow::Initialize(specification);

#if defined(DEBUG) || defined(_DEBUG)
	glEnable(GL_DEBUG_OUTPUT);
#endif

	glDebugMessageCallback(MessageCallback, this);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	ImGuiHelper::CreateImGuiAndSetUpContext();

	ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
	ImGui_ImplOpenGL3_Init("#version 450");

	m_Context = std::make_shared<OpenGLRenderingContext>(this);

	PlatformHelper::DisableTitleBar(*this);

	ED_LOG(Window, info, "Finished creating window")
}

void OpenGLWindow::Resize(int32_t width, int32_t height)
{
	glViewport(0, 0, width, height);
	GLFWWindow::Resize(width, height);
}

std::shared_ptr<RenderingContext> OpenGLWindow::GetContext()
{
	return m_Context;
}

void OpenGLWindow::Close()
{
	if (m_Window)
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	GLFWWindow::Close();
}

