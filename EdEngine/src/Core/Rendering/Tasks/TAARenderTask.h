#pragma once

#include "RenderTask.h"

class Framebuffer;

class TAARenderTask : public RenderTask
{
public:
	virtual void Setup(Renderer* renderer) override;
	virtual void Run(const std::vector<std::shared_ptr<Component>>& components, Camera* camera) override;
	virtual void Resize(glm::ivec2 size, float upscale) override;

private:
	std::shared_ptr<Framebuffer> m_AAFramebuffer;
	
	std::shared_ptr<Shader> m_TAAShader;

	uint32_t m_HistoryBufferSize = 2;
	std::vector<std::shared_ptr<Texture>> m_HistoryBuffer;
	int32_t m_ActiveHistoryBufferTextureIndex = 0;

	float m_TAAGamma = 1.25f;
};