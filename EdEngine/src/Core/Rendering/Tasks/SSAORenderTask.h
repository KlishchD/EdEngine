#pragma once

#include "RenderTask.h"

class SSAORenderTask : public RenderTask
{
public:
	virtual void Setup(Renderer* renderer) override;
	virtual void Run(const std::vector<std::shared_ptr<Component>>& components, Camera* camera) override;
	virtual void Resize(glm::ivec2 size, float upscale) override;

	std::shared_ptr<Texture2D> GetRawTexture() const;
	std::shared_ptr<Texture2D> GetBluredTexture() const;

	void SetSamplesCount(uint32_t count);
	uint32_t GetSamplesCount() const;

	void SetNoiseSize(uint32_t size);
	uint32_t GetNosiseSize() const;

	void SetRadius(float radius);
	float GetRadius() const;

	void SetBias(float bias);
	float GetBias() const;
private:
	RenderPassSpecification m_SSAOPassSpecification;
	RenderPassSpecification m_SSAOBlurPassSpecification;

	float m_Radius = 1.5f;
	float m_Bias = 0.025f;

	uint32_t m_SamplesCount = 16;
	std::vector<glm::vec3> m_Samples;

	uint32_t m_NoiseSize = 10;
	std::shared_ptr<Texture2D> m_Noise;

	void SSAO(Camera* camera);
	void Blur();
};
