#include "SSAORenderTask.h"
#include <random>
#include "Utils/Files.h"
#include "Utils/MathHelper.h"
#include "Utils/RenderingHelper.h"
#include "Core/Rendering/Framebuffers/Framebuffer.h"

void SSAORenderTask::Setup(Renderer* renderer)
{
	RenderTask::Setup(renderer);

	{
		m_SSAOPassSpecification.Name = "SSAO pass";

		m_SSAOPassSpecification.Framebuffer = RenderingHelper::CreateFramebuffer(1, 1);
		m_SSAOPassSpecification.Framebuffer->CreateAttachment(FramebufferAttachmentType::Distance);

		m_SSAOPassSpecification.Shader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(\shaders\SSAO.glsl)");

		m_SSAOPassSpecification.bUseBlending = false;
		m_SSAOPassSpecification.bClearColors = true;
	}

	{
		m_SSAOBlurPassSpecification.Name = "SSAO blur";

		m_SSAOBlurPassSpecification.Framebuffer = RenderingHelper::CreateFramebuffer(1, 1);
		m_SSAOBlurPassSpecification.Framebuffer->CreateAttachment(FramebufferAttachmentType::Distance);

		m_SSAOBlurPassSpecification.Shader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(\shaders\SSAOBlur.glsl)");

		m_SSAOBlurPassSpecification.bUseBlending = false;
		m_SSAOBlurPassSpecification.bClearColors = false;
	}

	{
		m_SSAOSamples = MathHelper::GenerateHalfSphereSamples(m_SSAOSamplesCount);

		std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
		std::default_random_engine generator;

		float* noise = (float*)std::malloc(3 * m_NoiseSize * m_NoiseSize * sizeof(float));
		for (int32_t i = 0; i < m_NoiseSize * m_NoiseSize; ++i)
		{
			float x = distribution(generator) * 2.0f - 1.0f;
			float y = distribution(generator) * 2.0f - 1.0f;
			float z = 0.0f;

			noise[i * 3] = x;
			noise[i * 3 + 1] = y;
			noise[i * 3 + 2] = z;
		}

		Texture2DImportParameters parameters;
		parameters.WrapS = WrapMode::Repeat;
		parameters.WrapT = WrapMode::Repeat;
		parameters.Format = PixelFormat::RGB16F;
		parameters.Filtering = FilteringMode::Linear;

		Texture2DData data;
		data.Data = reinterpret_cast<uint8_t*>(noise);
		data.Width = m_NoiseSize;
		data.Height = m_NoiseSize;

		m_SSAONoise = RenderingHelper::CreateTexture2D(parameters, data, "SSAO noise");
	}
}

void SSAORenderTask::Run(const std::vector<std::shared_ptr<Component>>& components, Camera* camera)
{
	SSAO(camera);
	Blur();
}

void SSAORenderTask::Resize(glm::ivec2 size, float upscale)
{
	{
		std::shared_ptr<Framebuffer> framebuffer = std::static_pointer_cast<Framebuffer>(m_SSAOPassSpecification.Framebuffer);
		framebuffer->Resize(size.x * upscale / 2.0f, size.y * upscale / 2.0f);
	}

	{
		std::shared_ptr<Framebuffer> framebuffer = std::static_pointer_cast<Framebuffer>(m_SSAOBlurPassSpecification.Framebuffer);
		framebuffer->Resize(size.x * upscale / 2.0f, size.y * upscale / 2.0f);
	}
}

std::shared_ptr<Texture2D> SSAORenderTask::GetRawTexture() const
{
	return std::static_pointer_cast<Texture2D>(m_SSAOPassSpecification.Framebuffer->GetAttachment(0));
}

std::shared_ptr<Texture2D> SSAORenderTask::GetBluredTexture() const
{
	return std::static_pointer_cast<Texture2D>(m_SSAOBlurPassSpecification.Framebuffer->GetAttachment(0));
}

void SSAORenderTask::SSAO(Camera* camera)
{
	glm::mat4 view = camera->GetView();
	m_Renderer->BeginRenderPass(m_SSAOPassSpecification, view, camera->GetProjection());

	std::shared_ptr<Shader> shader = m_SSAOPassSpecification.Shader;

	m_Context->SetShaderDataMat4("u_NormalMatrix", glm::transpose(glm::inverse(view)));

	m_Context->SetShaderDataTexture("u_Position", m_Renderer->GetRenderTarget(RenderTarget::GPosition));
	m_Context->SetShaderDataTexture("u_Normal", m_Renderer->GetRenderTarget(RenderTarget::GNormal));

	m_Context->SetShaderDataFloat("u_SampleCount", m_SSAOSamplesCount);
	m_Context->SetShaderDataFloat("u_NoiseSize", m_NoiseSize);
	m_Context->SetShaderDataTexture("u_Noise", m_SSAONoise);

	for (int32_t i = 0; i < m_SSAOSamplesCount; ++i)
	{
		m_Context->SetShaderDataFloat3("u_Samples[" + std::to_string(i) + "]", m_SSAOSamples[i]);
	}

	m_Renderer->SubmitQuad(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f);

	m_Renderer->EndRenderPass();
}

void SSAORenderTask::Blur()
{
	m_Renderer->BeginRenderPass(m_SSAOBlurPassSpecification);

	std::shared_ptr<Shader> shader = m_SSAOBlurPassSpecification.Shader;
	m_Context->SetShaderDataTexture("u_AmbientOcclusion", m_SSAOPassSpecification.Framebuffer->GetAttachment(0));

	glm::vec2 size = glm::vec2(1.0f / m_SSAOPassSpecification.Framebuffer->GetWidth(), 1.0f / m_SSAOPassSpecification.Framebuffer->GetHeight());
	m_Context->SetShaderDataFloat2("u_PixelSize", size);

	m_Renderer->SubmitQuad(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f);

	m_Renderer->EndRenderPass();
}
