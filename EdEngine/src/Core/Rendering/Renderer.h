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

class RenderTask;

class Renderer: public BaseManager
{
public:
    virtual void Initialize(Engine* engine) override;
    virtual void Deinitialize() override;

    void Update();

    void ResizeViewport(glm::vec2 size);

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

	int32_t m_LightCount = 0;

	glm::vec4 m_Up;

	std::shared_ptr<VertexBuffer> m_QuadVBO;
	std::shared_ptr<VertexBuffer> m_TextVBO;

	std::shared_ptr<VertexBuffer> m_PointLightMeshVBO;
	std::shared_ptr<IndexBuffer> m_PointLightMeshIBO;
public:
	void BeginRenderPass(const std::string& name, const std::shared_ptr<BaseFramebuffer>& framebuffer, const std::shared_ptr<Shader>& shader, const glm::mat4& view = glm::mat4(1.0f), const glm::mat4 projection = glm::mat4(1.0f), glm::vec3 viewPosition = glm::vec3(0.0f));
	void BeginRenderPass(RenderPassSpecification& specification, const glm::mat4& view = glm::mat4(1.0f), const glm::mat4& projection = glm::mat4(1.0f));

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

template<class T>
inline std::shared_ptr<T> Renderer::GetTask() const
{
	for (const std::shared_ptr<RenderTask>& task : m_Tasks)
	{
		if (std::shared_ptr<T> casted = std::dynamic_pointer_cast<T>(task))
		{
			return casted;
		}
	}

	return nullptr;
}
