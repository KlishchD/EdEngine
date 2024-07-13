#include "D3D12Window.h"
#include "EdD3D12Rendering.h"
#include "D3D12RenderingContext.h"

void D3D12Window::Initialize(const WindowSpecification& specification)
{
  Window::Initialize(specification);

  ED_ASSERT(glfwInit() == GLFW_TRUE, "Failed to initialize GLFW");

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  m_Window = glfwCreateWindow(specification.Width, specification.Height, specification.Title.c_str(), nullptr, nullptr);

  if (!m_Window)
  {
    glfwTerminate();
    ED_ASSERT(0, "Failed to create window");
  }

  glfwMakeContextCurrent(m_Window);

  GLFWWindow::Initialize(specification);

  m_Context = std::make_shared<D3D12RenderingContext>(this);

  ED_LOG(Window, info, "D3D12 window was created")
}

std::shared_ptr<class RenderingContext> D3D12Window::GetContext()
{
  return m_Context;
}

