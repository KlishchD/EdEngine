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
class SpotLightComponent;

class VertexBuffer;
class IndexBuffer;

class Framebuffer;
class CubeFramebuffer;
class Shader;

class Texture;
class Texture2D;

class StaticMesh;
class StaticSubmesh;

class RenderTask;

class Renderer: public BaseManager
{
    const int32_t PointLightMeshSectorsCount = 30;
    const int32_t PointLightMeshStackCount = 30;
    const int32_t SpotLightMeshSectorsCount = 50;
    const glm::vec3 SpotLightMeshDirection = glm::vec3(0.0f, -1.0f, 0.0f);
public:
    virtual void Initialize(Engine* engine) override;
    virtual void Deinitialize() override;

    void Update();

    void ResizeViewport(glm::vec2 size);

    void SubmitRenderCommand(const std::function<void(RenderingContext* context)>& command);

    template <class T>
    std::shared_ptr<T> GetTask() const;

    void SetSSDOEnabled(bool enabled);
    bool IsSSDOEnabled() const;

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

    std::shared_ptr<RenderingContext> GetRenderContext() const;

    std::shared_ptr<Framebuffer> GetLightFramebuffer() const;
    std::shared_ptr<Framebuffer> GetAntiAliasingFramebuffer() const;

    std::shared_ptr<Texture2D> GetAntiAliasingOutputTexture() const;

    std::shared_ptr<Texture2D> GetRenderTarget(RenderTarget target) const;
    std::shared_ptr<Texture2D> GetViewportTexture() const;

    float GetFarPlane() const;

private:
    std::shared_ptr<Framebuffer> m_LightFramebuffer;
    std::shared_ptr<Framebuffer> m_AAFramebuffer;
    
    bool m_bIsSSDOEnabled = false;
    bool m_bSSAOEnabled = true;
    bool m_bIsBloomEnabled = false;

    float m_FarPlane = 15000.0f;

    AAMethod m_AAMethod = AAMethod::TAA;

    float m_UpsampleScale = 1.0f;

    RenderTarget m_ActiveRenderTarget = RenderTarget::Resolution;

    std::vector<std::shared_ptr<RenderTask>> m_Tasks;

    std::queue<std::function<void(RenderingContext* context)>> m_Commands;

    Engine* m_Engine = nullptr;

    bool m_bIsViewportSizeDirty = true;
    glm::ivec2 m_ViewportSize = glm::ivec2(1);
private:
    std::shared_ptr<RenderingContext> m_Context;

    RenderPassSpecification* m_Specification;

    glm::mat4 m_View;
    glm::mat4 m_Projection;
    RenderPassSpecification m_TemporarySpecification;

    glm::vec4 m_Up;

    std::shared_ptr<VertexBuffer> m_QuadVBO;
    std::shared_ptr<VertexBuffer> m_TextVBO;

    std::vector<glm::vec3> m_PointLightVerticies;
    std::shared_ptr<VertexBuffer> m_PointLightMeshVBO;
    std::shared_ptr<IndexBuffer> m_PointLightMeshIBO;

    std::vector<glm::vec3> m_SpotLightVerticies;
    std::shared_ptr<VertexBuffer> m_SpotLightMeshVBO;
    std::shared_ptr<IndexBuffer> m_SpotLightMeshIBO;
public:
	void BeginRenderPass(const std::string& name, std::shared_ptr<BaseFramebuffer> framebuffer, std::shared_ptr<Shader> shader, const glm::mat4& view = glm::mat4(1.0f), const glm::mat4 projection = glm::mat4(1.0f), glm::vec3 viewPosition = glm::vec3(0.0f));
	void BeginRenderPass(RenderPassSpecification& specification, const glm::mat4& view = glm::mat4(1.0f), const glm::mat4& projection = glm::mat4(1.0f));

    void SetNewCameraInformation(const glm::mat4& view, const glm::mat4& projection, glm::vec3 viewPosition);

    bool IsLightMeshVisible(std::shared_ptr<PointLightComponent> light, Camera* camera) const;
    bool IsLightMeshVisible(std::shared_ptr<SpotLightComponent> light, Camera* camera) const;

    void SubmitLightMesh(std::shared_ptr<PointLightComponent> light);
    void SubmitLightMesh(std::shared_ptr<SpotLightComponent> light);

    void SubmitLightMeshWireframe(std::shared_ptr<PointLightComponent> light);
    void SubmitLightMeshWireframe(std::shared_ptr<SpotLightComponent> light);

    void SubmitMesh(std::shared_ptr<StaticMesh> mesh, const Transform& transform, const Transform& previousTransform);
    void SubmitSubmesh(std::shared_ptr<StaticSubmesh> submesh, const Transform& transform, const Transform& previousTransform);

    void SubmitMeshesRaw(const std::vector<std::shared_ptr<Component>>& components);
    void SubmitMeshRaw(std::shared_ptr<StaticMesh> mesh, const Transform& transform, const Transform& previousTransform);
    void SubmitSubmeshRaw(std::shared_ptr<StaticSubmesh> submesh, const Transform& transform, const Transform& previousTransform);

    void SubmitQuad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
    void SubmitIcon(std::shared_ptr<Texture2D> texture, const glm::mat4& Transform);

    void BeginUIFrame();
    void EndUIFrame();

    void EndRenderPass();
 private:
    bool IsLightMeshVisible(const std::vector<glm::vec3>& vertices, const Transform& transform, Camera* camera) const;
};

template<class T>
inline std::shared_ptr<T> Renderer::GetTask() const
{
	for (std::shared_ptr<RenderTask> task : m_Tasks)
	{
		if (std::shared_ptr<T> casted = std::dynamic_pointer_cast<T>(task))
		{
			return casted;
		}
	}

	return nullptr;
}
