#pragma once

#include "RenderTask.h"

class SSDORenderTask : public RenderTask
{
public:
	virtual void Setup(Renderer* renderer) override;
	virtual void Run(const std::vector<std::shared_ptr<Component>>& components, Camera* camera) override;
	virtual void Resize(glm::ivec2 size, float upscale) override;

	void SetSamplesCount(int32_t count);
	int32_t GetSamplesCount() const;

	void SetIndirectStrength(float strength);
	float GetIndirectStrength() const;

	void SetDirectLightStrength(float strength);
	float GetDirectLightStrength() const;

	void SetRadius(float radius);
	float GetRadius() const;

	void SetBlurFilterSize(int32_t size);
	int32_t GetBlurFilterSize() const;

	std::vector<glm::vec3> GetSamples() const;

	std::shared_ptr<Texture2D> GetTexutre() const;
private:
	RenderPassSpecification m_SSDOPassSpecification;
	RenderPassSpecification m_SSDOBlurPassSpecification;

	std::shared_ptr<Shader> m_IndirectLightShader;

	int32_t m_SamplesCount = 16;
	std::vector<glm::vec3> m_Samples;

	float m_DriectLightStrength = 1.0f;
	float m_IndirectStrength = 1.0f;
	float m_Radius = 1.0f;

	int32_t m_BlurFilterSize = 16;

	void SSDO(Camera* camera);
	void Blur();
};
