#pragma once

#include "Platform/Rendering/OpenGL/EdOpenGLRendering.h"
#include "Platform/Rendering/GLFWWindow.h"

class OpenGLWindow : public GLFWWindow
{
public:
	virtual void Initialize(const WindowSpecification& specification) override;

	virtual void Resize(int32_t width, int32_t height) override;

	virtual std::shared_ptr<RenderingContext> GetContext() override;
private:
	std::shared_ptr<RenderingContext> m_Context;
};