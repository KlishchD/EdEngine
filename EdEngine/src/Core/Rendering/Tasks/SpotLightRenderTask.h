#pragma once

#include "RenderTask.h"

class SpotLightComponent;

class SpotLightRenderTask : public RenderTask
{
public:
	virtual void Setup(Renderer* renderer) override;
	virtual void Run(const std::vector<std::shared_ptr<Component>>& components, Camera* camera) override;
	virtual void Resize(glm::ivec2 size, float upscale) override;

	void SetShadowSamplesBlockCount(int32_t count);
	int32_t GetShadowSamplesBlocksCount() const;

	void SetShadowFilterSize(int32_t size);
	int32_t GetShadowFilterSize() const;

	void SetShadowFilterRadius(float radius);
	float GetShadowFilterRadius() const;
private:
	RenderPassSpecification m_ShadowPassSpecification;
	RenderPassSpecification m_LightPassSpecification;
	RenderPassSpecification m_LightMeshWireframePassSpecification;

	glm::mat4 m_ShadowProjectionViewMatrix;

	float m_Aspect = 1.0f;
	glm::vec2 m_ShadowMapPixelSize;

	int32_t m_ShadowFilterSize = 4;
	int32_t m_ShadowSamplesBlockCount = 4;
	float m_ShadowFilterRadius = 4.0f;

	std::shared_ptr<Texture2D> m_ShadowSamples;

	void DrawShadows(const std::vector<std::shared_ptr<Component>>& components, const std::shared_ptr<SpotLightComponent>& light);
	void DrawLight(Camera* camera, const std::shared_ptr<SpotLightComponent>& light);
	void DrawWireframe(Camera* camera, const std::shared_ptr<SpotLightComponent>& light);

	void UpdateShadowSamplesTexture();
};