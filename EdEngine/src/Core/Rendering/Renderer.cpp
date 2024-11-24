#include "Renderer.h"
#include "Core/Engine.h"
#include "Core/Scene.h"
#include "Helpers/GeometryBuilder.h"
#include "Helpers/RenderingHelper.h"
#include "Helpers/MathHelper.h"
#include "Helpers/FilesHelper.h"
#include "Core/Macros.h"
#include "imgui.h"
#include <random>

#include "Core/Components/StaticMeshComponent.h"
#include "Core/Components/PointLightComponent.h"
#include "Core/Components/SpotLightComponent.h"

#include "Core/Rendering/Buffers/VertexBuffer.h"

#include "RenderingContex.h"
#include "RenderPassSpecification.h"
#include "Buffers/VertexBufferLayout.h"
#include "Shader.h"

#include "Passes/GBufferPass.h"
#include "Passes/SSAO/SSAOMultiPass.h"
#include "Passes/SSAO/SSAOBlurPass.h"
#include "Passes/EmissionPass.h"
#include "Passes/AmbientPass.h"
#include "Passes/Lighting/DirectionalLight/DirectionalLightMultiPass.h"
#include "Passes/Lighting/SpotLight/SpotLightMultiPass.h"
#include "Passes/Lighting/PointLight/PointLightMultiPass.h"
#include "Passes/FXAAPass.h"
#include "Passes/TAAPass.h"
#include "Passes/Bloom/BloomMultiPass.h"
#include "Passes/ResolutionPass.h"
#include "Passes/GrayscalePass.h"
#include "Passes/Editor/IconsPass.h"

void Renderer::Initialize(Engine* engine)
{
	ED_LOG(Renderer, info, "Started initalizing Renderer")

    m_Engine = engine;

	m_Context = engine->GetWindow()->GetContext();

	//TODO: Maybe use xy of position as texture coordinates ?)
	float square[4 * 6] = {
		0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,

		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,
	};

	VertexBufferLayout textVBOlayout = {
		{ "position",           ShaderDataType::Float2 },
		{ "textureCoordinates", ShaderDataType::Float2 }
	};

	m_TextVBO = RenderingHelper::CreateVertexBuffer(nullptr, 20 * sizeof(float), textVBOlayout, BufferUsage::DynamicDraw);
	m_QuadVBO = RenderingHelper::CreateVertexBuffer(square, 24 * sizeof(float), textVBOlayout, BufferUsage::StaticDraw);

	{
		m_Graph = std::make_shared<RenderGraph>();
		m_Graph->Initilaize(std::static_pointer_cast<Renderer>(shared_from_this()));

		m_Camera = m_Engine->GetLoadedScene()->GetPlayerActor()->GetCameraComponent();
		m_Graph->DeclareObjectPtrParameter("Camera", m_Camera); // Level transitions ? Will it crash here ? ;) yes it did :)

		m_Graph->DeclareParameter("Scene.Component", m_Components);
		m_Graph->DeclareParameter("Scene.StaticMesh", m_StaticMeshes);
		m_Graph->DeclareParameter("Scene.PointLight", m_PointLights);
		m_Graph->DeclareParameter("Scene.DirectionalLight", m_DirectionalLights);
		m_Graph->DeclareParameter("Scene.SpotLight", m_SpotLights);

		m_Graph->AddPass<GBufferPass>();
		
		m_Graph->AddPass<SSAOMultiPass>();

		m_Graph->AddPass<AmbientPass>();
		m_Graph->AddPass<EmissionPass>();
		
		m_Graph->AddPass<DirectionalLightMultiPass>();
		m_Graph->AddPass<SpotLightMultiPass>();
		m_Graph->AddPass<PointLightMultiPass>();

		m_Graph->AddPass<FXAAPass>();
		m_Graph->AddPass<TAAPass>();

		m_Graph->AddPass<BloomMultiPass>();

		m_Graph->AddPass<ResolutionPass>();

		m_Graph->AddPass<GrayscalePass>();

		m_Graph->AddPass<IconsPass>();

		m_Graph->Build();
	}

	SetSSAOEnabled(m_bSSAOEnabled);
	SetBloomEnabled(m_bIsBloomEnabled);

	SetAAMethod(m_AAMethod);

	ED_LOG(Renderer, info, "Finished initalizing Renderer")
}

void Renderer::Deinitialize()
{
    
}

void Renderer::Update(float deltaSeconds)
{
	m_Context->SwapBuffers();

	std::shared_ptr<Scene> scene = m_Engine->GetLoadedScene();

	// TODO: This is probably not the best way to do it :)
	// A (me from future): Kinda is, actually, there is no way around collecting all the objects
	// from the scene and there is no unified/pretty way to do it). But it would be preferable
	// if data to be drawn was collected, not components from which it is extracted,
	// there is no reason for the renderer to know anything about components.

	m_Components = scene->GetAllComponents();
	m_StaticMeshes.clear();
	m_DirectionalLights.clear();
	m_SpotLights.clear();
	m_PointLights.clear();

	StaticMeshesDrawn = 0;

	PointLightsShaded = 0;
	PointLightsShadowsDrawn = 0;

	SpotLightsShaded = 0;
	SpotLightsShadowsDrawn = 0;

	DirectionalLightsShaded = 0;
	DirectionalLightsShadowsDrawn = 0;

	for (const std::shared_ptr<Component>& component : m_Components)
	{
		switch (component->GetType())
		{
		case ComponentType::StaticMesh:
			m_StaticMeshes.push_back(std::static_pointer_cast<StaticMeshComponent>(component));
			break;
		case ComponentType::DirectionalLight:
			m_DirectionalLights.push_back(std::static_pointer_cast<DirectionalLightComponent>(component));
			break;
		case ComponentType::SpotLight:
			m_SpotLights.push_back(std::static_pointer_cast<SpotLightComponent>(component));
			break;
		case ComponentType::PointLight:
			m_PointLights.push_back(std::static_pointer_cast<PointLightComponent>(component));
			break;
		}
	}

	if (m_bIsViewportSizeDirty)
	{
		Camera& camera = m_Engine->GetLoadedScene()->GetPlayerActor()->GetCameraComponent()->GetCamera();
		camera.SetProjection(90.0f, 1.0f * m_ViewportSize.x / m_ViewportSize.y, 1.0f, m_FarPlane);
	}

	m_Graph->Update(deltaSeconds);

	while (!m_Commands.empty()) {
		m_Commands.front()(m_Context.get());
		m_Commands.pop();
	}

	m_bIsViewportSizeDirty = false;
}

bool Renderer::IsViewportSizeDirty() const
{
	return m_bIsViewportSizeDirty;
}

void Renderer::ResizeViewport(glm::vec2 size)
{
	if (m_ViewportSize != glm::ivec2(size))
	{
		m_bIsViewportSizeDirty = true;
		m_ViewportSize = size;
	}
}

glm::u32vec2 Renderer::GetViewportSize() const
{
	return m_ViewportSize;
}

void Renderer::SubmitRenderCommand(const std::function<void(RenderingContext* context)>& command)
{
    m_Commands.push(command);
}

std::shared_ptr<RenderingContext> Renderer::GetContext() const
{
	return m_Context;
}

std::shared_ptr<RenderGraph> Renderer::GetGraph() const
{
	return m_Graph;
}

void Renderer::SetSSAOEnabled(bool enabled)
{
	m_bSSAOEnabled = enabled;
}

bool Renderer::IsSSAOEnabled() const
{
    return m_bSSAOEnabled;
}

void Renderer::SetBloomEnabled(bool enabled)
{
	m_bIsBloomEnabled = enabled;
}

bool Renderer::IsBloomEnabled() const
{
	return m_bIsBloomEnabled;
}

void Renderer::SetUpsampleScale(float scale)
{
	m_UpsampleScale = scale;
}

float Renderer::GetUpsampleScale() const
{
	return m_UpsampleScale;
}

AAMethod Renderer::GetAAMethod() const
{
	return m_AAMethod;
}

void Renderer::SetActiveRenderTarget(RenderTarget target)
{
	m_ActiveRenderTarget = target;
}

RenderTarget Renderer::GetActiveRenderTarget() const
{
	return m_ActiveRenderTarget;
}

void Renderer::SetAAMethod(AAMethod method)
{
	m_AAMethod = method;
}

std::shared_ptr<Texture2D> Renderer::GetRenderTarget(RenderTarget target) const
{
	switch (target)
	{
	case RenderTarget::GAlbedo:                   return m_Graph->GetPass<GBufferPass>()->GetParameters().DrawFramebuffer->GetAttachment<Texture2D>(0);
	case RenderTarget::GPosition:                 return m_Graph->GetPass<GBufferPass>()->GetParameters().DrawFramebuffer->GetAttachment<Texture2D>(1);
	case RenderTarget::GNormal:                   return m_Graph->GetPass<GBufferPass>()->GetParameters().DrawFramebuffer->GetAttachment<Texture2D>(2);
	case RenderTarget::GRougnessMetalicEmission:  return m_Graph->GetPass<GBufferPass>()->GetParameters().DrawFramebuffer->GetAttachment<Texture2D>(3);
	case RenderTarget::GVelocity:                 return m_Graph->GetPass<GBufferPass>()->GetParameters().DrawFramebuffer->GetAttachment<Texture2D>(4);
	case RenderTarget::GDepth:                    return m_Graph->GetPass<GBufferPass>()->GetParameters().DrawFramebuffer->GetDepthAttachment<Texture2D>();

	case RenderTarget::SSAO:                      return m_Graph->GetPass<SSAOMultiPass>()->GetPass<SSAOBlurPass>()->GetParameters().DrawFramebuffer->GetAttachment<Texture2D>(0);

	case RenderTarget::Diffuse:                   return m_Graph->GetPass<AmbientPass>()->GetParameters().DrawFramebuffer->GetAttachment<Texture2D>(0);
	case RenderTarget::Specular:                  return m_Graph->GetPass<AmbientPass>()->GetParameters().DrawFramebuffer->GetAttachment<Texture2D>(1);
	case RenderTarget::Light:                     return m_Graph->GetPass<AmbientPass>()->GetParameters().DrawFramebuffer->GetAttachment<Texture2D>(2);

	case RenderTarget::Bloom:                     return m_Graph->GetPass<ResolutionPass>()->GetParameters().Bloom;

	case RenderTarget::AAOutput:                  return m_Graph->GetPass<TAAPass>()->GetParameters().DrawFramebuffer->GetAttachment<Texture2D>(0);
	case RenderTarget::Resolution:                return m_Graph->GetPass<ResolutionPass>()->GetParameters().DrawFramebuffer->GetAttachment<Texture2D>(0);
	default:
		ED_ASSERT(0, "Unsuppoerted target")
	}
}

std::shared_ptr<Texture2D> Renderer::GetViewportTexture() const
{
	return GetRenderTarget(m_ActiveRenderTarget);
}

float Renderer::GetFarPlane() const
{
	return m_FarPlane;
}

void Renderer::BeginUIFrame()
{
    m_Context->BeginUIFrame();
}

void Renderer::EndUIFrame()
{
	 m_Context->EndUIFrame();
}

void Renderer::SetCamera(const Camera& camera)
{
	glm::mat4 view = camera.GetView();
	glm::mat4 projection = camera.GetProjection();
	
	m_Context->SetShaderDataMat4("u_ViewMatrix", view);
	m_Context->SetShaderDataMat4("u_ProjectionMatrix", projection);
	m_Context->SetShaderDataMat4("u_ProjectionViewMatrix", projection * view);
	m_Context->SetShaderDataMat4("u_InvProjectionViewMatrix", glm::inverse(projection * view));

	m_Context->SetShaderDataFloat3("u_ViewPosition", camera.GetPosition());

	m_Context->SetShaderDataFloat("u_FarPlane", camera.GetFar());
}

void Renderer::SetCamera(const glm::mat4& view, const glm::mat4& projection, glm::vec3 viewPosition)
{
	m_Context->SetShaderDataMat4("u_ViewMatrix", view);
	m_Context->SetShaderDataMat4("u_ProjectionMatrix", projection);
	m_Context->SetShaderDataMat4("u_ProjectionViewMatrix", projection * view);
	m_Context->SetShaderDataFloat3("u_ViewPosition", viewPosition);
}

void Renderer::SubmitFullScreenQuad()
{
	SubmitQuad(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f);
}

void Renderer::SubmitQuad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
{
	float quad[24] = {
		x1, y1, 0.0f, 0.0f,
		x2, y2, 0.0f, 1.0f,
		x3, y3, 1.0f, 1.0f,

		x3, y3, 1.0f, 1.0f,
		x4, y4, 1.0f, 0.0f,
		x1, y1, 0.0f, 0.0f
	};

	m_QuadVBO->SetData(quad, 24 * sizeof(float), BufferUsage::DynamicDraw);
    m_Context->SetVertexBuffer(m_QuadVBO);
	m_Context->SetIndexBuffer(nullptr);

    m_Context->Draw();
}

void Renderer::SubmitIcon(std::shared_ptr<Texture2D> texture, const glm::mat4& transform)
{
	float quad[24] = {
		-1.0f, -1.0f, 0.0f, 0.0f,
		-1.0f,  1.0f, 0.0f, 1.0f,
         1.0f,  1.0f, 1.0f, 1.0f,

         1.0f,  1.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         -1.0f, -1.0f, 0.0f, 0.0f
	};

	m_QuadVBO->SetData(quad, 24 * sizeof(float), BufferUsage::DynamicDraw);
	m_Context->SetVertexBuffer(m_QuadVBO);

    m_Context->SetShaderDataMat4("u_ModelTransform", transform);
    m_Context->SetShaderDataTexture("u_IconTexture", texture);

    m_Context->Draw();
}