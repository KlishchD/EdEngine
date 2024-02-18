#pragma once

#include "RenderTask.h"

class SpotLightComponent;

class SpotLightRenderTask : public RenderTask
{
public:
	virtual void Setup(Renderer* renderer) override;
	virtual void Run(const std::vector<std::shared_ptr<Component>>& components, Camera* camera) override;
	virtual void Resize(glm::ivec2 size, float upscale) override;

	void SetShadowSamplesBlockCount(uint32_t count);
	uint32_t GetShadowSamplesBlocksCount() const;

	void SetShadowSamplesBlockSize(uint32_t size);
	uint32_t GetShadowSamplesBlockSize() const;
private:
	RenderPassSpecification m_ShadowPassSpecification;
	RenderPassSpecification m_LightPassSpecification;
	RenderPassSpecification m_LightMeshWireframePassSpecification;

	glm::mat4 m_ShadowProjectionViewMatrix;

	float m_Aspect = 1.0f;
	glm::vec2 m_ShadowMapPixelSize;

	uint32_t m_ShadowSamplesBlockCount = 10;
	uint32_t m_ShadowSamplesBlockSize = 32;

	std::shared_ptr<Texture2D> m_ShadowSamples;

	void DrawShadows(const std::vector<std::shared_ptr<Component>>& components, std::shared_ptr<SpotLightComponent> light);
	void DrawLight(Camera* camera, std::shared_ptr<SpotLightComponent> light);
	void DrawWireframe(Camera* camera, std::shared_ptr<SpotLightComponent> light);

	void UpdateShadowSamplesTexture();
};