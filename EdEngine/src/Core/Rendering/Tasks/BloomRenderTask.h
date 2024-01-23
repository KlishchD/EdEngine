#pragma once

#include "RenderTask.h"

class BloomRenderTask : public RenderTask
{
public:
	virtual void Setup(Renderer* renderer) override;
	virtual void Run(const std::vector<std::shared_ptr<Component>>& components, Camera* camera) override;
	virtual void Resize(glm::ivec2 size, float upscale) override;

private:
	std::shared_ptr<Shader> m_BloomDownscaleShader;
	std::shared_ptr<Shader> m_BloomUpscaleShader;

	std::shared_ptr<Framebuffer> m_BloomFramebuffer;

	float m_BloomMixStrength = 0.85f;

	std::vector<std::shared_ptr<Texture2D>> m_BloomIntermediateTextrues;
	int32_t m_NewBloomDownscaleCount = 4;

	void BloomDownscale(const std::shared_ptr<Texture2D>& in, const std::shared_ptr<Texture2D>& out);
	void BloomUpscale(const std::shared_ptr<Texture2D>& downscaled, const std::shared_ptr<Texture2D>& upscaled, const std::shared_ptr<Texture2D>& fullsize = nullptr);
	void UpdateBloomTexturesSizes(glm::vec2 size);
};