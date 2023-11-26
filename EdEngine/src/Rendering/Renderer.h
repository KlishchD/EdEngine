#pragma once

#include "Core/BaseManager.h"
#include "CubeFramebuffer.h"
#include "RendererAPI.h"
#include "Core/Assets/Sampler.h"
#include <queue>

class Engine;
class Camera;

class Renderer: public BaseManager
{
	constexpr static int32_t BrightnessFilterTextureSlot = 10;
	constexpr static int32_t BlurReadTextreSlot = 10;

	constexpr static int32_t AlbedoTextureSlot = 10;
	constexpr static int32_t PositionTextureSlot = 11;
	constexpr static int32_t NormalTextureSlot = 12;
	constexpr static int32_t RoughnessMetalicTextureSlot = 13;
	constexpr static int32_t ShadowMapRandomSamplesTextureSlot = 14;

	constexpr static int32_t LightPassTextureSlot = 15;

	constexpr static int32_t BloomPassTextureSlot = 16;
public:
    virtual void Initialize(Engine* engine) override;
    virtual void Deinitialize() override;

    void Update();

    void ResizeViewport(glm::vec2 size);

    void SubmitRenderCommand(const std::function<void(RendererAPI* renderAPI)>& command);

    std::shared_ptr<Framebuffer> GetGeometryPassFramebuffer() const;
    std::shared_ptr<Framebuffer> LightPassFramebuffer() const;
    std::shared_ptr<Framebuffer> GetViewport() const;
private:
	float m_FarPlane = 15000.0f;

    RenderPassSpecification m_GeometryPassSpecification;
    RenderPassSpecification m_LightPassSpecification;
    RenderPassSpecification m_CombinationPassSpecification;

    RenderPassSpecification m_ShadowPassSpecification;
    RenderPassSpecification m_BrighnessFilterPassSpecification;

    glm::mat4 m_LightPerspective = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, m_FarPlane);
    
    // Bloom
    std::shared_ptr<Framebuffer> m_BlurFramebuffer1;
    std::shared_ptr<Framebuffer> m_BlurFramebuffer2;
    std::shared_ptr<Shader> m_BlurShader;
    int32_t m_BlurPassCount = 20;

    //Shadows
    int32_t m_ShadowMapFiltersCount = 10;
    int32_t m_FilterSize = 5;
    std::shared_ptr<Texture2D> m_ShadowMapRandomSamples;

    std::queue<std::function<void(RendererAPI* renderAPI)>> m_Commands;

    RendererAPI* m_RendererAPI = nullptr;
    Engine* m_Engine = nullptr;

    glm::vec2 m_ViewportSize = glm::vec2(1.0f);

    void GeometryPass(const std::vector<std::shared_ptr<Component>>& components, Camera* camera);
    void LightPass(const std::vector<std::shared_ptr<Component>>& components, Camera* camera);
    void BloomPass();
    void CombinationPass(const std::vector<std::shared_ptr<Component>>& components, Camera* camera);

    void CreateRandomShadowMapSamples();

private:
    void SetupGeometryRenderPass();
    void SetupLightRenderPass();
    void SetupCombinationRenderPass();

    void SetupShadowRenderPass();
    void SetupBrightnessFilterPass();
};
