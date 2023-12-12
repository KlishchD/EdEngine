#include "Renderer.h"
#include "Core/Engine.h"
#include "Core/Scene.h"
#include "Utils/GeometryBuilder.h"
#include "Utils/RenderingHelper.h"
#include "Utils/Files.h"
#include "Core/Macros.h"
#include "imgui.h"
#include <random>

#include "Core/Components/StaticMeshComponent.h"
#include "Core/Components/PointLightComponent.h"

#include "Core/Rendering/Buffers/VertexBuffer.h"

#include "RenderingContex.h"
#include "RenderPassSpecification.h"
#include "Framebuffers/Framebuffer.h"
#include "Framebuffers/CubeFramebuffer.h"
#include "Buffers/VertexBufferLayout.h"
#include "Shader.h"

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

	auto [vertices, indices] = GeometryBuilder::MakeSphere(1, 50, 50);

	VertexBufferLayout pointLightVBOLayout = { { "position", ShaderDataType::Float3 } };

	m_PointLightMeshVBO = RenderingHelper::CreateVertexBuffer(vertices.data(), sizeof(float) * vertices.size(), pointLightVBOLayout, BufferUsage::StaticDraw);
	m_PointLightMeshIBO = RenderingHelper::CreateIndexBuffer(indices.data(), sizeof(int32_t) * indices.size(), BufferUsage::StaticDraw);



    m_BlurFramebuffer1 = RenderingHelper::CreateFramebuffer(1, 1);
	m_BlurFramebuffer1->CreateAttachment(FramebufferAttachmentType::Color16);

	m_BlurFramebuffer2 = RenderingHelper::CreateFramebuffer(1, 1);
	m_BlurFramebuffer2->CreateAttachment(FramebufferAttachmentType::Color16);

    m_BlurShader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(\shaders\blur.glsl)");
    
    SetupGeometryRenderPass();
    SetupLightRenderPass();
    SetupSSAORenderPass();
    SetupCombinationRenderPass();

    SetupShadowRenderPass();
    SetupBrightnessFilterPass();

    CreateRandomShadowMapSamples();

	ED_LOG(Renderer, info, "Finished initalizing Renderer")
}

void Renderer::Deinitialize()
{
    
}

void Renderer::Update()
{
	m_Context->SwapBuffers();

	if (m_GeometryPassSpecification.Framebuffer->GetWidth() != m_ViewportSize.x || m_GeometryPassSpecification.Framebuffer->GetHeight() != m_ViewportSize.y)
	{
		std::static_pointer_cast<Framebuffer>(m_GeometryPassSpecification.Framebuffer)->Resize(m_ViewportSize.x, m_ViewportSize.y);
		std::static_pointer_cast<Framebuffer>(m_LightPassSpecification.Framebuffer)->Resize(m_ViewportSize.x, m_ViewportSize.y);
		std::static_pointer_cast<Framebuffer>(m_CombinationPassSpecification.Framebuffer)->Resize(m_ViewportSize.x, m_ViewportSize.y);

		std::static_pointer_cast<CubeFramebuffer>(m_PointLightShadowPassSpecification.Framebuffer)->Resize(m_ViewportSize.y);

		std::static_pointer_cast<Framebuffer>(m_BlurFramebuffer1)->Resize(m_ViewportSize.x, m_ViewportSize.y);
		std::static_pointer_cast<Framebuffer>(m_BlurFramebuffer2)->Resize(m_ViewportSize.x, m_ViewportSize.y);

		std::static_pointer_cast<Framebuffer>(m_SSAOPassSpecification.Framebuffer)->Resize(m_ViewportSize.x, m_ViewportSize.y);
		std::static_pointer_cast<Framebuffer>(m_SSAOBlurPassSpecification.Framebuffer)->Resize(m_ViewportSize.x, m_ViewportSize.y);

		m_Engine->GetCamera()->SetProjection(90.0f, 1.0f * m_ViewportSize.x / m_ViewportSize.y, 1.0f, m_FarPlane);
	}

	std::shared_ptr<Scene> scene = m_Engine->GetLoadedScene();
	Camera* camera = m_Engine->GetCamera();

	std::vector<std::shared_ptr<Component>> components = scene->GetAllComponents();

	GeometryPass(components, camera);
	LightPass(components, camera);
	SSAOPass(camera);
	BloomPass();
	CombinationPass(components, camera);

	while (m_Commands.size()) {
		m_Commands.front()(m_Context.get());
		m_Commands.pop();
	}
}

void Renderer::ResizeViewport(glm::vec2 size)
{
    m_ViewportSize = 2.0f * size;
}

void Renderer::SubmitRenderCommand(const std::function<void(RenderingContext* context)>& command)
{
    m_Commands.push(command);
}

void Renderer::SetSSAOEnabled(bool enabled)
{
    bSSAOEnabled = enabled;
}

bool Renderer::IsSSAOEnabled() const
{
    return bSSAOEnabled;
}

std::shared_ptr<Framebuffer> Renderer::GetGeometryPassFramebuffer() const
{
    return std::static_pointer_cast<Framebuffer>(m_GeometryPassSpecification.Framebuffer);
}

std::shared_ptr<Framebuffer> Renderer::LightPassFramebuffer() const
{
    return std::static_pointer_cast<Framebuffer>(m_LightPassSpecification.Framebuffer);
}

std::shared_ptr<Framebuffer> Renderer::GetViewport() const
{
    return std::static_pointer_cast<Framebuffer>(m_CombinationPassSpecification.Framebuffer);
}

void Renderer::GeometryPass(const std::vector<std::shared_ptr<Component>>& components, Camera* camera)
{
    m_GeometryPassSpecification.ViewPosition = camera->GetPosition();
    BeginRenderPass(m_GeometryPassSpecification, camera->GetView(), camera->GetProjection());

    for (const std::shared_ptr<Component>& component : components)
    {
        if (std::shared_ptr<StaticMeshComponent> meshComponent = std::dynamic_pointer_cast<StaticMeshComponent>(component))
        {
            if (std::shared_ptr<StaticMesh> mesh = meshComponent->GetStaticMesh()) 
            {
                SubmitMesh(mesh, meshComponent->GetTransform()); // TODO: Need a hierarchical Transform
            }
        }
    }

    EndRenderPass();
}

void Renderer::SSAOPass(Camera* camera) {
    if (bSSAOEnabled)
    {
        {
            glm::mat4 view = camera->GetView();
            BeginRenderPass(m_SSAOPassSpecification, view, camera->GetProjection());

            std::shared_ptr<Shader> shader = m_SSAOPassSpecification.Shader;

            m_Context->SetShaderDataMat4("u_NormalMatrix", glm::transpose(glm::inverse(view)));
            
            m_Context->SetShaderDataTexture("u_Position", m_GeometryPassSpecification.Framebuffer->GetAttachment(1));
            m_Context->SetShaderDataTexture("u_Normal", m_GeometryPassSpecification.Framebuffer->GetAttachment(2));
            
            m_Context->SetShaderDataFloat("u_SampleCount", m_SSAOSamplesCount);
            m_Context->SetShaderDataFloat("u_NoiseSize", m_NoiseSize);
            m_Context->SetShaderDataTexture("u_Noise", m_SSAONoise);

            for (int32_t i = 0; i < m_SSAOSamplesCount; ++i)
            {
                m_Context->SetShaderDataFloat3("u_Samples[" + std::to_string(i) + "]", m_SSAOSamples[i]);
            }

            SubmitQuad(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f);

            EndRenderPass();
        }
        {
            BeginRenderPass(m_SSAOBlurPassSpecification, camera->GetView(), camera->GetProjection());

            std::shared_ptr<Shader> shader = m_SSAOBlurPassSpecification.Shader;
            m_Context->SetShaderDataTexture("u_AmbientOcclusion", m_SSAOPassSpecification.Framebuffer->GetAttachment(0));
            m_Context->SetShaderDataFloat2("u_PixelSize", 1.0f / glm::vec2(m_ViewportSize));

			SubmitQuad(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f);

            EndRenderPass();
        }
    }
}

void Renderer::BloomPass() 
{
    BeginRenderPass(m_BrighnessFilterPassSpecification, glm::mat4(1.0f), glm::mat4(1.0f)); // Temporary step, may be removed after PBR bloom will be added :)

    m_Context->SetShaderDataTexture("u_Image", m_LightPassSpecification.Framebuffer->GetAttachment(0));

    SubmitQuad(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f);

    EndRenderPass();

    std::shared_ptr<Framebuffer> read;
    std::shared_ptr<Framebuffer> write;

    for (int32_t i = 0; i < m_BlurPassCount; ++i) {
		if (i % 2)
		{
			read = m_BlurFramebuffer2;
			write = m_BlurFramebuffer1;
		}
		else
		{
			read = m_BlurFramebuffer1;
			write = m_BlurFramebuffer2;
		}

		BeginRenderPass("Blur pass", write, m_BlurShader, glm::mat4(1.0f), glm::mat4(1.0f), glm::vec3(0.0f));

		m_Context->ClearColorTarget();

		m_Context->SetShaderDataBool("u_HorizontalPass", i % 2 == 0);
		m_Context->SetShaderDataFloat("u_PixelWidth", 1.0f / read->GetWidth());
		m_Context->SetShaderDataFloat("u_PixelHeight", 1.0f / read->GetHeight());
		m_Context->SetShaderDataFloat2("u_ScreenSize", write->GetWidth(), write->GetHeight());

        m_Context->SetShaderDataTexture("u_Image", read->GetAttachment(0));

		SubmitQuad(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f);

		EndRenderPass();
    }
}

void Renderer::LightPass(const std::vector<std::shared_ptr<Component>>& components, Camera* camera)
{
    m_LightPassSpecification.ViewPosition = camera->GetPosition();

    BeginRenderPass(m_LightPassSpecification, camera->GetView(), camera->GetProjection());
    
    m_Context->ClearColorTarget();
    m_Context->ClearDepthTarget();
    
    EndRenderPass();


    for (const std::shared_ptr<Component>& outerComponent : components)
    {
        if (std::shared_ptr<PointLightComponent> light = std::dynamic_pointer_cast<PointLightComponent>(outerComponent))
        {
            if (light->GetIntensity() == 0) 
            {
                continue;
            }

            m_PointLightShadowPassSpecification.ViewPosition = light->GetPosition();

			BeginRenderPass(m_PointLightShadowPassSpecification, glm::mat4(1.0f), glm::mat4(1.0f));
            
            m_Context->SetShaderDataFloat("u_FarPlane", m_FarPlane);

            std::shared_ptr<CubeFramebuffer> frambuffer = std::static_pointer_cast<CubeFramebuffer>(m_PointLightShadowPassSpecification.Framebuffer);

            if (light->IsShadowCasting())
            {
                for (int32_t i = 0; i < 6; ++i) {
                    m_Context->SetShaderDataMat4("u_ViewProjection[" + std::to_string(i) + "]", m_LightPerspective * light->GetShadowMapPassCameraTransformation(i));
				}
                
                frambuffer->AttachLayers();

				for (const std::shared_ptr<Component>& component : components)
				{
					if (std::shared_ptr<StaticMeshComponent> meshComponent = std::dynamic_pointer_cast<StaticMeshComponent>(component))
					{
						if (std::shared_ptr<StaticMesh> mesh = meshComponent->GetStaticMesh()) {
							SubmitMeshRaw(mesh, meshComponent->GetTransform()); // TODO: Need a hierarchical Transform
						}
					}
				}
            }
			else
			{
				for (int32_t i = 0; i < 6; ++i) {
                    frambuffer->AttachFace(i);
					m_Context->ClearDepthTarget();
				}
			}

            EndRenderPass();

            m_LightPassSpecification.ViewPosition = camera->GetPosition();

            BeginRenderPass(m_LightPassSpecification, camera->GetView(), camera->GetProjection());

            std::shared_ptr<Shader> shader = m_LightPassSpecification.Shader;

			m_Context->SetShaderDataTexture("u_Albedo", m_GeometryPassSpecification.Framebuffer->GetAttachment(0));
			m_Context->SetShaderDataTexture("u_Position", m_GeometryPassSpecification.Framebuffer->GetAttachment(1));
			m_Context->SetShaderDataTexture("u_Normal", m_GeometryPassSpecification.Framebuffer->GetAttachment(2));
            m_Context->SetShaderDataTexture("u_RoughnessMetalic", m_GeometryPassSpecification.Framebuffer->GetAttachment(3));
            
            m_Context->SetShaderDataFloat("u_FarPlane", m_FarPlane);
            
            m_Context->SetShaderDataTexture("u_RandomSamples", m_ShadowMapRandomSamples);
            m_Context->SetShaderDataFloat("u_FilterSize", m_FilterSize);
            m_Context->SetShaderDataFloat("u_ShadowMapPixelSize", 1.0f / m_PointLightShadowPassSpecification.Framebuffer->GetWidth());

            m_Context->EnableFaceCulling();
            SubmitLightMesh(light, m_PointLightShadowPassSpecification.Framebuffer->GetDepthAttachment());
            m_Context->DisableFaceCulling();

            EndRenderPass();
        }
    }
}

void Renderer::CreateRandomShadowMapSamples()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dist(-0.5, 0.5);

	float* samples = static_cast<float*>(std::malloc(m_ShadowMapFiltersCount * m_FilterSize * m_FilterSize * m_FilterSize * sizeof(float)));
	int32_t sample = 0;
	for (int32_t i = 0; i < m_ShadowMapFiltersCount; ++i) {
		for (int32_t u = m_FilterSize - 1; u >= 0; --u) {
			for (int32_t v = 0; v < m_FilterSize; ++v) {
				for (int32_t p = 0; p < m_FilterSize; ++p) {
					//                     float u1 = (u + 0.5f + dist(gen)) / m_FilterSize;
					//                     float v1 = (v + 0.5f + dist(gen)) / m_FilterSize;
					//                     float p1 = (p + 0.5f + dist(gen)) / m_FilterSize;
					// 
					//                     float x = sqrtf(v1) * cos(2.0f * glm::pi<float>() * u1);
					//                     float y = sqrtf(v1) * sin(2.0f * glm::pi<float>() * u1);
					//                     float z = sqrtf(v1) * sin(2.0f * glm::pi<float>() * p1);

					samples[sample * 3] = (u + 0.5f + dist(gen));
					samples[sample * 3 + 1] = (v + 0.5f + dist(gen));
					samples[sample * 3 + 2] = (p + 0.5f + dist(gen));
				}
			}
		}
	}

	Texture2DImportParameters parameters;
	parameters.WrapS = WrapMode::Repeat;
	parameters.WrapT = WrapMode::Repeat;
	parameters.Format = PixelFormat::RGB8F;
	parameters.Filtering = FilteringMode::Linear;

	Texture2DData data;
	data.Width = m_FilterSize * m_FilterSize;
	data.Height = m_ShadowMapFiltersCount;
	data.Data = (uint8_t*)samples;

    m_ShadowMapRandomSamples = RenderingHelper::CreateTexture2D(parameters, data, "Shadow map random samples");
}

void Renderer::SetupSSAORenderPass()
{
    {
        m_SSAOPassSpecification.Name = "SSAO pass";

        m_SSAOPassSpecification.Framebuffer = RenderingHelper::CreateFramebuffer(1, 1);
        m_SSAOPassSpecification.Framebuffer->CreateAttachment(FramebufferAttachmentType::Distance);

        m_SSAOPassSpecification.Shader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(\shaders\SSAO.glsl)");

        m_SSAOPassSpecification.bUseBlending = false;
        m_SSAOPassSpecification.bClearColors = true;
    }

    std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
    std::default_random_engine generator;
    for (int32_t i = 0; i < m_SSAOSamplesCount; ++i)
    {
        float x = distribution(generator) * 2.0f - 1.0f;
        float y = distribution(generator) * 2.0f - 1.0f;
        float z = distribution(generator);

        glm::vec3 sample(x, y, z);
        sample = glm::normalize(sample) * distribution(generator);

        float scale = 1.0f * i / m_SSAOSamplesCount;
        Renderer::lerp(0.1f, 1.0f, scale * scale);
        sample *= scale;

        m_SSAOSamples.push_back(sample);
    }

	float* noise = (float*)std::malloc(3 * m_NoiseSize * m_NoiseSize * sizeof(float));
    for (int32_t i = 0; i < m_NoiseSize * m_NoiseSize; ++i) 
    {
        float x = distribution(generator) * 2.0f - 1.0f;
        float y = distribution(generator) * 2.0f - 1.0f;
        float z = 0.0f; // TODO: Why?
        
		noise[i * 3] = x;
		noise[i * 3 + 1] = y;
		noise[i * 3 + 2] = z;
    }

    Texture2DImportParameters parameters;
    parameters.WrapS = WrapMode::Repeat;
    parameters.WrapT = WrapMode::Repeat;
    parameters.Format = PixelFormat::RGB32F;
    parameters.Filtering = FilteringMode::Linear;

    Texture2DData data;
    data.Data = reinterpret_cast<uint8_t*>(noise);
    data.Width = m_NoiseSize;
    data.Height = m_NoiseSize;

    m_SSAONoise = RenderingHelper::CreateTexture2D(parameters, data, "SSAO noise");

    {
        m_SSAOBlurPassSpecification.Name = "SSAO blur";

        m_SSAOBlurPassSpecification.Framebuffer = RenderingHelper::CreateFramebuffer(1, 1);
		m_SSAOBlurPassSpecification.Framebuffer->CreateAttachment(FramebufferAttachmentType::Distance);

        m_SSAOBlurPassSpecification.Shader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(\shaders\SSAOBlur.glsl)");

        m_SSAOBlurPassSpecification.bUseBlending = false;
        m_SSAOBlurPassSpecification.bClearColors = false;
    }
}

void Renderer::SetupLightRenderPass()
{
    m_LightPassSpecification.Name = "Light pass";

    std::shared_ptr<Framebuffer> framebuffer = RenderingHelper::CreateFramebuffer(1, 1);
    framebuffer->CreateAttachment(FramebufferAttachmentType::Color16);

    m_LightPassSpecification.Framebuffer = framebuffer;

    m_LightPassSpecification.Shader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(\shaders\deferred\light-pass-mesh.glsl)");

    m_LightPassSpecification.SourceFactor = BlendFactor::One;
    m_LightPassSpecification.DestinationFactor = BlendFactor::One;
}

void Renderer::SetupBrightnessFilterPass()
{
    m_BrighnessFilterPassSpecification.Name = "Brighness filter";

    m_BrighnessFilterPassSpecification.Framebuffer = m_BlurFramebuffer1;

    m_BrighnessFilterPassSpecification.Shader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(\shaders\brightness-filter.glsl)");

    m_BrighnessFilterPassSpecification.bClearColors = true;
}

void Renderer::SetupShadowRenderPass()
{
    m_PointLightShadowPassSpecification.Name = "Shadow pass";

	std::shared_ptr<CubeFramebuffer> framebuffer = RenderingHelper::CreateCubeFramebuffer(1);
    framebuffer->CreateAttachment(FramebufferAttachmentType::Depth);

    m_PointLightShadowPassSpecification.Framebuffer = framebuffer;

    m_PointLightShadowPassSpecification.Shader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(\shaders\shadow-pass.glsl)");

    m_PointLightShadowPassSpecification.bUseBlending = false;

    m_PointLightShadowPassSpecification.bClearDepth = true;
}

void Renderer::SetupCombinationRenderPass()
{
    m_CombinationPassSpecification.Name = "Combination pass";

    std::shared_ptr<Framebuffer> framebuffer = RenderingHelper::CreateFramebuffer(1, 1);
    framebuffer->CreateAttachment(FramebufferAttachmentType::Color);
    framebuffer->CreateAttachment(FramebufferAttachmentType::Depth);

    m_CombinationPassSpecification.Framebuffer = framebuffer;

    m_CombinationPassSpecification.Shader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(\shaders\deferred\combination-pass.glsl)");

    m_CombinationPassSpecification.bUseBlending = false;

    m_CombinationPassSpecification.bClearColors = true;
    m_CombinationPassSpecification.bClearDepth = true;
}

void Renderer::SetupGeometryRenderPass()
{
	m_GeometryPassSpecification.Name = "Geometry pass";

    std::shared_ptr<Framebuffer> framebuffer = RenderingHelper::CreateFramebuffer(1, 1);
    framebuffer->CreateAttachment(FramebufferAttachmentType::Color);
    framebuffer->CreateAttachment(FramebufferAttachmentType::Position);
    framebuffer->CreateAttachment(FramebufferAttachmentType::Direction);
    framebuffer->CreateAttachment(FramebufferAttachmentType::Color);
    framebuffer->CreateAttachment(FramebufferAttachmentType::Depth);

	m_GeometryPassSpecification.Framebuffer = framebuffer;

    m_GeometryPassSpecification.Shader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(\shaders\deferred\geometry-pass.glsl)");

    m_GeometryPassSpecification.bUseBlending = false;

    m_GeometryPassSpecification.bClearColors = true;
    m_GeometryPassSpecification.bClearDepth = true;
}

float Renderer::lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

void Renderer::CombinationPass(const std::vector<std::shared_ptr<Component>>& components, Camera* camera)
{
    m_CombinationPassSpecification.ViewPosition = camera->GetPosition();

    BeginRenderPass(m_CombinationPassSpecification, camera->GetView(), camera->GetProjection());

    std::shared_ptr<Shader> shader = m_CombinationPassSpecification.Shader;

    m_Context->SetShaderDataTexture("u_Albedo", m_GeometryPassSpecification.Framebuffer->GetAttachment(0));
    m_Context->SetShaderDataTexture("u_RoughnessMetalic", m_GeometryPassSpecification.Framebuffer->GetAttachment(3));
    m_Context->SetShaderDataTexture("u_Light", m_LightPassSpecification.Framebuffer->GetAttachment(0));
    m_Context->SetShaderDataTexture("u_Bloom", (m_BlurPassCount % 2 ? m_BlurFramebuffer2 : m_BlurFramebuffer1)->GetAttachment(0));
    m_Context->SetShaderDataTexture("u_AmbientOcclusion", bSSAOEnabled ? m_SSAOBlurPassSpecification.Framebuffer->GetAttachment(0) : RenderingHelper::GetWhiteTexture());

    SubmitQuad(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f);

    EndRenderPass();
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

void Renderer::SubmitLightMesh(const std::shared_ptr<PointLightComponent>& light, const std::shared_ptr<Texture>& shadowMap)
{
	glm::vec3 color = light->GetColor();
	float Imax = std::max(color.x, std::max(color.y, color.z));
	float radius = std::sqrt(light->GetIntensity() * Imax) * 16.0f;

	if (glm::length(m_Specification->ViewPosition - light->GetPosition()) * 0.9f > radius)
	{
        m_Context->SetCullingFace(Face::Back);
	}
	else
	{
		m_Context->SetCullingFace(Face::Front);
	}

	glm::mat4 transform = glm::scale(light->GetTransform().GetMatrixWithOutScale(), glm::vec3(radius));
	m_Context->SetShaderDataMat4("u_ModelMatrix", transform);

	m_Context->SetShaderDataFloat3("u_PointLight.Position", light->GetPosition());
	m_Context->SetShaderDataFloat3("u_PointLight.Color", light->GetColor());
	m_Context->SetShaderDataFloat("u_PointLight.Intensity", light->GetIntensity());
	m_Context->SetShaderDataFloat("u_PointLight.Radius", radius);

	if (shadowMap)
	{
		m_Context->SetShaderDataTexture("u_PointLight.ShadowMap", shadowMap);
		m_Context->SetShaderDataBool("u_PointLight.UseShadowMap", true);
	}
	else
	{
		m_Context->SetShaderDataBool("u_PointLight.UseShadowMap", false);
	}

	m_Context->SetVertexBuffer(m_PointLightMeshVBO);
	m_Context->SetIndexBuffer(m_PointLightMeshIBO);
	m_Context->Draw();
}

void Renderer::SubmitLight(const std::shared_ptr<PointLightComponent>& light)
{
	std::string front = "u_PointLights[" + std::to_string(m_LightCount) + "]";
	m_Context->SetShaderDataFloat3(front + ".Position", light->GetPosition());
    m_Context->SetShaderDataFloat3(front + ".Color", light->GetColor());
    m_Context->SetShaderDataFloat(front + ".Intensity", light->GetIntensity());

	m_LightCount++;
    m_Context->SetShaderDataInt("u_PointLightsCount", m_LightCount);
}

void Renderer::SubmitMesh(const std::shared_ptr<StaticMesh>& mesh, const Transform& transform)
{
	for (const std::shared_ptr<StaticSubmesh>& submesh : mesh->GetSubmeshes())
	{
		SubmitSubmesh(submesh, transform);
	}
}

void Renderer::SubmitSubmesh(const std::shared_ptr<StaticSubmesh>& submesh, const Transform& transform)
{
	if (std::shared_ptr<Material> material = submesh->GetMaterial()) {
        m_Context->SetVertexBuffer(submesh->GetVertexBuffer());
        m_Context->SetIndexBuffer(submesh->GetIndexBuffer());
		
        material->SetShaderData(m_Context);

		m_Context->SetShaderDataMat4("u_ModelMatrix", transform.GetMatrix());
		m_Context->SetShaderDataMat3("u_NormalMatrix", transform.GetInversedTransposedMatrix());

        m_Context->Draw();
	}
}

void Renderer::SubmitMeshRaw(const std::shared_ptr<StaticMesh>& mesh, const Transform& transform)
{
	for (const std::shared_ptr<StaticSubmesh>& submesh : mesh->GetSubmeshes())
	{
		SubmitSubmeshRaw(submesh, transform);
	}
}

void Renderer::SubmitSubmeshRaw(const std::shared_ptr<StaticSubmesh>& submesh, const Transform& transform)
{
	if (std::shared_ptr<Material> material = submesh->GetMaterial()) {
		m_Context->SetVertexBuffer(submesh->GetVertexBuffer());
		m_Context->SetIndexBuffer(submesh->GetIndexBuffer());

        m_Context->SetShaderDataMat4("u_ModelMatrix", transform.GetMatrix());

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
	m_LightCount = 0;
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
