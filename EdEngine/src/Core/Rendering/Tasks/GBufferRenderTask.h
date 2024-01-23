#pragma once

#include "RenderTask.h"

class GBufferRenderTask : public RenderTask
{
public:
	virtual void Setup(Renderer* renderer) override;
	virtual void Run(const std::vector<std::shared_ptr<Component>>& components, Camera* camera) override;
	virtual void Resize(glm::ivec2 size, float upscale) override;

	std::shared_ptr<Texture2D> GetAlbedoTexture() const;
	std::shared_ptr<Texture2D> GetPositionTexture() const;
	std::shared_ptr<Texture2D> GetNormalTexture() const;
	std::shared_ptr<Texture2D> GetRoughnessMetalicEmissionTexture() const;
	std::shared_ptr<Texture2D> GetVelocityTexture() const;
	std::shared_ptr<Texture2D> GetDepthTexture() const;
	std::shared_ptr<Framebuffer> GetFramebuffer() const;
private:
	RenderPassSpecification m_GBufferPassSpecification;

	int32_t m_JitterSequenceSize = 16;
	int32_t m_CurrentJitterIndex;
	std::vector<glm::vec2> m_JitterSequence;

	glm::mat4 m_PreviousView;
};