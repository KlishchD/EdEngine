#include "SSAOPass.h"
#include "Utils/MathHelper.h"
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

		std::shared_ptr<Texture2D> texture = RenderingHelper::CreateTexture2D("SSAO noise");
		texture->SetWrapS(WrapMode::Repeat);
		texture->SetWrapT(WrapMode::Repeat);
		texture->SetPixelFormat(PixelFormat::RGB16F);
		texture->SetFilteringMode(FilteringMode::Linear);

		Texture2DData data(m_ShaderParameters.NoiseSize, m_ShaderParameters.NoiseSize, noise.data(), noise.size() * sizeof(glm::vec3), false);
		texture->SetData(std::move(data));

		texture->Initialize();

		m_ShaderParameters.Noise = texture;
	}
}

void SSAOBasePass::Execute()
{
	RenderPass<SSAOBasePassParameters, SSAOBasePassShaderParameters>::Execute();

	if (m_Renderer->IsViewportSizeDirty())
	{
		glm::u32vec2 size = m_Renderer->GetViewportSize();
		m_Parameters.DrawFramebuffer->Resize(size.x, size.y, 1);
	}

	Camera& camera = m_Parameters.Camera->GetCamera();
	m_Renderer->SetCamera(camera);

	m_ShaderParameters.NormalMatrix = glm::transpose(glm::inverse(camera.GetView()));
	m_ShaderParameters.ScreenSize = m_Parameters.Base->GetSize();

	SubmitShaderParameters();

	m_Renderer->SubmitFullScreenQuad();
}

