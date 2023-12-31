﻿#pragma once

#include "Core/BaseManager.h"
#include "RenderPassSpecification.h"
#include "Core/Math/Camera.h"
#include "Core/Math/Transform.h"
#include <queue>
#include <functional>

class Engine;
class RenderingContext;
class Component;
class PointLightComponent;

class VertexBuffer;
class IndexBuffer;

class Framebuffer;
class CubeFramebuffer;
class Shader;

class Texture;
class Texture2D;

class StaticMesh;
class StaticSubmesh;

class Renderer: public BaseManager
{
public:
    virtual void Initialize(Engine* engine) override;
    virtual void Deinitialize() override;

    void Update();

    void ResizeViewport(glm::vec2 size);

    void SubmitRenderCommand(const std::function<void(RenderingContext* context)>& command);

    void SetSSAOEnabled(bool enabled);
    bool IsSSAOEnabled() const;

    std::shared_ptr<Framebuffer> GetGeometryPassFramebuffer() const;
    std::shared_ptr<Framebuffer> LightPassFramebuffer() const;
    std::shared_ptr<Framebuffer> GetViewport() const;
private:
	float m_FarPlane = 15000.0f;

    RenderPassSpecification m_GeometryPassSpecification;
    RenderPassSpecification m_LightPassSpecification;
    RenderPassSpecification m_SSAOPassSpecification;
    RenderPassSpecification m_SSAOBlurPassSpecification;
    RenderPassSpecification m_CombinationPassSpecification;

    RenderPassSpecification m_PointLightShadowPassSpecification;
    RenderPassSpecification m_BrighnessFilterPassSpecification;

    glm::mat4 m_LightPerspective = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, m_FarPlane);
    
    // SSAO
    int32_t m_SSAOSamplesCount = 16;
    std::vector<glm::vec3> m_SSAOSamples;
    int32_t m_NoiseSize = 10;
    std::shared_ptr<Texture2D> m_SSAONoise;

    bool bSSAOEnabled = true;

    // Bloom
    std::shared_ptr<Framebuffer> m_BlurFramebuffer1;
    std::shared_ptr<Framebuffer> m_BlurFramebuffer2;
    std::shared_ptr<Shader> m_BlurShader;
    int32_t m_BlurPassCount = 20;

    //Shadows
    int32_t m_ShadowMapFiltersCount = 10;
    int32_t m_FilterSize = 5;
    std::shared_ptr<Texture2D> m_ShadowMapRandomSamples;

    std::queue<std::function<void(RenderingContext* context)>> m_Commands;

    Engine* m_Engine = nullptr;

    glm::ivec2 m_ViewportSize = glm::ivec2(1);

    void GeometryPass(const std::vector<std::shared_ptr<Component>>& components, Camera* camera);
    void LightPass(const std::vector<std::shared_ptr<Component>>& components, Camera* camera);
    void SSAOPass(Camera* camera);
    void BloomPass();
    void CombinationPass(const std::vector<std::shared_ptr<Component>>& components, Camera* camera);

    void CreateRandomShadowMapSamples();

private:
    void SetupGeometryRenderPass();
    void SetupLightRenderPass();
    void SetupSSAORenderPass();
    void SetupCombinationRenderPass();

    void SetupShadowRenderPass();
    void SetupBrightnessFilterPass();

    static float lerp(float a, float b, float f);

private:
	std::shared_ptr<RenderingContext> m_Context;

	RenderPassSpecification* m_Specification;

	glm::mat4 m_View;
	glm::mat4 m_Projection;
	RenderPassSpecification m_TemporarySpecification;

	int32_t m_LightCount = 0;

	glm::vec4 m_Up;

	std::shared_ptr<VertexBuffer> m_QuadVBO;
	std::shared_ptr<VertexBuffer> m_TextVBO;

	std::shared_ptr<VertexBuffer> m_PointLightMeshVBO;
	std::shared_ptr<IndexBuffer> m_PointLightMeshIBO;
public:
	void BeginRenderPass(const std::string& name, const std::shared_ptr<BaseFramebuffer>& framebuffer, const std::shared_ptr<Shader>& shader, const glm::mat4& view, const glm::mat4 projection, glm::vec3 viewPosition);
	void BeginRenderPass(RenderPassSpecification& specification, const glm::mat4& view, const glm::mat4& projection);

	void SetNewCameraInformation(const glm::mat4& view, const glm::mat4& projection, glm::vec3 viewPosition);

	void SubmitLight(const std::shared_ptr<PointLightComponent>& light);
	void SubmitLightMesh(const std::shared_ptr<PointLightComponent>& light, const std::shared_ptr<Texture>& shadowMap);

	void SubmitMesh(const std::shared_ptr<StaticMesh>& mesh, const Transform& transform);
	void SubmitSubmesh(const std::shared_ptr<StaticSubmesh>& submesh, const Transform& transform);

	void SubmitMeshRaw(const std::shared_ptr<StaticMesh>& mesh, const Transform& transform);
	void SubmitSubmeshRaw(const std::shared_ptr<StaticSubmesh>& submesh, const Transform& transform);

	void SubmitQuad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
	void SubmitIcon(const std::shared_ptr<Texture2D>& texture, const glm::mat4& Transform);

	void BeginUIFrame();
	void EndUIFrame();

    //void Submit(const std::string& text, const Font& font, float x, float y, float scale, glm::vec3 color, const std::shared_ptr<Shader>& shader);

	void EndRenderPass();
};
