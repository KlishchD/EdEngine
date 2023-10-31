#include "Window.h"

#include <iostream>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "Engine.h"

Window* window = nullptr;

static void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    if (type == GL_DEBUG_TYPE_ERROR) {
        fprintf( stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
                 ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ),
                  type, severity, message );
    }
}

static void ResizeCallback(GLFWwindow* inWindow, int width, int height)
{
    window->Resize(width, height);
}

void KeyCallback(GLFWwindow* inWindow, int32_t key, int32_t scancode, int32_t action, int32_t mods)
{
    Engine::Get().InputAction(Input::ConvertGLFWInputKey(key), Input::ConvertGLFWInputAction(action));
    std::cout << "[Input] key " << key << " action " << action << std::endl;
}

void MouseButtonCallback(GLFWwindow* inWindow, int button, int action, int mods)
{
    Engine::Get().InputAction(Input::ConvertGLFWInputKey(button), Input::ConvertGLFWInputAction(action));
    std::cout << "[Input] button " << button << " action " << action << std::endl;
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

void Window::Clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::BeginUIFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
}

void Window::EndUIFrame()
{
    ImGui::Render();

    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

void Window::Initialize()
{
}

void Window::Deinitialize()
{
}

void Window::Update()
{
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
    glfwSwapBuffers(m_Window);

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
}

void Window::Resize(int32_t width, int32_t height)
{
    glViewport(0, 0, width, height);
    m_Width = width;
    m_Height = height;
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
    if (!glfwInit()) {
        std::cout << "ERROR: FAILED TO INITIALIZE GLFW" << std::endl;
        return;
    }
        
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//  glfwWindowHint(GLFW_SAMPLES, 4);
//  glEnable(GL_MULTISAMPLE);
        

    m_Window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!m_Window)
    {
        glfwTerminate();
        std::cout << "ERROR: FAILED TO INITIALIZE WINDOW" << std::endl;
        return;
    }

    glfwMakeContextCurrent(m_Window);

    if (glewInit() != GLEW_OK) {
        glfwTerminate();
        std::cout << "ERROR: FAILED TO INITIALIZE GLEW" << std::endl;
        return;
    }

    glfwSetWindowSizeCallback(m_Window, ResizeCallback);
    glfwSetKeyCallback(m_Window, KeyCallback);
    glfwSetMouseButtonCallback(m_Window, MouseButtonCallback);

    // During init, enable debug output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
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
}
