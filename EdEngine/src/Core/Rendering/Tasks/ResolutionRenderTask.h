#pragma once

#include "RenderTask.h"

class ResolutionRenderTask : public RenderTask
{
public:
	virtual void Setup(Renderer* renderer) override;
	virtual void Run(const std::vector<std::shared_ptr<Component>>& components, Camera* camera) override;
	virtual void Resize(glm::ivec2 size, float upscale) override;

	void SetGamma(float gamma);
	float GetGamma() const;

	void SetBloomStrength(float strength);
	float GetBloomStrength() const;

	void SetBloomIntensity(float intensity);
	float GetBloomIntensity() const;
private:
	RenderPassSpecification m_ResolutionRenderPassSpecification;

	float m_BloomStrength = 0.1f;
	float m_BloomIntensity = 1.0f;

	float m_Gamma = 2.2f;
};