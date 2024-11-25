#include "OpenGLWindow.h"
#include "Core/Rendering/EdRendering.h"
#include "imgui.h"
#include "Core/Macros.h"
#include "OpenGLRenderingContext.h"
#include "Core/Engine.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include "Helpers/PlatformHelper.h"
#include "Helpers/FilesHelper.h"

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
		Engine::Get().RecieveInputAction(Input::ConvertGLFWInputKey(key), Input::ConvertGLFWInputAction(action));
		ED_LOG(Input, info, "Keyboard key {} action {}", key, action)
	});

	glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* inWindow, int button, int action, int mods)
	{
		Engine::Get().RecieveInputAction(Input::ConvertGLFWInputKey(button), Input::ConvertGLFWInputAction(action));
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
	ImGui_ImplOpenGL3_Init("#version 460");

	m_Context = std::make_shared<OpenGLRenderingContext>(this);

	ImVec4* colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg] = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
	colors[ImGuiCol_Border] = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
	colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
	colors[ImGuiCol_FrameBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
	colors[ImGuiCol_FrameBgActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TitleBgActive] = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
	colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
	colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
	colors[ImGuiCol_CheckMark] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
	colors[ImGuiCol_SliderGrab] = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
	colors[ImGuiCol_SliderGrabActive] = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
	colors[ImGuiCol_Button] = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_ButtonHovered] = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
	colors[ImGuiCol_ButtonActive] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_Header] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
	colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
	colors[ImGuiCol_Separator] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_SeparatorHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
	colors[ImGuiCol_SeparatorActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
	colors[ImGuiCol_ResizeGrip] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
	colors[ImGuiCol_ResizeGripActive] = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
	colors[ImGuiCol_Tab] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TabHovered] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
	colors[ImGuiCol_TabUnfocused] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_DockingPreview] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
	colors[ImGuiCol_DockingEmptyBg] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotLines] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogram] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TableBorderStrong] = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TableBorderLight] = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_TextSelectedBg] = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_DragDropTarget] = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
	colors[ImGuiCol_NavHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg] = ImVec4(1.00f, 0.00f, 0.00f, 0.35f);

	ImGuiStyle& style = ImGui::GetStyle();
	style.WindowPadding = ImVec2(8.00f, 8.00f);
	style.FramePadding = ImVec2(5.00f, 2.00f);
	style.CellPadding = ImVec2(6.00f, 6.00f);
	style.ItemSpacing = ImVec2(6.00f, 6.00f);
	style.ItemInnerSpacing = ImVec2(6.00f, 6.00f);
	style.TouchExtraPadding = ImVec2(0.00f, 0.00f);
	style.IndentSpacing = 25;
	style.ScrollbarSize = 15;
	style.GrabMinSize = 10;
	style.WindowBorderSize = 1;
	style.ChildBorderSize = 1;
	style.PopupBorderSize = 1;
	style.FrameBorderSize = 1;
	style.TabBorderSize = 1;
	style.WindowRounding = 7;
	style.ChildRounding = 4;
	style.FrameRounding = 3;
	style.PopupRounding = 4;
	style.ScrollbarRounding = 9;
	style.GrabRounding = 3;
	style.LogSliderDeadzone = 4;
	style.TabRounding = 4;

	PlatformHelper::DisableTitleBar(*this);

	std::string configPath = FilesHelper::ContentFolderPath + "\\" + "imgui.ini";
	ImGui::LoadIniSettingsFromDisk(configPath.c_str());

	ED_LOG(Window, info, "Finished creating window")
}

void OpenGLWindow::Update()
{
	if (glfwGetMouseButton(m_Window, 0) == GLFW_PRESS && dragState == 0)
	{
		glfwGetCursorPos(m_Window, &s_xpos, &s_ypos);
		glfwGetWindowSize(m_Window, &w_xsiz, &w_ysiz);
		dragState = 1;
	}

	if (glfwGetMouseButton(m_Window, 0) == GLFW_PRESS && dragState == 1)
	{
		double c_xpos, c_ypos;
		int w_xpos, w_ypos;
		glfwGetCursorPos(m_Window, &c_xpos, &c_ypos);
		glfwGetWindowPos(m_Window, &w_xpos, &w_ypos);

		if (s_ypos >= 0 && s_ypos <= 30) 
		{
			glfwSetWindowPos(m_Window, w_xpos + (c_xpos - s_xpos) + 6, w_ypos + (c_ypos - s_ypos) + 30);
		}
	}
	if (glfwGetMouseButton(m_Window, 0) == GLFW_RELEASE && dragState == 1) 
	{
		dragState = 0;
	}

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
	return GetMousePositionNormalized() * glm::vec2(m_Width, m_Height);
}

glm::vec2 OpenGLWindow::GetMousePositionNormalized()
{
	if (m_MousePositionOverideEnabled)
	{
		return m_MousePositionOverride;
	}

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
