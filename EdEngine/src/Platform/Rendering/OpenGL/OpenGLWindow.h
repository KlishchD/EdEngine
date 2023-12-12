#pragma once

#include "Core/Window.h"

class OpenGLWindow : public Window
{
public:
	OpenGLWindow(WindowSpecification specification);

	virtual void Update() override;

	virtual bool IsRunning() override;

	virtual void Resize(int32_t width, int32_t height) override;

	virtual glm::vec2 GetMousePosition() override;
	virtual glm::vec2 GetMousePositionNormalized() override;

	virtual void* GetNativeWindow() override;

	virtual std::shared_ptr<RenderingContext> GetContext() override;

	virtual void Close() override;
	virtual ~OpenGLWindow() override;
private:
	std::shared_ptr<RenderingContext> m_Context;

	struct GLFWwindow* m_Window;
};