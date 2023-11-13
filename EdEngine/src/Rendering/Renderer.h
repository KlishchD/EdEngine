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
    constexpr static int32_t BrightnessFilterTextureSlot = 10;
    constexpr static int32_t BlurReadTextreSlot = 10;

    constexpr static int32_t AlbedoTextureSlot = 10;
    constexpr static int32_t PositionTextureSlot = 11;
    constexpr static int32_t NormalTextureSlot = 12;
    constexpr static int32_t RoughnessMetalicTextureSlot = 13;
    constexpr static int32_t ShadowMapRandomSamplesTextureSlot = 14;

    constexpr static int32_t LightPassTextureSlot = 15;

    constexpr static int32_t BloomPassTextureSlot = 16;

	float m_FarPlane = 15000.0f;

    std::shared_ptr<Shader> m_GeometryPassShader;
    std::shared_ptr<Framebuffer> m_GeometryFramebuffer;
    
    std::shared_ptr<Shader> m_ShadowPassShader;
    std::shared_ptr<CubeFramebuffer> m_ShadowMapsFramebuffer;
    glm::mat4 m_LightPerspective = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, m_FarPlane);
    
    std::shared_ptr<Shader> m_LightPassShader;
    std::shared_ptr<Framebuffer> m_LightPassFramebuffer;

    std::shared_ptr<Framebuffer> m_BlurFramebuffer1;
    std::shared_ptr<Framebuffer> m_BlurFramebuffer2;
    std::shared_ptr<Shader> m_BlurShader;
    std::shared_ptr<Shader> m_BrighnessFilterShader;

    int32_t m_ShadowMapFiltersCount = 10;
    int32_t m_FilterSize = 5;
    std::shared_ptr<Texture2D> m_ShadowMapRandomSamples;

    int32_t m_BlurPassCount = 20;

    std::shared_ptr<Shader> m_CombinationPassShader;

    std::shared_ptr<Framebuffer> m_ViewportFramebuffer;

    std::queue<std::function<void(RendererAPI* renderAPI)>> m_Commands;

    RendererAPI* m_RendererAPI = nullptr;
    Engine* m_Engine = nullptr;

    glm::vec2 m_ViewportSize = glm::vec2(1.0f);

    void GeometryPass(const std::vector<std::shared_ptr<Component>>& components, Camera* camera);
    void LightPass(const std::vector<std::shared_ptr<Component>>& components, Camera* camera);
    void BloomPass();
    void CombinationPass(const std::vector<std::shared_ptr<Component>>& components, Camera* camera);

    void CreateRandomShadowMapSamples();
};
