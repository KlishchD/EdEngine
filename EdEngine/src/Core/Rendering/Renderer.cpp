#include "Renderer.h"
#include "Core/Engine.h"
#include "Core/Scene.h"
#include "Utils/GeometryBuilder.h"
#include "Utils/RenderingHelper.h"
#include "Utils/MathHelper.h"
#include "Utils/Files.h"
#include "Core/Macros.h"
#include "imgui.h"
#include <random>

#include "Core/Components/StaticMeshComponent.h"
#include "Core/Components/PointLightComponent.h"
#include "Core/Components/SpotLightComponent.h"

#include "Core/Rendering/Buffers/VertexBuffer.h"

#include "RenderingContex.h"
#include "RenderPassSpecification.h"
#include "Framebuffers/Framebuffer.h"
#include "Framebuffers/CubeFramebuffer.h"
#include "Buffers/VertexBufferLayout.h"
#include "Shader.h"

#include "Tasks/RenderTask.h"
#include "Tasks/GBufferRenderTask.h"
#include "Tasks/SSAORenderTask.h"
#include "Tasks/EmissionRenderTask.h"
#include "Tasks/PointLightRenderTask.h"
#include "Tasks/AmbientRenderTask.h"
#include "Tasks/SSDORenderTask.h"
#include "Tasks/FXAARenderTask.h"
#include "Tasks/TAARenderTask.h"
#include "Tasks/BloomRenderTask.h"
#include "Tasks/ResolutionRenderTask.h"
#include "Tasks/SpotLightRenderTask.h"

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

	VertexBufferLayout lightVBOLayout = { { "position", ShaderDataType::Float3 } };

	{
		auto [vertices, indices] = GeometryBuilder::MakeSphere(1, PointLightMeshSectorsCount, PointLightMeshStackCount);

		m_PointLightMeshVBO = RenderingHelper::CreateVertexBuffer(vertices.data(), sizeof(float) * vertices.size(), lightVBOLayout, BufferUsage::StaticDraw);
		m_PointLightMeshIBO = RenderingHelper::CreateIndexBuffer(indices.data(), sizeof(int32_t) * indices.size(), BufferUsage::StaticDraw);
	}

	{
		auto [vertices, indices] = GeometryBuilder::MakeCone(SpotLightMeshSectorsCount);

		m_SpotLightMeshVBO = RenderingHelper::CreateVertexBuffer(vertices.data(), sizeof(float) * vertices.size(), lightVBOLayout, BufferUsage::StaticDraw);
		m_SpotLightMeshIBO = RenderingHelper::CreateIndexBuffer(indices.data(), sizeof(int32_t) * indices.size(), BufferUsage::StaticDraw);
	}

	m_LightFramebuffer = RenderingHelper::CreateFramebuffer(1, 1);
	m_LightFramebuffer->CreateAttachment(FramebufferAttachmentType::Color16);
	m_LightFramebuffer->CreateAttachment(FramebufferAttachmentType::Color16);
	m_LightFramebuffer->CreateAttachment(FramebufferAttachmentType::Color16);

	{
		Texture2DImportParameters pararmeters = RenderingHelper::GetDefaultNormalTexture2DImportParameters("");

		Texture2DData data;
		data.Width = 1;
		data.Height = 1;
		data.Data = nullptr;

		m_AAFramebuffer = RenderingHelper::CreateFramebuffer(1, 1);
		m_AAFramebuffer->CreateAttachment(FramebufferAttachmentType::Color16);
	}

	{
		m_Tasks.push_back(std::make_shared<GBufferRenderTask>());

		m_Tasks.push_back(std::make_shared<SSAORenderTask>());

		m_Tasks.push_back(std::make_shared<EmissionRenderTask>());

		m_Tasks.push_back(std::make_shared<SpotLightRenderTask>());
		m_Tasks.push_back(std::make_shared<PointLightRenderTask>());

		m_Tasks.push_back(std::make_shared<SSDORenderTask>());

		m_Tasks.push_back(std::make_shared<AmbientRenderTask>());

		m_Tasks.push_back(std::make_shared<FXAARenderTask>());
		m_Tasks.push_back(std::make_shared<TAARenderTask>());

		m_Tasks.push_back(std::make_shared<BloomRenderTask>());

		m_Tasks.push_back(std::make_shared<ResolutionRenderTask>());

		for (const std::shared_ptr<RenderTask>& task : m_Tasks)
		{
			task->Setup(this);
		}
	}

	SetSSAOEnabled(m_bSSAOEnabled);
	SetSSDOEnabled(m_bIsSSDOEnabled);
	SetBloomEnabled(m_bIsBloomEnabled);

	SetAAMethod(m_AAMethod);

	ED_LOG(Renderer, info, "Finished initalizing Renderer")
}

void Renderer::Deinitialize()
{
    
}

void Renderer::Update()
{
	m_Context->SwapBuffers();

	if (m_bIsViewportSizeDirty)
	{
		for (const std::shared_ptr<RenderTask>& task : m_Tasks)
		{
			task->Resize(m_ViewportSize, m_UpsampleScale);
		}

		m_LightFramebuffer->Resize(m_ViewportSize.x * m_UpsampleScale, m_ViewportSize.y * m_UpsampleScale);
		m_AAFramebuffer->Resize(m_ViewportSize.x * m_UpsampleScale, m_ViewportSize.y * m_UpsampleScale);

		m_Engine->GetCamera()->SetProjection(90.0f, 1.0f * m_ViewportSize.x / m_ViewportSize.y, 1.0f, m_FarPlane);

		m_bIsViewportSizeDirty = false;
	}

	std::shared_ptr<Scene> scene = m_Engine->GetLoadedScene();
	Camera* camera = m_Engine->GetCamera();

	std::vector<std::shared_ptr<Component>> components = scene->GetAllComponents();

	for (const std::shared_ptr<RenderTask>& task : m_Tasks)
	{
		task->Run(components, camera);
	}

	while (m_Commands.size()) {
		m_Commands.front()(m_Context.get());
		m_Commands.pop();
	}
}

void Renderer::ResizeViewport(glm::vec2 size)
{
	if (m_ViewportSize != glm::ivec2(size))
	{
		m_bIsViewportSizeDirty = true;
		m_ViewportSize = size;
	}
}

void Renderer::SubmitRenderCommand(const std::function<void(RenderingContext* context)>& command)
{
    m_Commands.push(command);
}

std::shared_ptr<RenderingContext> Renderer::GetRenderContext() const
{
	return m_Context;
}

std::shared_ptr<Framebuffer> Renderer::GetLightFramebuffer() const
{
	return m_LightFramebuffer;
}

std::shared_ptr<Framebuffer> Renderer::GetAntiAliasingFramebuffer() const
{
	return m_AAFramebuffer;
}

std::shared_ptr<Texture2D> Renderer::GetAntiAliasingOutputTexture() const
{
	return m_AAMethod == AAMethod::None ? GetRenderTarget(RenderTarget::Light) : std::static_pointer_cast<Texture2D>(m_AAFramebuffer->GetAttachment(0));
}

void Renderer::SetSSDOEnabled(bool enabled)
{
	m_bIsSSDOEnabled = enabled;
	GetTask<SSDORenderTask>()->SetEnabled(enabled);
}

bool Renderer::IsSSDOEnabled() const
{
	return m_bIsSSDOEnabled;
}

void Renderer::SetSSAOEnabled(bool enabled)
{
	m_bSSAOEnabled = enabled;
	GetTask<SSAORenderTask>()->SetEnabled(enabled);
}

bool Renderer::IsSSAOEnabled() const
{
    return m_bSSAOEnabled;
}

void Renderer::SetBloomEnabled(bool enabled)
{
	m_bIsBloomEnabled = enabled;
	GetTask<BloomRenderTask>()->SetEnabled(enabled);
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

	switch (method)
	{
	case AAMethod::None:
		GetTask<FXAARenderTask>()->SetEnabled(false);
		GetTask<TAARenderTask>()->SetEnabled(false);
		break;
	case AAMethod::TAA:
		GetTask<FXAARenderTask>()->SetEnabled(false);
		GetTask<TAARenderTask>()->SetEnabled(true);
		break;
	case AAMethod::FXAA:
		GetTask<FXAARenderTask>()->SetEnabled(true);
		GetTask<TAARenderTask>()->SetEnabled(false);
		break;
	}
}

std::shared_ptr<Texture2D> Renderer::GetRenderTarget(RenderTarget target) const
{
	switch (target)
	{
	case RenderTarget::GAlbedo:                   return GetTask<GBufferRenderTask>()->GetAlbedoTexture();
	case RenderTarget::GPosition:                 return GetTask<GBufferRenderTask>()->GetPositionTexture();
	case RenderTarget::GNormal:                   return GetTask<GBufferRenderTask>()->GetNormalTexture();
	case RenderTarget::GRougnessMetalicEmission:  return GetTask<GBufferRenderTask>()->GetRoughnessMetalicEmissionTexture();
	case RenderTarget::GVelocity:                 return GetTask<GBufferRenderTask>()->GetVelocityTexture();
	case RenderTarget::GDepth:                    return GetTask<GBufferRenderTask>()->GetDepthTexture();

	case RenderTarget::SSAO:                      return GetTask<SSAORenderTask>()->GetBluredTexture();

	case RenderTarget::Diffuse:                   return std::static_pointer_cast<Texture2D>(m_LightFramebuffer->GetAttachment(0));
	case RenderTarget::Specular:                  return std::static_pointer_cast<Texture2D>(m_LightFramebuffer->GetAttachment(1));
	case RenderTarget::Light:                     return std::static_pointer_cast<Texture2D>(m_LightFramebuffer->GetAttachment(2));

	case RenderTarget::SSDO:                      return GetTask<SSDORenderTask>()->GetTexutre();

	case RenderTarget::Bloom:                     return GetTask<BloomRenderTask>()->GetTexture();

	case RenderTarget::AAOutput:                  return std::static_pointer_cast<Texture2D>(m_AAFramebuffer->GetAttachment(0));
	case RenderTarget::Resolution:                return GetTask<ResolutionRenderTask>()->GetTexture();
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

void Renderer::BeginRenderPass(const std::string& name, const std::shared_ptr<BaseFramebuffer>& framebuffer, const std::shared_ptr<Shader>& shader, const glm::mat4& view, glm::mat4 projection, glm::vec3 viewPosition)
{
	m_TemporarySpecification = RenderPassSpecification();

	m_TemporarySpecification.Name = name;
	m_TemporarySpecification.Framebuffer = framebuffer;
	m_TemporarySpecification.Shader = shader;
	m_TemporarySpecification.ViewPosition = viewPosition;

	BeginRenderPass(m_TemporarySpecification, view, projection);
}

void Renderer::BeginRenderPass(RenderPassSpecification& specification, const glm::mat4& view, const glm::mat4& projection)
{
	m_Specification = &specification;

	m_View = view;
	m_Projection = projection;

	m_Context->SetFramebuffer(specification.Framebuffer);

	m_Context->SetShader(specification.Shader);
	m_Context->SetShaderDataMat4("u_ViewMatrix", m_View);
	m_Context->SetShaderDataMat4("u_ProjectionMatrix", m_Projection);
	m_Context->SetShaderDataMat4("u_ProjectionViewMatrix", m_Projection * m_View);
	m_Context->SetShaderDataMat4("u_InvProjectionViewMatrix", glm::inverse(m_Projection * m_View));
	m_Context->SetShaderDataFloat3("u_ViewPosition", m_Specification->ViewPosition);
	m_Context->SetShaderDataFloat2("u_ScreenSize", m_Specification->Framebuffer->GetWidth(), m_Specification->Framebuffer->GetHeight());

	if (specification.bUseBlending)
	{
		m_Context->EnableBlending(specification.SourceFactor, specification.DestinationFactor);
	}
	else
	{
        m_Context->DisableBlending();
	}

	if (specification.bUseDepthTesting)
	{
        m_Context->EnableDethTest(specification.DepthFunction);
	}
	else
	{
        m_Context->DisableDethTest();
	}

	if (specification.bClearColors)
	{
        m_Context->ClearColorTarget();
	}

	if (specification.bClearDepth)
	{
        m_Context->ClearDepthTarget();
	}
}

void Renderer::SetNewCameraInformation(const glm::mat4& view, const glm::mat4& projection, glm::vec3 viewPosition)
{
	m_View = view;
	m_Projection = projection;

	m_Specification->ViewPosition = viewPosition;

	m_Context->SetShaderDataMat4("u_ViewMatrix", m_View);
	m_Context->SetShaderDataMat4("u_ProjectionMatrix", m_Projection);
	m_Context->SetShaderDataMat4("u_ProjectionViewMatrix", m_Projection * m_View);
	m_Context->SetShaderDataFloat3("u_ViewPosition", viewPosition);
}

void Renderer::SubmitLightMesh(const std::shared_ptr<PointLightComponent>& light)
{
	m_Context->EnableFaceCulling(Face::Front);

	Transform transform = light->GetWorldTransform();
	transform.SetScale(glm::vec3(light->GetRadius()));

	m_Context->SetShaderDataMat4("u_ModelMatrix", transform.GetMatrix());

	m_Context->SetVertexBuffer(m_PointLightMeshVBO);
	m_Context->SetIndexBuffer(m_PointLightMeshIBO);
	m_Context->Draw();

	m_Context->DisableFaceCulling();
}

void Renderer::SubmitLightMesh(const std::shared_ptr<SpotLightComponent>& light)
{
	m_Context->EnableFaceCulling(Face::Front);

	const float angle = light->GetOuterAngle();
	const float length = light->GetMaxDistance();
	const float radius = glm::tan(angle) * length;

	Transform transform = light->GetWorldTransform();
	transform.SetScale(glm::vec3(radius, length, radius));

	glm::vec3 directionTransformed = glm::normalize(transform.GetRotation() * SpotLightMeshDirection);
	m_Context->SetShaderDataFloat3("u_SpotLight.Direction", directionTransformed);

	m_Context->SetShaderDataMat4("u_ModelMatrix", transform.GetMatrix());

	m_Context->SetVertexBuffer(m_SpotLightMeshVBO);
	m_Context->SetIndexBuffer(m_SpotLightMeshIBO);
	m_Context->Draw();

	m_Context->DisableFaceCulling();
}

void Renderer::SubmitLightMeshWireframe(const std::shared_ptr<PointLightComponent>& light)
{
	m_Context->EnableFaceCulling();

	Transform transform = light->GetWorldTransform();
	transform.SetScale(glm::vec3(light->GetRadius()));

	m_Context->SetShaderDataMat4("u_ModelMatrix", transform.GetMatrix());

	m_Context->SetVertexBuffer(m_PointLightMeshVBO);
	m_Context->SetIndexBuffer(m_PointLightMeshIBO);
	m_Context->Draw(DrawMode::LineStrip);

	m_Context->DisableFaceCulling();
}

void Renderer::SubmitLightMeshWireframe(const std::shared_ptr<SpotLightComponent>& light)
{
	const float angle = light->GetOuterAngle();
	const float length = light->GetMaxDistance();
	const float radius = glm::tan(angle) * length;

	Transform transform = light->GetWorldTransform();
	transform.SetScale(glm::vec3(radius, length, radius));

	m_Context->SetShaderDataMat4("u_ModelMatrix", transform.GetMatrix());

	m_Context->SetVertexBuffer(m_SpotLightMeshVBO);
	m_Context->SetIndexBuffer(m_SpotLightMeshIBO);
	m_Context->Draw(DrawMode::LineStrip);
}

void Renderer::SubmitMesh(const std::shared_ptr<StaticMesh>& mesh, const Transform& transform, const Transform& previousTransform)
{
	for (const std::shared_ptr<StaticSubmesh>& submesh : mesh->GetSubmeshes())
	{
		SubmitSubmesh(submesh, transform, previousTransform);
	}
}

void Renderer::SubmitSubmesh(const std::shared_ptr<StaticSubmesh>& submesh, const Transform& transform, const Transform& previousTransform)
{
	if (std::shared_ptr<Material> material = submesh->GetMaterial()) {
        m_Context->SetVertexBuffer(submesh->GetVertexBuffer());
        m_Context->SetIndexBuffer(submesh->GetIndexBuffer());
		
        material->SetShaderData(m_Context);

		m_Context->SetShaderDataMat4("u_PreviousModelMatrix", previousTransform.GetMatrix());
		m_Context->SetShaderDataMat4("u_ModelMatrix", transform.GetMatrix());
		m_Context->SetShaderDataMat3("u_NormalMatrix", transform.GetInversedTransposedMatrix());

        m_Context->Draw();
	}
}

void Renderer::SubmitMeshesRaw(const std::vector<std::shared_ptr<Component>>& components)
{
	for (const std::shared_ptr<Component>& component : components)
	{
		if (component->GetType() == ComponentType::StaticMesh)
		{
			std::shared_ptr<StaticMeshComponent> mesh = std::static_pointer_cast<StaticMeshComponent>(component);
			SubmitMeshRaw(mesh->GetStaticMesh(), mesh->GetWorldTransform(), mesh->GetPreviousWorldTransform());
		}
	}
}

void Renderer::SubmitMeshRaw(const std::shared_ptr<StaticMesh>& mesh, const Transform& transform, const Transform& previousTransform)
{
	if (mesh)
	{
		for (const std::shared_ptr<StaticSubmesh>& submesh : mesh->GetSubmeshes())
		{
			SubmitSubmeshRaw(submesh, transform, previousTransform);
		}
	}
}

void Renderer::SubmitSubmeshRaw(const std::shared_ptr<StaticSubmesh>& submesh, const Transform& transform, const Transform& previousTransform)
{
	if (std::shared_ptr<Material> material = submesh->GetMaterial()) {
		m_Context->SetVertexBuffer(submesh->GetVertexBuffer());
		m_Context->SetIndexBuffer(submesh->GetIndexBuffer());

        m_Context->SetShaderDataMat4("u_ModelMatrix", transform.GetMatrix());
        m_Context->SetShaderDataMat4("u_PreviousModelMatrix", previousTransform.GetMatrix());

        m_Context->Draw();
	}
}

void Renderer::SubmitQuad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
{
	float quad[32] = {
		x1, y1, 0.0f, 0.0f,
		x2, y2, 0.0f, 1.0f,
		x3, y3, 1.0f, 1.0f,

		x3, y3, 1.0f, 1.0f,
		x4, y4, 1.0f, 0.0f,
		x1, y1, 0.0f, 0.0f
	};

	m_QuadVBO->SetData(quad, 32 * sizeof(float), BufferUsage::DynamicDraw);
    m_Context->SetVertexBuffer(m_QuadVBO);
	m_Context->SetIndexBuffer(nullptr);

    m_Context->Draw();
}

void Renderer::SubmitIcon(const std::shared_ptr<Texture2D>& texture, const glm::mat4& transform)
{
	float quad[32] = {
		-1.0f, -1.0f, 0.0f, 0.0f,
		-1.0f,  1.0f, 0.0f, 1.0f,
         1.0f,  1.0f, 1.0f, 1.0f,

         1.0f,  1.0f, 1.0f, 1.0f,
         1.0f, -1.0f, 1.0f, 0.0f,
         -1.0f, -1.0f, 0.0f, 0.0f
	};

	m_QuadVBO->SetData(quad, 32 * sizeof(float), BufferUsage::DynamicDraw);
	m_Context->SetVertexBuffer(m_QuadVBO);

    m_Context->SetShaderDataMat4("u_ModelTransform", transform);
    m_Context->SetShaderDataTexture("u_IconTexture", texture);

    m_Context->Draw();
}

void Renderer::EndRenderPass()
{
    m_Context->SetDefaultFramebuffer();

	m_Specification = nullptr;
}


/*
void RendererAPI::SetFramebuffer(const std::shared_ptr<Framebuffer>& framebuffer)
{
	if (m_Framebuffer)
	{
		m_Framebuffer->Unbind();
	}

	m_Framebuffer = framebuffer;


	if (m_Framebuffer)
	{
		m_Framebuffer->Bind();
	}
}

void RendererAPI::SubmitComponent(const std::shared_ptr<Component>& component, const glm::mat4& Transform)
{
	glm::mat4 componentTransform = glm::mat4(1);

	if (component->GetType() == ComponentType::StaticMesh)
	{
		std::shared_ptr<StaticMeshComponent> staticMeshComponent = std::static_pointer_cast<StaticMeshComponent>(component);
		SubmitMesh(staticMeshComponent, Transform);
	}

	for (const std::shared_ptr<Component>& child: component->GetChildren())
	{
		SubmitComponent(child, Transform * componentTransform);
	}
}*/

/*
void RendererAPI::SubmitScene(const std::shared_ptr<Scene>& scene)
{
	std::vector<std::shared_ptr<Component>> components = scene->GetAllComponents();
	for (const auto& component: components)
	{
		if (component->GetType() == ComponentType::PointLight)
		{
			AddLight(std::static_pointer_cast<PointLightComponent>(component));
		}
	}

	for (const auto& actor: scene->GetActors())
	{
		for (const std::shared_ptr<Component>& component: actor->GetComponents())
		{
			SubmitComponent(component, actor->GetTransform());
		}
	}

#ifdef WITH_EDITOR
	for (const auto& component: components)
	{
		if (component->GetType() == ComponentType::PointLight)
		{
			std::shared_ptr<PointLightComponent> pointLightComponent = std::static_pointer_cast<PointLightComponent>(component);
			SubmitIcon(m_LightIcon, pointLightComponent->GetTransform());
		}
	}
#endif

}
*/

/*
void RendererAPI::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray, const glm::mat4& Transform)
{
	Submit(shader, *vertexArray, Transform);
}

void RendererAPI::Submit(const std::shared_ptr<Shader>& shader, const VertexArray& vertexArray, const glm::mat4& Transform)
{
	shader->SetMat4("u_CameraMatrix", m_ProjectionViewMatrix);
	shader->SetMat4("u_Transformation", Transform);
	vertexArray.Bind();
	shader->Bind();
	glDrawArrays(GL_TRIANGLES, 0, vertexArray.GetCount());
}*/

/*
void RendererAPI::Submit(const std::string& text, const Font& font, float x, float y, float scale, glm::vec3 color,
					  const std::shared_ptr<Shader>& shader)
{
	shader->Bind();
	shader->SetMat4("u_CameraMatrix", m_ProjectionViewMatrix);
	shader->SetFloat3("u_Color", color);
	shader->SetInt("u_Texture", 0);

	m_TextVAO.Bind();

	for (const char& c: text)
	{
		const std::shared_ptr<Character>& texture = font.GetCharacterTexture(c);

		float width = texture->GetWidth();
		float height = texture->GetHeight();
		glm::vec2 bearing = texture->GetBearing();

		texture->Bind();

		float xPos = x + bearing.x * scale;
		float yPos = y - (height - bearing.y) * scale;

		float data[20] = {
			xPos,                 yPos,                  0.0f, 0.0f, 1.0f,
			xPos,                 yPos + height * scale, 0.0f, 0.0f, 0.0f,
			xPos + width * scale, yPos + height * scale, 0.0f, 1.0f, 0.0f,
			xPos + width * scale, yPos,                  0.0f, 1.0f, 1.0f
		};

		m_TextVAO.GetBuffer(0)->SetData(data);

		x += ((int32_t) texture->GetAdvance().x >> 6) * scale;

		glDrawArrays(GL_QUADS, 0, 4);
	}
}
*/
