#include "Renderer.h"
#include "Core/Engine.h"
#include "Utils/Files.h"
#include <random>

void Renderer::Initialize(Engine* engine)
{
    m_Engine = engine;
    m_RendererAPI = &RendererAPI::GetRendererAPI();

    m_GeometryFramebuffer = std::make_shared<Framebuffer>(1.0f, 1.0f);
    m_GeometryFramebuffer->Bind();
    m_GeometryFramebuffer->CreateAttachment(FramebufferAttachmentType::Color);
    m_GeometryFramebuffer->CreateAttachment(FramebufferAttachmentType::Position);
    m_GeometryFramebuffer->CreateAttachment(FramebufferAttachmentType::Direction);
    m_GeometryFramebuffer->CreateAttachment(FramebufferAttachmentType::Color);
    m_GeometryFramebuffer->CreateAttachment(FramebufferAttachmentType::Depth);
    m_GeometryFramebuffer->Unbind();

    m_ShadowMapsFramebuffer = std::make_shared<CubeFramebuffer>(1.0f);
    m_ShadowMapsFramebuffer->Bind();
    m_ShadowMapsFramebuffer->CreateAttachment(FramebufferAttachmentType::Depth);
    m_ShadowMapsFramebuffer->Unbind();
    
    m_LightPassFramebuffer = std::make_shared<Framebuffer>(1.0f, 1.0f);
    m_LightPassFramebuffer->Bind();
    m_LightPassFramebuffer->CreateAttachment(FramebufferAttachmentType::Color16);
    m_LightPassFramebuffer->Unbind();

	m_BlurFramebuffer1 = std::make_shared<Framebuffer>(1.0f, 1.0f);
	m_BlurFramebuffer1->Bind();
	m_BlurFramebuffer1->CreateAttachment(FramebufferAttachmentType::Color16);
	m_BlurFramebuffer1->Unbind();

	m_BlurFramebuffer2 = std::make_shared<Framebuffer>(1.0f, 1.0f);
	m_BlurFramebuffer2->Bind();
	m_BlurFramebuffer2->CreateAttachment(FramebufferAttachmentType::Color16);
	m_BlurFramebuffer2->Unbind();

    m_ViewportFramebuffer = std::make_shared<Framebuffer>(1.0f, 1.0f);
    m_ViewportFramebuffer->Bind();
    m_ViewportFramebuffer->CreateAttachment(FramebufferAttachmentType::Color);
    m_ViewportFramebuffer->CreateAttachment(FramebufferAttachmentType::Depth);
    m_ViewportFramebuffer->Unbind();
    
    m_GeometryPassShader = std::make_shared<Shader>(Files::ContentFolderPath + R"(\shaders\deferred\geometry-pass.glsl)");
    m_CombinationPassShader = std::make_shared<Shader>(Files::ContentFolderPath + R"(\shaders\deferred\combination-pass.glsl)");
    m_LightPassShader = std::make_shared<Shader>(Files::ContentFolderPath + R"(\shaders\deferred\light-pass-mesh.glsl)");
    m_BrighnessFilterShader = std::make_shared<Shader>(Files::ContentFolderPath + R"(\shaders\brightness-filter.glsl)");
    m_BlurShader = std::make_shared<Shader>(Files::ContentFolderPath + R"(\shaders\blur.glsl)");
    m_ShadowPassShader = std::make_shared<Shader>(Files::ContentFolderPath + R"(\shaders\shadow-pass.glsl)");

    CreateRandomShadowMapSamples();
}

void Renderer::Deinitialize()
{
    
}

void Renderer::Update()
{
	if (m_ViewportFramebuffer->GetWidth() != m_ViewportSize.x || m_ViewportFramebuffer->GetHeight() != m_ViewportSize.y)
	{
		m_GeometryFramebuffer->Resize(m_ViewportSize.x, m_ViewportSize.y);
		m_ShadowMapsFramebuffer->Resize(m_ViewportSize.y);
		m_LightPassFramebuffer->Resize(m_ViewportSize.x, m_ViewportSize.y);
		m_ViewportFramebuffer->Resize(m_ViewportSize.x, m_ViewportSize.y);
		m_BlurFramebuffer1->Resize(m_ViewportSize.x, m_ViewportSize.y);
        m_BlurFramebuffer2->Resize(m_ViewportSize.x, m_ViewportSize.y);
		m_Engine->GetCamera()->SetProjection(90.0f, 1.0f * m_ViewportSize.x / m_ViewportSize.y, 1.0f, m_FarPlane);
	}

    std::shared_ptr<Scene> scene = m_Engine->GetLoadedScene();
    Camera* camera = m_Engine->GetCamera();
    
    m_RendererAPI->SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    std::vector<std::shared_ptr<Component>> components = scene->GetAllComponents();

    GeometryPass(components, camera);
    LightPass(components, camera);
    BloomPass();
    CombinationPass(components, camera);

    while (m_Commands.size()) {
        m_Commands.front()(m_RendererAPI);
        m_Commands.pop();
    }
}

void Renderer::ResizeViewport(glm::vec2 size)
{
    m_ViewportSize = 2.0f * size;
}

void Renderer::SubmitRenderCommand(const std::function<void(RendererAPI* renderAPI)>& command)
{
    m_Commands.push(command);
}

std::shared_ptr<Framebuffer> Renderer::GetGeometryPassFramebuffer() const
{
    return m_GeometryFramebuffer;
}

std::shared_ptr<Framebuffer> Renderer::LightPassFramebuffer() const
{
    return m_LightPassFramebuffer;
}

std::shared_ptr<Framebuffer> Renderer::GetViewport() const
{
    return m_ViewportFramebuffer;
}

void Renderer::GeometryPass(const std::vector<std::shared_ptr<Component>>& components, Camera* camera)
{
    m_RendererAPI->BeginRenderPass("geometry pass", m_GeometryFramebuffer, m_GeometryPassShader, camera->GetMatrix(), camera->GetPosition());

    m_RendererAPI->EnableBlending();
    m_RendererAPI->SetBlendFunction(BlendFactor::SourceAlpha, BlendFactor::OneMinusSourceAlpha);

    m_RendererAPI->ClearColorTarget();
    m_RendererAPI->ClearDepthTarget();

    for (const std::shared_ptr<Component>& component : components)
    {
        if (std::shared_ptr<StaticMeshComponent> meshComponent = std::dynamic_pointer_cast<StaticMeshComponent>(component))
        {
            if (std::shared_ptr<StaticMesh> mesh = meshComponent->GetStaticMesh()) 
            {
                m_RendererAPI->SubmitMesh(mesh, meshComponent->GetTransform()); // TODO: Need a hierarchical Transform
            }
        }
    }

    m_RendererAPI->EndRenderPass();
}

void Renderer::BloomPass() 
{
    m_RendererAPI->BeginRenderPass("Blur filter", m_BlurFramebuffer1, m_BrighnessFilterShader, glm::mat4(1.0f), glm::vec3(0.0f)); // Temporary step, may be removed after PBR bloom will be added :)

    m_RendererAPI->ClearColorTarget();

    m_BrighnessFilterShader->SetInt("u_Image", BrightnessFilterTextureSlot);
    m_LightPassFramebuffer->GetAttachment(0)->Bind(BrightnessFilterTextureSlot);

	m_RendererAPI->SubmitQuad(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f);

    m_RendererAPI->EndRenderPass();

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

		m_RendererAPI->BeginRenderPass("Blur pass", write, m_BlurShader, glm::mat4(1.0f), glm::vec3(0.0f));

		m_RendererAPI->ClearColorTarget();

		m_BlurShader->SetBool("u_HorizontalPass", i % 2 == 0);
		m_BlurShader->SetFloat("u_PixelWidth", 1.0f / read->GetWidth());
		m_BlurShader->SetFloat("u_PixelHeight", 1.0f / read->GetHeight());
		m_BlurShader->SetFloat2("u_ScreenSize", write->GetWidth(), write->GetHeight());

		m_BlurShader->SetInt("u_Image", BlurReadTextreSlot);
		read->GetAttachment(0)->Bind(BlurReadTextreSlot);

		m_RendererAPI->SubmitQuad(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f);

		m_RendererAPI->EndRenderPass();
    }
}

void Renderer::LightPass(const std::vector<std::shared_ptr<Component>>& components, Camera* camera)
{
    m_GeometryFramebuffer->GetAttachment(0)->Bind(AlbedoTextureSlot);
    m_GeometryFramebuffer->GetAttachment(1)->Bind(PositionTextureSlot);
    m_GeometryFramebuffer->GetAttachment(2)->Bind(NormalTextureSlot);
    m_GeometryFramebuffer->GetAttachment(3)->Bind(RoughnessMetalicTextureSlot);
    m_ShadowMapRandomSamples->Bind(ShadowMapRandomSamplesTextureSlot);

    m_RendererAPI->BeginRenderPass("lighting pass", m_LightPassFramebuffer, m_LightPassShader, camera->GetMatrix(), camera->GetPosition());
    m_RendererAPI->ClearColorTarget();
    m_RendererAPI->ClearDepthTarget();
    m_RendererAPI->EndRenderPass();


    for (const std::shared_ptr<Component>& outerComponent : components)
    {
        if (std::shared_ptr<PointLightComponent> light = std::dynamic_pointer_cast<PointLightComponent>(outerComponent))
        {
			m_RendererAPI->BeginRenderPass("shadow maps", m_ShadowMapsFramebuffer, m_ShadowPassShader, glm::mat4(1), light->GetPosition());
			m_RendererAPI->DisableBlending();

            if (light->IsShadowCasting())
            {
                m_ShadowPassShader->SetFloat("u_FarPlane", m_FarPlane);
                for (int32_t i = 0; i < 6; ++i) {
					m_ShadowPassShader->SetMat4("u_ViewProjection[" + std::to_string(i) + "]", m_LightPerspective * light->GetShadowMapPassCameraTransformation(i));
				}
                
				m_ShadowMapsFramebuffer->AttachLayers();

				m_RendererAPI->ClearDepthTarget();

				for (const std::shared_ptr<Component>& component : components)
				{
					if (std::shared_ptr<StaticMeshComponent> meshComponent = std::dynamic_pointer_cast<StaticMeshComponent>(component))
					{
						if (std::shared_ptr<StaticMesh> mesh = meshComponent->GetStaticMesh()) {
							m_RendererAPI->SubmitMeshRaw(mesh, meshComponent->GetTransform()); // TODO: Need a hierarchical Transform
						}
					}
				}
            }
            else
            {
                for (int32_t i = 0; i < 6; ++i) {
                    m_ShadowMapsFramebuffer->AttachFace(i);
                    m_RendererAPI->ClearDepthTarget();
                }
            }

			m_RendererAPI->EndRenderPass();

            m_RendererAPI->BeginRenderPass("lighting pass", m_LightPassFramebuffer, m_LightPassShader, camera->GetMatrix(), camera->GetPosition());

            m_RendererAPI->EnableBlending();
            m_RendererAPI->SetBlendFunction(BlendFactor::One, BlendFactor::One);

            m_LightPassShader->SetInt("u_Albedo", AlbedoTextureSlot);
            m_LightPassShader->SetInt("u_Position", PositionTextureSlot);
            m_LightPassShader->SetInt("u_Normal", NormalTextureSlot);
            m_LightPassShader->SetInt("u_RoughnessMetalic", RoughnessMetalicTextureSlot);
            m_LightPassShader->SetFloat("u_FarPlane", m_FarPlane);

            m_LightPassShader->SetInt("u_RandomSamples", ShadowMapRandomSamplesTextureSlot);
            m_LightPassShader->SetFloat("u_FilterSize", m_FilterSize);
            m_LightPassShader->SetFloat("u_ShadowMapPixelSize", 1.0f / m_ShadowMapsFramebuffer->GetSize());

            m_RendererAPI->EnableFaceCulling();
            m_RendererAPI->SubmitLightMesh(light, m_ShadowMapsFramebuffer->GetDepthAttachment());
            m_RendererAPI->DisableFaceCulling();

            m_RendererAPI->EndRenderPass();
        }
    }
}

void Renderer::CreateRandomShadowMapSamples()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dist(-0.5, 0.5);

	std::vector<float> samples;
	samples.reserve(m_ShadowMapFiltersCount * m_FilterSize * m_FilterSize * 3);
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

					samples.push_back((u + 0.5f + dist(gen)));
					samples.push_back((v + 0.5f + dist(gen)));
					samples.push_back((p + 0.5f + dist(gen)));
				}
			}
		}
	}

	Texture2DImportParameters parameters;
	parameters.WrapS = GL_REPEAT;
	parameters.WrapT = GL_REPEAT;
	parameters.InternalFormat = GL_RGB8;
	parameters.ExternalFormat = GL_RGB;
	parameters.Filtering = GL_NEAREST;
	parameters.DataType = GL_UNSIGNED_BYTE;

	Texture2DData data;
	data.Width = m_FilterSize * m_FilterSize;
	data.Height = m_ShadowMapFiltersCount;
	data.Channels = 2;
	data.Data = (uint8_t*)samples.data();

	m_ShadowMapRandomSamples = std::make_shared<Texture2D>(parameters, data);
}

void Renderer::CombinationPass(const std::vector<std::shared_ptr<Component>>& components, Camera* camera)
{
	m_GeometryFramebuffer->GetAttachment(0)->Bind(AlbedoTextureSlot);
	m_GeometryFramebuffer->GetAttachment(3)->Bind(RoughnessMetalicTextureSlot);

    m_LightPassFramebuffer->GetAttachment(0)->Bind(LightPassTextureSlot);
    if (m_BlurPassCount % 2) 
    {
		m_BlurFramebuffer2->GetAttachment(0)->Bind(BloomPassTextureSlot);
    }
    else
    {
		m_BlurFramebuffer1->GetAttachment(0)->Bind(BloomPassTextureSlot);
    }

    m_RendererAPI->BeginRenderPass("combination pass", m_ViewportFramebuffer, m_CombinationPassShader, camera->GetMatrix(), camera->GetPosition());

    m_RendererAPI->DisableBlending();
    m_RendererAPI->ClearColorTarget();
    m_RendererAPI->ClearDepthTarget();

    m_CombinationPassShader->SetInt("u_Albedo", AlbedoTextureSlot);
    m_CombinationPassShader->SetInt("u_RoughnessMetalic", RoughnessMetalicTextureSlot);
    m_CombinationPassShader->SetInt("u_Light", LightPassTextureSlot);
    m_CombinationPassShader->SetInt("u_Bloom", BloomPassTextureSlot);

    m_RendererAPI->SubmitQuad(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f);

    m_RendererAPI->EndRenderPass();
}
