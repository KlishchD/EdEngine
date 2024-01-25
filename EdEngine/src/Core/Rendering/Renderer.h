#pragma once

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

    void SetGamma(float gamma);
    float GetGamma() const;

    void SetSSAOEnabled(bool enabled);
    bool IsSSAOEnabled() const;

    void SetBloomEnabled(bool enabled);
    bool IsBloomEnabled() const;

    void SetUseNewBloom(bool use);
    bool IsUsingNewBloom() const;

    void SetBloomStrength(float strength);
    float GetBloomStrength() const;

    void SetBloomIntensity(float intensity);
    float GetBloomIntensity() const;

    void SetBloomMixStrength(float strength);
    float GetBloomMixStrength() const;

    void SetBloomDownscaleTexturesCount(uint32_t count);
    uint32_t GetBloomDownscaleTextureCount() const;

    void SetUpsampleScale(float scale);
    float GetUpsampleScale() const;

    void SetAAMethod(AAMethod method);
    AAMethod GetAAMethod() const;

    void SetContrastThreshold(float threshold);
    float GetContrastThreshold() const;

    void SetRelativeThreshold(float threshold);
    float GetRelativeThreshold() const;

    void SetSubpixelBlending(float scale);
    float GetSubpixelBlending() const;

    void SetIsUsingComputeShadersForPostProcessing(bool active);
    bool IsUsingComputeShadersForPostProcessing() const;

    void SetActiveRenderTarget(RenderTarget target);
    RenderTarget GetActiveRenderTarget() const;

    void SetTAAGamma(float gamma);
    float GetTAAGamma() const;

    void SetSSDORadius(float radius);
    float GetSSDORadius() const;

    void SetSSDOEnabled(bool enabled);
    bool IsSSDOEnabled() const;

    void SetSSDOSampleCount(int32_t samples);
    int32_t GetSSDOSampleCount() const;

    void SetSSDOBounceStrength(float strength);
    float GetSSDOBounceStrength() const;

    std::shared_ptr<Framebuffer> GetGeometryPassFramebuffer() const;
    std::shared_ptr<Framebuffer> LightPassFramebuffer() const;
    std::shared_ptr<Framebuffer> GetViewport() const;
    std::shared_ptr<Texture2D> GetViewportTexture() const;

private:
	float m_FarPlane = 15000.0f;

    RenderPassSpecification m_GeometryPassSpecification;
    RenderPassSpecification m_CombinationPassSpecification;

    // Light

    RenderPassSpecification m_LightPassSpecification;
    RenderPassSpecification m_PointLightShadowPassSpecification;
    
    std::shared_ptr<Shader> m_EmissionPassShader;

    //
    RenderPassSpecification m_BrighnessFilterPassSpecification;

    glm::mat4 m_LightPerspective = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, m_FarPlane);
    
    // SSAO

    RenderPassSpecification m_SSAOPassSpecification;
    RenderPassSpecification m_SSAOBlurPassSpecification;

    int32_t m_SSAOSamplesCount = 16;
    std::vector<glm::vec3> m_SSAOSamples;
    int32_t m_NoiseSize = 30;
    std::shared_ptr<Texture2D> m_SSAONoise;

    bool m_bSSAOEnabled = false;

    // SSDO

    int32_t m_SSDOSamplesCount = 4;
    std::vector<glm::vec3> m_SSDOSamples;

    RenderPassSpecification m_SSDOPassSpecification;
    RenderPassSpecification m_SSDOBlurPassSpecification;
    float m_SSDORadius = 1.0f;

    float m_SSDOBounceStrength = 1.0f;

    bool m_bSSDOEnabled = true;

    // Old Bloom
    std::shared_ptr<Framebuffer> m_BlurFramebuffer1;
    std::shared_ptr<Framebuffer> m_BlurFramebuffer2;
    std::shared_ptr<Shader> m_BlurShader;
    int32_t m_BlurPassCount = 20;

    //Shadows
    int32_t m_ShadowMapFiltersCount = 10;
    int32_t m_FilterSize = 5;
    std::shared_ptr<Texture2D> m_ShadowMapRandomSamples;

    // New Bloom
    bool m_bIsBloomEnabled = false;

	std::shared_ptr<Shader> m_BloomDownscaleShader;
	std::shared_ptr<Shader> m_BloomUpscaleShader;

	std::shared_ptr<Shader> m_BloomDownscaleComputeShader;
	std::shared_ptr<Shader> m_BloomUpscaleComputeShader;

    std::shared_ptr<Framebuffer> m_BloomFramebuffer;

    float m_BloomStrength = 0.1f;
    float m_BloomMixStrength = 0.85f;
    float m_BloomIntensity = 1.0f;

    std::vector<std::shared_ptr<Texture2D>> m_BloomIntermediateTextrues;
    int32_t m_NewBloomDownscaleCount = 4;

    bool m_bUseNewBloom = true;
    
    // Post-Processing
    RenderPassSpecification m_PostProcessingRenderPassSpecification;
    float m_Gamma = 2.2f;

    // AA

    AAMethod m_AAMethod = AAMethod::TAA;
    std::shared_ptr<Framebuffer> m_AAOutput;

    // TAA
    std::shared_ptr<Shader> m_TAAShader;
    std::shared_ptr<Shader> m_TAAComputeShader;

    int32_t m_JitterSequenceSize = 16;
    int32_t m_CurrentJitterIndex;
    std::vector<glm::vec2> m_JitterSequence;
    
    uint32_t m_HistoryBufferSize = 2;
    std::vector<std::shared_ptr<Texture>> m_HistoryBuffer;
    int32_t m_ActiveHistoryBufferTextureIndex = 0;

    glm::mat4 m_PreviousView = glm::mat4(1.0f);

    float m_TAAGamma = 1.25f;

    // Super sample
    float m_UpsampleScale = 1.0f;

    // FXAA

    std::shared_ptr<Shader> m_FFXAShader;
    float m_ContrastThreshold = 0.0312f;
    float m_RelativeThreshold = 0.125f;
    float m_SubpixelBlending = 1.0f;

    //

	bool m_bUseComputeShadersForPostProcessing = false;

    RenderTarget m_ActiveRenderTarget = RenderTarget::PostProcessing;

    std::queue<std::function<void(RenderingContext* context)>> m_Commands;

    Engine* m_Engine = nullptr;

    glm::ivec2 m_ViewportSize = glm::ivec2(1);

    void GeometryPass(const std::vector<std::shared_ptr<Component>>& components, Camera* camera);
    void LightPass(const std::vector<std::shared_ptr<Component>>& components, Camera* camera);

    void SSAOPass(Camera* camera);
    void SSDOPass(Camera* camera);

    void BloomPass();
    void NewBloomPass();
    void CombinationPass(const std::vector<std::shared_ptr<Component>>& components, Camera* camera);
    void PostProcessingPass();

    void TAAPass();
    void FXAAPass();

    void CreateRandomShadowMapSamples();

    void BloomDownscale(const std::shared_ptr<Texture2D>& in, const std::shared_ptr<Texture2D>& out);
    void BloomUpscale(const std::shared_ptr<Texture2D>& downscaled, const std::shared_ptr<Texture2D>& upscaled, const std::shared_ptr<Texture2D>& fullsize = nullptr);
    void UpdateBloomTexturesSizes();

private:
    void SetupGeometryRenderPass();
    void SetupLightRenderPass();
    void SetupSSAORenderPass();
    void SetupSSDORenderPass();
    void SetupCombinationRenderPass();
    void SetupPostProcessingRenderPass();

    void SetupNewBloomRenderPass();

    void SetupAA();
    void SetupFXAARenderPass();
    void SetupTAARenderPass();

    void SetupShadowRenderPass();
    void SetupBrightnessFilterPass();

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

	void SubmitMesh(const std::shared_ptr<StaticMesh>& mesh, const Transform& transform, const Transform& previousTransform);
	void SubmitSubmesh(const std::shared_ptr<StaticSubmesh>& submesh, const Transform& transform, const Transform& previousTransform);

	void SubmitMeshRaw(const std::shared_ptr<StaticMesh>& mesh, const Transform& transform, const Transform& previousTransform);
	void SubmitSubmeshRaw(const std::shared_ptr<StaticSubmesh>& submesh, const Transform& transform, const Transform& previousTransform);

	void SubmitQuad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
	void SubmitIcon(const std::shared_ptr<Texture2D>& texture, const glm::mat4& Transform);

	void BeginUIFrame();
	void EndUIFrame();

    //void Submit(const std::string& text, const Font& font, float x, float y, float scale, glm::vec3 color, const std::shared_ptr<Shader>& shader);

	void EndRenderPass();
};
