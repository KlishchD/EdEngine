#pragma once

#include "RenderTask.h"

class SSAORenderTask : public RenderTask
{
public:
	virtual void Setup(Renderer* renderer) override;
	virtual void Run(const std::vector<std::shared_ptr<Component>>& components, Camera* camera) override;
	virtual void Resize(glm::ivec2 size, float upscale) override;

private:
	RenderPassSpecification m_SSAOPassSpecification;
	RenderPassSpecification m_SSAOBlurPassSpecification;

	int32_t m_SSAOSamplesCount = 16;
	std::vector<glm::vec3> m_SSAOSamples;

	int32_t m_NoiseSize = 10;
	std::shared_ptr<Texture2D> m_SSAONoise;

	void SSAO(Camera* camera);
	void Blur();
};
