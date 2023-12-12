#include "OpenGLWindow.h"
#include "Core/Ed.h"
#include "imgui.h"
#include "Core/Macros.h"
#include "OpenGLRenderingContex.h"
#include "Core/Engine.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

std::vector<Window*> s_Windows;

static void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
	if (type == GL_DEBUG_TYPE_ERROR)
	{
		Window* window = (Window*)userParam;
		std::string str = std::string(message, length);
		ED_LOG(Rendering, err, "Window with title: {} type {}, severity {}, message = {}", window->GetTitle(), type, severity, str)
	}
}

OpenGLWindow::OpenGLWindow(WindowSpecification specification): Window(specification)
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

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	s_Windows.push_back(this);

	glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* inWindow, int width, int height)
	{
		for (Window* window : s_Windows)
		{
			if (window->GetNativeWindow() == inWindow)
			{
				window->Resize(width, height);
			}
		}
	});

	glfwSetKeyCallback(m_Window, [](GLFWwindow* inWindow, int32_t key, int32_t scancode, int32_t action, int32_t mods)
	{
		Engine::Get().InputAction(Input::ConvertGLFWInputKey(key), Input::ConvertGLFWInputAction(action));
		ED_LOG(Input, info, "Keyboard key {} action {}", key, action)
	});

	glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* inWindow, int button, int action, int mods)
	{
		Engine::Get().InputAction(Input::ConvertGLFWInputKey(button), Input::ConvertGLFWInputAction(action));
		ED_LOG(Input, info, "Mouse button {} action {}", button, action)
	});

	// During init, enable debug output
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, this);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;

	ImGui::StyleColorsClassic();

	ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
	ImGui_ImplOpenGL3_Init("#version 460 core");

	m_Context = std::make_shared<OpenGLRenderingContext>(this);

	printf("%s\n", glGetString(GL_VERSION));

	ED_LOG(Window, info, "Finished creating window")
}

void OpenGLWindow::Update()
{
	glfwPollEvents();
}

bool OpenGLWindow::IsRunning()
{
	return !glfwWindowShouldClose(m_Window);
}

void OpenGLWindow::Resize(int32_t width, int32_t height)
{
	glViewport(0, 0, width, height);
	m_Width = width;
	m_Height = height;

	ED_LOG(Window, info, "Window is resized to {}x{}", width, height) // TODO: Add resize for all screen size framebuffers :)
}

glm::vec2 OpenGLWindow::GetMousePosition()
{
	glm::dvec2 position;
	glfwGetCursorPos(m_Window, &position.x, &position.y);
	return position;
}

glm::vec2 OpenGLWindow::GetMousePositionNormalized()
{
	glm::dvec2 position;
	glfwGetCursorPos(m_Window, &position.x, &position.y);
	return { position.x / m_Width, position.y / m_Height };
}

void* OpenGLWindow::GetNativeWindow()
{
	return m_Window;
}

std::shared_ptr<RenderingContext> OpenGLWindow::GetContext()
{
	return m_Context;
}

void OpenGLWindow::Close()
{
	if (m_Window)
	{
		glfwTerminate();

		ImGui_ImplGlfw_Shutdown();
		ImGui_ImplOpenGL3_Shutdown();
		ImGui::DestroyContext();

		m_Window = nullptr;

		ED_LOG(Window, info, "Window is closed")
	}
}

OpenGLWindow::~OpenGLWindow()
{
	int32_t index = -1;
	for (int32_t i = 0; i < s_Windows.size(); ++i)
	{
		if (s_Windows[i] == this)
		{
			index = i;
		}
	}

	if (index != -1)
	{
		s_Windows.erase(s_Windows.begin() + index);
	}

	Close();
}
