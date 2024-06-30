#pragma once

#include "Core/Window.h"

class GLFWWindow : public Window
{
public:
	GLFWWindow(WindowSpecification specification);

	virtual void Initialize(WindowSpecification specification) override;

	virtual void Update() override;

	virtual bool IsRunning() override;

	virtual void Resize(int32_t width, int32_t height) override;

	virtual glm::vec2 GetMousePosition() override;
	virtual glm::vec2 GetMousePositionNormalized() override;

	virtual void Move(glm::vec2 delta) override;

	virtual void* GetNativeWindow() override;

	virtual void Close() override;

	virtual ~GLFWWindow() override;
protected:
	struct GLFWwindow* m_Window;

	double s_xpos = 0, s_ypos = 0;
	int w_xsiz = 0, w_ysiz = 0;
	int dragState = 0;
};