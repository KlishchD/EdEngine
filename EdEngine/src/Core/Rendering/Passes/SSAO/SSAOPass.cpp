#include "SSAOPass.h"
#include "Helpers/MathHelper.h"
#include <random>

void SSAOBasePass::Initialize(std::shared_ptr<RenderGraph> graph)
{
	RenderPass<SSAOBasePassParameters, SSAOBasePassShaderParameters>::Initialize(graph);

	m_Parameters.Name = "SSAO pass";
	m_Parameters.Shader = RenderingHelper::CreateShader("shaders\\SSAO.glsl");

	m_Parameters.bUseBlending = false;
	m_Parameters.bClearColors = true;

	m_ShaderParameters.Position = m_Parameters.Position;
	m_ShaderParameters.Normal = m_Parameters.Normal;

	{
		std::shared_ptr<Texture2D> texture = RenderingHelper::CreateTexture2D("SSAO noise");
		texture->SetWrapS(WrapMode::Repeat);
		texture->SetWrapT(WrapMode::Repeat);
		texture->SetPixelFormat(PixelFormat::RGB16F);
		texture->SetFilteringMode(FilteringMode::Linear);

		texture->Initialize();

		m_ShaderParameters.Noise = texture;
	}

    SetSamplesCount(16);
    SetNoiseSize(10);
	SetRadius(1.5f);
	SetBias(0.025f);
}

void SSAOBasePass::Execute()
{
	RenderPass<SSAOBasePassParameters, SSAOBasePassShaderParameters>::Execute();

	glm::u32vec2 size = m_Renderer->GetViewportSize();
    m_Parameters.DrawFramebuffer->Resize(std::max<int32_t>(size.x / 2, 1), std::max<int32_t>(size.y / 2, 1), 1);

	Camera& camera = m_Parameters.Camera->GetCamera();
	m_Renderer->SetCamera(camera);

	m_ShaderParameters.NormalMatrix = glm::transpose(glm::inverse(camera.GetView()));
	m_ShaderParameters.ScreenSize = m_Parameters.Base->GetSize();

	SubmitShaderParameters();

	m_Renderer->SubmitFullScreenQuad();
}

void SSAOBasePass::SetSamplesCount(uint32_t count)
{
    m_ShaderParameters.SampleCount = count;
	
	std::vector<glm::vec3> samples = MathHelper::GenerateHalfSphereSamples(count);
	for (int32_t i = 0; i < count; ++i)
	{
		m_ShaderParameters.Samples[i] = samples[i];
	}
}

uint32_t SSAOBasePass::GetSamplesCount() const
{
	return m_ShaderParameters.SampleCount;
}

void SSAOBasePass::SetNoiseSize(uint32_t size)
{
    m_ShaderParameters.NoiseSize = size;

    std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
    std::default_random_engine generator;

    std::vector<glm::vec3> noise;
    for (int32_t i = 0; i < size * size; ++i)
    {
        noise.emplace_back(distribution(generator) * 2.0f - 1.0f, distribution(generator) * 2.0f - 1.0f, 0.0f);
    }

    Texture2DData data(size, size, (uint8_t*) &noise[0], noise.size() * sizeof(glm::vec3), false);
    m_ShaderParameters.Noise->SetData(std::move(data));
}

uint32_t SSAOBasePass::GetNosiseSize() const
{
	return m_ShaderParameters.NoiseSize;
}

void SSAOBasePass::SetRadius(float radius)
{
	m_ShaderParameters.Radius = radius;
}

float SSAOBasePass::GetRadius() const
{
	return m_ShaderParameters.Radius;
}

void SSAOBasePass::SetBias(float bias)
{
	m_ShaderParameters.Bias = bias;
}

float SSAOBasePass::GetBias() const
{
	return m_ShaderParameters.Bias;
}

