#pragma once

#include "RenderTask.h"

class FXAARenderTask : public RenderTask
{
public:
	virtual void Setup(Renderer* renderer) override;
	virtual void Run(const std::vector<std::shared_ptr<Component>>& components, Camera* camera) override;
private:
	std::shared_ptr<Framebuffer> m_AAFramebuffer;

	std::shared_ptr<Shader> m_FFXAShader;

	float m_ContrastThreshold = 0.0312f;
	float m_RelativeThreshold = 0.125f;
	float m_SubpixelBlending = 1.0f;
};