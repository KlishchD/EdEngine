#pragma once

#include "Core/Rendering/Renderer.h"
#include "Core/Rendering/RenderingContex.h"
#include "Core/Rendering/Textures/Texture2D.h"

class RenderTask
{
public:
	virtual void Setup(Renderer* renderer);
	virtual void Run(const std::vector<std::shared_ptr<Component>>& components, Camera* camera) = 0;
	virtual void Resize(glm::ivec2 size, float upscale) {};

	void SetEnabled(bool enabled);
	bool IsEnabled() const;

	virtual ~RenderTask() {}
protected:
	Renderer* m_Renderer;
	std::shared_ptr<RenderingContext> m_Context;

	bool m_bIsEnabled = true;
};