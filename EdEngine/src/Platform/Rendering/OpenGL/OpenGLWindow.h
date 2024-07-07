#pragma once

#include "Platform/Rendering/OpenGL/EdOpenGLRendering.h"
#include "Platform/Rendering/GLFWWindow.h"

class OpenGLWindow : public GLFWWindow
{
public:
	OpenGLWindow(WindowSpecification specification);

	virtual void Initialize(WindowSpecification specification) override;

	virtual void Resize(int32_t width, int32_t height) override;

	virtual std::shared_ptr<RenderingContext> GetContext() override;

	virtual void Close() override;
private:
	std::shared_ptr<RenderingContext> m_Context;
};