#pragma once

#include "Core/BaseManager.h"
#include "RenderPassSpecification.h"
#include "Core/Math/Camera.h"
#include "Core/Math/Transform.h"
#include "Framebuffer.h"
#include <queue>
#include <functional>

class Engine;
class RenderingContext;

class Component;
class StaticMeshComponent;
class PointLightComponent;
class SpotLightComponent;
class DirectionalLightComponent;
class CameraComponent;

class VertexBuffer;
class IndexBuffer;

class Shader;

class Texture;
class Texture2D;

class StaticMesh;
class StaticSubmesh;

class RenderGraph;

// TODO: It is not spoused to be manager !!!
ED_CLASS(Renderer) : public BaseManager
{
    ED_CLASS_BODY(Renderer, BaseManager)
public:
    virtual void Initialize(Engine* engine) override;
    virtual void Deinitialize() override;

    void Update(float deltaSeconds);

    bool IsViewportSizeDirty() const;

    void ResizeViewport(glm::vec2 size);
    glm::u32vec2 GetViewportSize() const;

    void SubmitRenderCommand(const std::function<void(RenderingContext* context)>& command);

    template <class T>
    std::shared_ptr<T> GetTask() const;

    void SetSSAOEnabled(bool enabled);
    bool IsSSAOEnabled() const;

    void SetBloomEnabled(bool enabled);
    bool IsBloomEnabled() const;

    void SetUpsampleScale(float scale);
    float GetUpsampleScale() const;

    void SetAAMethod(AAMethod method);
    AAMethod GetAAMethod() const;

    void SetActiveRenderTarget(RenderTarget target);
    RenderTarget GetActiveRenderTarget() const;

    std::shared_ptr<RenderingContext> GetContext() const;
    std::shared_ptr<RenderGraph> GetGraph() const;

    std::shared_ptr<Texture2D> GetRenderTarget(RenderTarget target) const;
    std::shared_ptr<Texture2D> GetViewportTexture() const;

    float GetFarPlane() const;
	
    void SetCamera(const Camera& camera);
	void SetCamera(const glm::mat4& view, const glm::mat4& projection, glm::vec3 viewPosition);

	void SubmitFullScreenQuad();
	void SubmitQuad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
	void SubmitIcon(std::shared_ptr<Texture2D> texture, const glm::mat4& Transform);

	void BeginUIFrame();
	void EndUIFrame();
public:
    uint32_t StaticMeshesDrawn = 0;

    uint32_t PointLightsShaded = 0;
    uint32_t PointLightsShadowsDrawn = 0;

    uint32_t SpotLightsShaded = 0;
    uint32_t SpotLightsShadowsDrawn = 0;

    uint32_t DirectionalLightsShaded = 0;
    uint32_t DirectionalLightsShadowsDrawn = 0;

private:
    bool m_bSSAOEnabled = true;
    bool m_bIsBloomEnabled = false;

    float m_FarPlane = 500.0f;

    AAMethod m_AAMethod = AAMethod::TAA;

    float m_UpsampleScale = 1.0f;

    RenderTarget m_ActiveRenderTarget = RenderTarget::Resolution;

    std::shared_ptr<RenderGraph> m_Graph;

    std::queue<std::function<void(RenderingContext* context)>> m_Commands;

    Engine* m_Engine = nullptr;

    bool m_bIsViewportSizeDirty = true;
    glm::ivec2 m_ViewportSize = glm::ivec2(1);
    
    std::shared_ptr<RenderingContext> m_Context;

    std::shared_ptr<VertexBuffer> m_QuadVBO;
    std::shared_ptr<VertexBuffer> m_TextVBO;

    std::shared_ptr<CameraComponent> m_Camera;

    std::vector<std::shared_ptr<Component>> m_Components;
    std::vector<std::shared_ptr<StaticMeshComponent>> m_StaticMeshes;
    std::vector<std::shared_ptr<DirectionalLightComponent>> m_DirectionalLights;
    std::vector<std::shared_ptr<PointLightComponent>> m_PointLights;
    std::vector<std::shared_ptr<SpotLightComponent>> m_SpotLights;
};