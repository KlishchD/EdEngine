#pragma once

#include "RenderTask.h"

class DirectionalLightComponent;

class DirectionalLightRenderTask : public RenderTask
{
	const uint32_t ShadowCascadeMinSize = 2048;
	const uint32_t MinShadowCascadesCount = 1;
	const uint32_t MaxShadowCascadesCount = 4;
public:
	virtual void Setup(Renderer* renderer) override;
	virtual void Run(const std::vector<std::shared_ptr<Component>>& components, Camera* camera) override;
	virtual void Resize(glm::ivec2 size, float upscale) override;

	std::shared_ptr<Texture2D> GetShadowTexture() const;
private:
	RenderPassSpecification	m_LightPassSpecification;
	RenderPassSpecification m_ShadowPassSpecification;

	uint32_t m_MaxCascadesCount = 4;

	std::vector<glm::mat4> m_CascadeProjectionViewMatrices;

	void CalculateShadowProjections(std::shared_ptr<DirectionalLightComponent> light, Camera* camera);
	void DrawShadows(std::shared_ptr<DirectionalLightComponent> light, const std::vector<std::shared_ptr<Component>>& components, Camera* camera);
	void DrawLight(std::shared_ptr<DirectionalLightComponent> light, Camera* camera);
};