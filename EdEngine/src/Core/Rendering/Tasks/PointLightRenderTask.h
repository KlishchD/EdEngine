#pragma once

#include "RenderTask.h"
#include <Core/Rendering/Renderer.h>

class PointLightComponent;

class PointLightRenderTask : public RenderTask
{
public:
	virtual void Setup(Renderer* renderer) override;
	virtual void Run(const std::vector<std::shared_ptr<Component>>& components, Camera* camera) override;
	virtual void Resize(glm::ivec2 size, float upscale) override;

private:
	RenderPassSpecification m_LightPassSpecification;

	RenderPassSpecification m_ShadowPassSpecification;

	int32_t m_FilterSize = 5;

	glm::mat4 m_ShadowMapPerspective;

	void DrawShadowMap(const std::vector<std::shared_ptr<Component>>& components, const std::shared_ptr<PointLightComponent>& light);
	void DrawLight(const std::shared_ptr<PointLightComponent>& light, Camera* camera);
};