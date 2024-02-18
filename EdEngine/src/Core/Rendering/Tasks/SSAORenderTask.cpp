#include "SSAORenderTask.h"
#include <random>
#include "Utils/Files.h"
#include "Utils/MathHelper.h"
#include "Utils/RenderingHelper.h"

void SSAORenderTask::Setup(Renderer* renderer)
{
	RenderTask::Setup(renderer);

	{
		m_SSAOPassSpecification.Name = "SSAO pass";

		m_SSAOPassSpecification.Framebuffer = RenderingHelper::CreateFramebuffer(1, 1, 1, { FramebufferAttachmentType::Distance }, TextureType::Texture2D);

		m_SSAOPassSpecification.Shader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(\shaders\SSAO.glsl)");

		m_SSAOPassSpecification.bUseBlending = false;
		m_SSAOPassSpecification.bClearColors = true;
	}

	{
		m_SSAOBlurPassSpecification.Name = "SSAO blur";

		m_SSAOBlurPassSpecification.Framebuffer = RenderingHelper::CreateFramebuffer(1, 1, 1, { FramebufferAttachmentType::Distance }, TextureType::Texture2D);

		m_SSAOBlurPassSpecification.Shader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(\shaders\SSAOBlur.glsl)");

		m_SSAOBlurPassSpecification.bUseBlending = false;
		m_SSAOBlurPassSpecification.bClearColors = true;
	}

	{
		Texture2DImportParameters parameters;
		parameters.WrapS = WrapMode::Repeat;
		parameters.WrapT = WrapMode::Repeat;
		parameters.Format = PixelFormat::RGB16F;
		parameters.Filtering = FilteringMode::Linear;

		Texture2DData data(1, 1);

		m_Noise = RenderingHelper::CreateTexture2D(std::move(parameters), std::move(data), "SSAO noise");
	}

	SetSamplesCount(m_SamplesCount);
	SetNoiseSize(m_NoiseSize);
}

void SSAORenderTask::Run(const std::vector<std::shared_ptr<Component>>& components, Camera* camera)
{
	if (m_bIsEnabled)
	{
		SSAO(camera);
		Blur();
	}
}

void SSAORenderTask::Resize(glm::ivec2 size, float upscale)
{
	m_SSAOPassSpecification.Framebuffer->Resize(size.x, size.y, 1);
	m_SSAOBlurPassSpecification.Framebuffer->Resize(size.x, size.y, 1);
}

std::shared_ptr<Texture2D> SSAORenderTask::GetRawTexture() const
{
	return std::static_pointer_cast<Texture2D>(m_SSAOPassSpecification.Framebuffer->GetAttachment(0));
}

std::shared_ptr<Texture2D> SSAORenderTask::GetBluredTexture() const
{
	return std::static_pointer_cast<Texture2D>(m_SSAOBlurPassSpecification.Framebuffer->GetAttachment(0));
}

void SSAORenderTask::SetSamplesCount(uint32_t count)
{
	m_SamplesCount = count;
	m_Samples = MathHelper::GenerateHalfSphereSamples(count);
}

uint32_t SSAORenderTask::GetSamplesCount() const
{
	return m_SamplesCount;
}

void SSAORenderTask::SetNoiseSize(uint32_t size)
{
	m_NoiseSize = size;

	std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
	std::default_random_engine generator;

	std::vector<glm::vec3> noise;
	for (int32_t i = 0; i < m_NoiseSize * m_NoiseSize; ++i)
	{
		noise.emplace_back(distribution(generator) * 2.0f - 1.0f, distribution(generator) * 2.0f - 1.0f, 0.0f);
	}

	Texture2DData data(m_NoiseSize, m_NoiseSize, noise.data(), noise.size() * sizeof(glm::vec3), false);
	m_Noise->SetData(std::move(data));
}

uint32_t SSAORenderTask::GetNosiseSize() const
{
	return m_NoiseSize;
}

void SSAORenderTask::SetRadius(float radius)
{
	m_Radius = radius;
}

float SSAORenderTask::GetRadius() const
{
	return m_Radius;
}

void SSAORenderTask::SetBias(float bias)
{
	m_Bias = bias;
}

float SSAORenderTask::GetBias() const
{
	return m_Bias;
}

void SSAORenderTask::SSAO(Camera* camera)
{
	glm::mat4 view = camera->GetView();
	m_Renderer->BeginRenderPass(m_SSAOPassSpecification, view, camera->GetProjection());

	m_Context->SetShaderDataMat4("u_NormalMatrix", glm::transpose(glm::inverse(view)));

	m_Context->SetShaderDataTexture("u_Position", m_Renderer->GetRenderTarget(RenderTarget::GPosition));
	m_Context->SetShaderDataTexture("u_Normal", m_Renderer->GetRenderTarget(RenderTarget::GNormal));

	m_Context->SetShaderDataFloat("u_SampleCount", m_SamplesCount);
	m_Context->SetShaderDataFloat("u_NoiseSize", m_NoiseSize);
	m_Context->SetShaderDataTexture("u_Noise", m_Noise);

	m_Context->SetShaderDataFloat("u_Radius", m_Radius);
	m_Context->SetShaderDataFloat("u_Bias", m_Bias);

	for (int32_t i = 0; i < m_SamplesCount; ++i)
	{
		m_Context->SetShaderDataFloat3("u_Samples[" + std::to_string(i) + "]", m_Samples[i]);
	}

	m_Renderer->SubmitQuad(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f);

	m_Renderer->EndRenderPass();
}

void SSAORenderTask::Blur()
{
	m_Renderer->BeginRenderPass(m_SSAOBlurPassSpecification);

	m_Context->SetShaderDataTexture("u_AmbientOcclusion", m_SSAOPassSpecification.Framebuffer->GetAttachment(0));

	m_Renderer->SubmitQuad(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f);

	m_Renderer->EndRenderPass();
}
