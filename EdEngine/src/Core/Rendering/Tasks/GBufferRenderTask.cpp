#include "GBufferRenderTask.h"
#include "Utils/RenderingHelper.h"
#include "Utils/MathHelper.h"
#include "Core/Components/StaticMeshComponent.h"
#include "Utils/Files.h"

void GBufferRenderTask::Setup(Renderer* renderer)
{
	RenderTask::Setup(renderer);

	m_GBufferPassSpecification.Name = "Geometry pass";
	std::vector<FramebufferAttachmentType> attachments = { FramebufferAttachmentType::Color, FramebufferAttachmentType::Position, FramebufferAttachmentType::Direction,
														   FramebufferAttachmentType::Color16, FramebufferAttachmentType::Velocity, FramebufferAttachmentType::Depth };
	std::shared_ptr<Framebuffer> framebuffer = RenderingHelper::CreateFramebuffer(1, 1, 1, attachments, TextureType::Texture2D);

	m_GBufferPassSpecification.Framebuffer = framebuffer;

	m_GBufferPassSpecification.Shader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(\shaders\deferred\geometry-pass.glsl)");

	m_GBufferPassSpecification.bUseBlending = false;

	m_GBufferPassSpecification.bClearColors = true;
	m_GBufferPassSpecification.bClearDepth = true;

	for (int32_t i = 0; i < m_JitterSequenceSize; ++i)
	{
		m_JitterSequence.push_back(glm::vec2(2.0f * MathHelper::Halton(i + 1, 2) - 1.0f, 2.0f * MathHelper::Halton(i + 1, 3) - 1.0f));
	}
}

void GBufferRenderTask::Run(const std::vector<std::shared_ptr<Component>>& components, Camera* camera)
{
	glm::mat4 view = camera->GetView();
	glm::mat4 projection = camera->GetProjection();

	glm::vec2 size = glm::vec2(m_GBufferPassSpecification.Framebuffer->GetWidth(), m_GBufferPassSpecification.Framebuffer->GetHeight());

	glm::vec2 jitter = m_JitterSequence[m_CurrentJitterIndex] / size;
	glm::vec2 previousJitter = m_JitterSequence[(m_CurrentJitterIndex - 1 + m_JitterSequenceSize) % m_JitterSequenceSize] / size;

	bool bIsTAAEnabled = m_Renderer->GetAAMethod() == AAMethod::TAA;

	if (bIsTAAEnabled)
	{
		projection = glm::translate(glm::mat4(1.0f), glm::vec3(jitter - previousJitter, 0.0f)) * projection;
		camera->SetProjection(projection);
	}

	m_GBufferPassSpecification.ViewPosition = camera->GetPosition();

	m_Renderer->BeginRenderPass(m_GBufferPassSpecification, view, projection);

	if (bIsTAAEnabled)
	{
		m_Context->SetShaderDataFloat2("u_PreviousJitter", previousJitter);
		m_Context->SetShaderDataFloat2("u_Jitter", jitter);
	}
	else
	{
		m_Context->SetShaderDataMat4("u_PreviousProjectionMatrix", projection);
	}

	m_Context->SetShaderDataMat4("u_PreviousViewMatrix", m_PreviousView);

	for (std::shared_ptr<Component> component : components)
	{
		if (std::shared_ptr<StaticMeshComponent> meshComponent = std::dynamic_pointer_cast<StaticMeshComponent>(component))
		{
			if (std::shared_ptr<StaticMesh> mesh = meshComponent->GetStaticMesh())
			{
				m_Renderer->SubmitMesh(mesh, meshComponent->GetWorldTransform(), meshComponent->GetPreviousWorldTransform());
			}
		}
	}

	m_Renderer->EndRenderPass();

	m_CurrentJitterIndex = (m_CurrentJitterIndex + 1) % m_JitterSequenceSize;
	m_PreviousView = view;
}

void GBufferRenderTask::Resize(glm::ivec2 size, float upscale)
{
	m_GBufferPassSpecification.Framebuffer->Resize(size.x * upscale, size.y * upscale, 0);
}

std::shared_ptr<Texture2D> GBufferRenderTask::GetAlbedoTexture() const
{
	return std::static_pointer_cast<Texture2D>(m_GBufferPassSpecification.Framebuffer->GetAttachment(0));
}

std::shared_ptr<Texture2D> GBufferRenderTask::GetPositionTexture() const
{
	return std::static_pointer_cast<Texture2D>(m_GBufferPassSpecification.Framebuffer->GetAttachment(1));
}

std::shared_ptr<Texture2D> GBufferRenderTask::GetNormalTexture() const
{
	return std::static_pointer_cast<Texture2D>(m_GBufferPassSpecification.Framebuffer->GetAttachment(2));
}

std::shared_ptr<Texture2D> GBufferRenderTask::GetRoughnessMetalicEmissionTexture() const
{
	return std::static_pointer_cast<Texture2D>(m_GBufferPassSpecification.Framebuffer->GetAttachment(3));
}

std::shared_ptr<Texture2D> GBufferRenderTask::GetVelocityTexture() const
{
	return std::static_pointer_cast<Texture2D>(m_GBufferPassSpecification.Framebuffer->GetAttachment(4));
}

std::shared_ptr<Texture2D> GBufferRenderTask::GetDepthTexture() const
{
	return std::static_pointer_cast<Texture2D>(m_GBufferPassSpecification.Framebuffer->GetDepthAttachment());
}

std::shared_ptr<Framebuffer> GBufferRenderTask::GetFramebuffer() const
{
	return std::static_pointer_cast<Framebuffer>(m_GBufferPassSpecification.Framebuffer);
}
