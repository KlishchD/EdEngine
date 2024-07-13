#pragma once

#include "Platform/Rendering/GLFWWindow.h"

class D3D12RenderingContext;

class D3D12Window : public GLFWWindow
{
public:
  virtual void Initialize(const WindowSpecification& specification) override;

  virtual std::shared_ptr<RenderingContext> GetContext() override;

protected:
  std::shared_ptr<D3D12RenderingContext> m_Context;
};
