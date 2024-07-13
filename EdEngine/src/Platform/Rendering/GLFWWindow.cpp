#include "GLFWWindow.h"
#include <glslang/Public/ShaderLang.h>
#include "Platform/Rendering/EdRendering.h"
#include "Core/Rendering/RenderingContex.h"
#include "Core/Engine.h"
#include "Helpers/PlatformHelper.h"

std::vector<Window*> s_Windows;

void GLFWWindow::Initialize(const WindowSpecification& specification)
{
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

  ShInitialize();
}

void GLFWWindow::Update()
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

bool GLFWWindow::IsRunning()
{
  return !glfwWindowShouldClose(m_Window);
}

void GLFWWindow::Resize(int32_t width, int32_t height)
{
  m_Width = width;
  m_Height = height;
  ED_LOG(Window, info, "Window is resized to {}x{}", width, height) // TODO: Add resize for all screen size frame buffers :)
}

glm::vec2 GLFWWindow::GetMousePosition()
{
  glm::dvec2 position;
  glfwGetCursorPos(m_Window, &position.x, &position.y);
  return position;
}

glm::vec2 GLFWWindow::GetMousePositionNormalized()
{
  glm::dvec2 position;
  glfwGetCursorPos(m_Window, &position.x, &position.y);
  return {position.x / m_Width, position.y / m_Height};
}

void GLFWWindow::Move(glm::vec2 delta)
{
  glm::ivec2 position;
  glfwGetWindowPos(m_Window, &position.x, &position.y);
  position += delta;
  glfwSetWindowPos(m_Window, position.x, position.y);
}

void* GLFWWindow::GetNativeWindow()
{
  return m_Window;
}

void GLFWWindow::Close()
{
  if (m_Window)
  {
    GetContext()->Close();

    ShFinalize();

    glfwTerminate();

    m_Window = nullptr;

    ED_LOG(Window, info, "Window is closed")
  }
}

GLFWWindow::~GLFWWindow()
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
