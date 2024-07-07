#include "OpenGLWindow.h"
#include "OpenGLRenderingContext.h"
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

void OpenGLWindow::Initialize(const WindowSpecification& specification)
{
  Window::Initialize(specification);

  ED_ASSERT(glfwInit() == GLFW_TRUE, "Failed to initialize GLFW");

  m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), NULL, NULL);

  if (!m_Window)
  {
    glfwTerminate();
    ED_ASSERT(0, "Failed to initialize GLFW");
  }

  GLFWWindow::Initialize(specification);

  glfwMakeContextCurrent(m_Window);

  m_Context = std::make_shared<OpenGLRenderingContext>(this);

  ED_LOG(Window, info, "OpenGL window was created");
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
