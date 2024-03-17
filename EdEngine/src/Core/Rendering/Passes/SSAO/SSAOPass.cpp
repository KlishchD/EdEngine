#include "SSAOPass.h"
#include "Utils/MathHelper.h"
#include <random>

void SSAOPass::Initialize(std::shared_ptr<RenderGraph> graph)
{
	RenderPass<SSAORenderPassParameters, SSAORenderPassShaderParameters>::Initialize(graph);

	m_Parameters.Name = "SSAO pass";
	m_Parameters.Shader = RenderingHelper::CreateShader("shaders\\SSAO.glsl");

	m_Parameters.bUseBlending = false;
	m_Parameters.bClearColors = true;

	m_ShaderParameters.Position = m_Parameters.Position;
	m_ShaderParameters.Normal = m_Parameters.Normal;
	
	m_ShaderParameters.SampleCount = 16;
	m_ShaderParameters.NoiseSize = 10;

	{
		std::vector<glm::vec3> samples = MathHelper::GenerateHalfSphereSamples(m_ShaderParameters.SampleCount);

		for (int32_t i = 0; i < m_ShaderParameters.SampleCount; ++i)
		{
			m_ShaderParameters.Samples[i] = samples[i];
		}
	}

	{
		std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
		std::default_random_engine generator;

		std::vector<glm::vec3> noise;
		for (int32_t i = 0; i < m_ShaderParameters.NoiseSize * m_ShaderParameters.NoiseSize; ++i)
		{
			noise.emplace_back(distribution(generator) * 2.0f - 1.0f, distribution(generator) * 2.0f - 1.0f, 0.0f);
		}

		Texture2DImportParameters parameters;
		parameters.WrapS = WrapMode::Repeat;
		parameters.WrapT = WrapMode::Repeat;
		parameters.Format = PixelFormat::RGB16F;
		parameters.Filtering = FilteringMode::Linear;

		Texture2DData data(m_ShaderParameters.NoiseSize, m_ShaderParameters.NoiseSize, noise.data(), noise.size() * sizeof(glm::vec3), false);

		m_ShaderParameters.Noise = RenderingHelper::CreateTexture2D(std::move(parameters), std::move(data), "SSAO noise");
	}
}

void SSAOPass::Update(float deltaSeconds)
{
	RenderPass<SSAORenderPassParameters, SSAORenderPassShaderParameters>::Update(deltaSeconds);

	if (m_Renderer->IsViewportSizeDirty())
	{
		glm::u32vec2 size = m_Renderer->GetViewportSize();
		m_Parameters.DrawFramebuffer->Resize(size.x, size.y, 1);
	}

	Camera& camera = m_Parameters.Camera->GetCamera();
	m_Renderer->SetCamera(camera);

	m_ShaderParameters.NormalMatrix = glm::transpose(glm::inverse(camera.GetView()));

	SubmitShaderParameters();

	m_Renderer->SubmitFullScreenQuad();
}

