#pragma once

#include "RenderTask.h"

class AmbientRenderTask : public RenderTask
{
public:
	virtual void Setup(Renderer* renderer) override;
	virtual void Run(const std::vector<std::shared_ptr<Component>>& components, Camera* camera) override;
private:
	std::shared_ptr<Shader> m_AmbientShader;
};