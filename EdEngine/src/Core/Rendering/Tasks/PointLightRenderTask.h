#pragma once

#include "RenderTask.h"

class PointLightComponent;

class PointLightRenderTask : public RenderTask
{
public:
	virtual void Setup(Renderer* renderer) override;
	virtual void Run(const std::vector<std::shared_ptr<Component>>& components, Camera* camera) override;
	virtual void Resize(glm::ivec2 size, float upscale) override;

private:
	RenderPassSpecification m_ShadowPassSpecification;
	RenderPassSpecification m_LightPassSpecification;
	RenderPassSpecification m_LightWireframePassSpecification;

	glm::mat4 m_ShadowMapPerspective;

	void DrawShadowMap(const std::vector<std::shared_ptr<Component>>& components, std::shared_ptr<PointLightComponent> light);
	void DrawLight(std::shared_ptr<PointLightComponent> light, Camera* camera);
	void DrawWireframe(std::shared_ptr<PointLightComponent> light, Camera* camera);
};