#include "Window.h"

#include <iostream>

#include "Rendering/RendererAPI.h"

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "Engine.h"

#include "Macros.h"

Window* window = nullptr;

static void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    if (type == GL_DEBUG_TYPE_ERROR) 
    {
        ED_LOG(Rendering, err, "type {}, severity {}, message = {}", type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "", type, severity, message)
    } 
}

static void ResizeCallback(GLFWwindow* inWindow, int width, int height)
{
    window->Resize(width, height);
}

void KeyCallback(GLFWwindow* inWindow, int32_t key, int32_t scancode, int32_t action, int32_t mods)
{
    Engine::Get().InputAction(Input::ConvertGLFWInputKey(key), Input::ConvertGLFWInputAction(action));
    ED_LOG(Input, info, "Keyboard key {} action {}", key, action)
}

void MouseButtonCallback(GLFWwindow* inWindow, int button, int action, int mods)
{
    Engine::Get().InputAction(Input::ConvertGLFWInputKey(button), Input::ConvertGLFWInputAction(action));
	ED_LOG(Input, info, "Mouse button {} action {}", button, action)
}

Window& Window::Create(const std::string& name, uint32_t width, uint32_t height)
{
    if (!window)
    {
        window = new Window(name, width, height);
    }
    return *window;
}

Window& Window::Get()
{
    return *window;
}

void Window::Delete()
{
    window->Close();
    delete window;
    window = nullptr;
}

void Window::Update()
{
    m_RenderAPI->SwapBuffers(m_Window);

	glfwPollEvents();
}

bool Window::IsRunning()
{
    return !glfwWindowShouldClose(m_Window);
}

void Window::Close()
{
    glfwTerminate();
        
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();

	ED_LOG(Window, info, "Window is closed")
}

void Window::Resize(int32_t width, int32_t height)
{
    glViewport(0, 0, width, height);
    m_Width = width;
    m_Height = height;

	ED_LOG(Window, info, "Window is resized to {}x{}", width, height)
}

glm::vec2 Window::GetMousePosition()
{
    glm::dvec2 position;
    glfwGetCursorPos(m_Window, &position.x, &position.y);
    return position;
}

glm::vec2 Window::GetMousePositionNormalized()
{
    glm::dvec2 position;
    glfwGetCursorPos(m_Window, &position.x, &position.y);
    return { position.x / m_Width, position.y / m_Height };
}

uint32_t Window::GetWidth() const
{
    return m_Width;
}

uint32_t Window::GetHeight() const
{
    return m_Height;
}

GLFWwindow* Window::GetWindow() const
{
    return m_Window;
}

Window::Window(const std::string& name, uint32_t width, uint32_t height): m_Name(name), m_Width(width), m_Height(height)
{
	ED_LOG(Window, info, "Stared creating window")

    m_RenderAPI = &RendererAPI::Get();
    m_Window = m_RenderAPI->CreateContext();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//  glfwWindowHint(GLFW_SAMPLES, 4);
	//  glEnable(GL_MULTISAMPLE);

    ImGui::StyleColorsClassic();

    ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
    ImGui_ImplOpenGL3_Init("#version 460 core");

	glfwSetWindowSizeCallback(m_Window, ResizeCallback);
	glfwSetKeyCallback(m_Window, KeyCallback);
	glfwSetMouseButtonCallback(m_Window, MouseButtonCallback);

	// During init, enable debug output
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(MessageCallback, 0);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	ED_LOG(Window, info, "Finished creating window")
}
