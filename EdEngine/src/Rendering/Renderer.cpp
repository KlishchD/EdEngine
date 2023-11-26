#include "Renderer.h"
#include "Core/Engine.h"
#include "Core/Scene.h"
#include "Utils/Files.h"
#include "Core/Macros.h"

#include <random>

void Renderer::Initialize(Engine* engine)
{
	ED_LOG(Renderer, info, "Started initalizing Renderer")

    m_Engine = engine;
    m_RendererAPI = &RendererAPI::Get();

    m_BlurFramebuffer1 = std::make_shared<Framebuffer>(1.0f, 1.0f);
	m_BlurFramebuffer1->Bind();
	m_BlurFramebuffer1->CreateAttachment(FramebufferAttachmentType::Color16);
	m_BlurFramebuffer1->Unbind();

	m_BlurFramebuffer2 = std::make_shared<Framebuffer>(1.0f, 1.0f);
	m_BlurFramebuffer2->Bind();
	m_BlurFramebuffer2->CreateAttachment(FramebufferAttachmentType::Color16);
	m_BlurFramebuffer2->Unbind();

    m_BlurShader = std::make_shared<Shader>(Files::ContentFolderPath + R"(\shaders\blur.glsl)");
    
    SetupGeometryRenderPass();
    SetupLightRenderPass();
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
	if (m_GeometryPassSpecification.framebuffer->GetWidth() != m_ViewportSize.x || m_GeometryPassSpecification.framebuffer->GetHeight() != m_ViewportSize.y)
	{
        m_GeometryPassSpecification.framebuffer->Resize(m_ViewportSize.x, m_ViewportSize.y);
		m_LightPassSpecification.framebuffer->Resize(m_ViewportSize.x, m_ViewportSize.y);
		m_CombinationPassSpecification.framebuffer->Resize(m_ViewportSize.x, m_ViewportSize.y);

		m_ShadowPassSpecification.framebuffer->Resize(m_ViewportSize.y);

		m_BlurFramebuffer1->Resize(m_ViewportSize.x, m_ViewportSize.y);
        m_BlurFramebuffer2->Resize(m_ViewportSize.x, m_ViewportSize.y);

        m_Engine->GetCamera()->SetProjection(90.0f, 1.0f * m_ViewportSize.x / m_ViewportSize.y, 1.0f, m_FarPlane);
	}

    std::shared_ptr<Scene> scene = m_Engine->GetLoadedScene();
    Camera* camera = m_Engine->GetCamera();
  
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
    return std::static_pointer_cast<Framebuffer>(m_GeometryPassSpecification.framebuffer);
}

std::shared_ptr<Framebuffer> Renderer::LightPassFramebuffer() const
{
    return std::static_pointer_cast<Framebuffer>(m_LightPassSpecification.framebuffer);
}

std::shared_ptr<Framebuffer> Renderer::GetViewport() const
{
    return std::static_pointer_cast<Framebuffer>(m_CombinationPassSpecification.framebuffer);
}

void Renderer::GeometryPass(const std::vector<std::shared_ptr<Component>>& components, Camera* camera)
{
    m_GeometryPassSpecification.projectionViewMatrix = camera->GetMatrix();
    m_GeometryPassSpecification.viewPosition = camera->GetPosition();

    m_RendererAPI->BeginRenderPass(m_GeometryPassSpecification);

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
    m_RendererAPI->BeginRenderPass(m_BrighnessFilterPassSpecification); // Temporary step, may be removed after PBR bloom will be added :)


    m_BrighnessFilterPassSpecification.shader->SetInt("u_Image", BrightnessFilterTextureSlot);
    m_LightPassSpecification.framebuffer->GetAttachment(0)->Bind(BrightnessFilterTextureSlot);

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
    m_GeometryPassSpecification.framebuffer->GetAttachment(0)->Bind(AlbedoTextureSlot);
    m_GeometryPassSpecification.framebuffer->GetAttachment(1)->Bind(PositionTextureSlot);
    m_GeometryPassSpecification.framebuffer->GetAttachment(2)->Bind(NormalTextureSlot);
    m_GeometryPassSpecification.framebuffer->GetAttachment(3)->Bind(RoughnessMetalicTextureSlot);
    m_ShadowMapRandomSamples->Bind(ShadowMapRandomSamplesTextureSlot);

    m_LightPassSpecification.projectionViewMatrix = camera->GetMatrix();
    m_LightPassSpecification.viewPosition = camera->GetPosition();

    m_RendererAPI->BeginRenderPass(m_LightPassSpecification);
    m_RendererAPI->ClearColorTarget();
    m_RendererAPI->ClearDepthTarget();
    m_RendererAPI->EndRenderPass();


    for (const std::shared_ptr<Component>& outerComponent : components)
    {
        if (std::shared_ptr<PointLightComponent> light = std::dynamic_pointer_cast<PointLightComponent>(outerComponent))
        {
            m_ShadowPassSpecification.viewPosition = light->GetPosition();

			m_RendererAPI->BeginRenderPass(m_ShadowPassSpecification);
            m_ShadowPassSpecification.shader->SetFloat("u_FarPlane", m_FarPlane);

            std::shared_ptr<CubeFramebuffer> frambuffer = std::static_pointer_cast<CubeFramebuffer>(m_ShadowPassSpecification.framebuffer);

            if (light->IsShadowCasting())
            {
                for (int32_t i = 0; i < 6; ++i) {
                    m_ShadowPassSpecification.shader->SetMat4("u_ViewProjection[" + std::to_string(i) + "]", m_LightPerspective * light->GetShadowMapPassCameraTransformation(i));
				}
                
                frambuffer->AttachLayers();

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
                    frambuffer->AttachFace(i);
					m_RendererAPI->ClearDepthTarget();
				}
			}

			m_RendererAPI->EndRenderPass();

            m_LightPassSpecification.projectionViewMatrix = camera->GetMatrix();
            m_LightPassSpecification.viewPosition = camera->GetPosition();

            m_RendererAPI->BeginRenderPass(m_LightPassSpecification);

            std::shared_ptr<Shader> shader = m_LightPassSpecification.shader;

            shader->SetInt("u_Albedo", AlbedoTextureSlot);
            shader->SetInt("u_Position", PositionTextureSlot);
            shader->SetInt("u_Normal", NormalTextureSlot);
            shader->SetInt("u_RoughnessMetalic", RoughnessMetalicTextureSlot);
            shader->SetFloat("u_FarPlane", m_FarPlane);
            
            shader->SetInt("u_RandomSamples", ShadowMapRandomSamplesTextureSlot);
            shader->SetFloat("u_FilterSize", m_FilterSize);
            shader->SetFloat("u_ShadowMapPixelSize", 1.0f / m_ShadowPassSpecification.framebuffer->GetWidth());

            m_RendererAPI->EnableFaceCulling();
            m_RendererAPI->SubmitLightMesh(light, m_ShadowPassSpecification.framebuffer->GetDepthAttachment());
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

void Renderer::SetupLightRenderPass()
{
    m_LightPassSpecification.name = "Light pass";

	std::shared_ptr<Framebuffer> framebuffer = std::make_shared<Framebuffer>(1.0f, 1.0f);
    framebuffer->Bind();
    framebuffer->CreateAttachment(FramebufferAttachmentType::Color16);
    framebuffer->Unbind();

    m_LightPassSpecification.framebuffer = framebuffer;

    m_LightPassSpecification.shader = std::make_shared<Shader>(Files::ContentFolderPath + R"(\shaders\deferred\light-pass-mesh.glsl)");

    m_LightPassSpecification.sourceFactor = BlendFactor::One;
    m_LightPassSpecification.destinationFactor = BlendFactor::One;
}

void Renderer::SetupBrightnessFilterPass()
{
    m_BrighnessFilterPassSpecification.name = "Brighness filter";

    m_BrighnessFilterPassSpecification.framebuffer = m_BlurFramebuffer1;

    m_BrighnessFilterPassSpecification.shader = std::make_shared<Shader>(Files::ContentFolderPath + R"(\shaders\brightness-filter.glsl)");

    m_BrighnessFilterPassSpecification.clearColors = true;
}

void Renderer::SetupShadowRenderPass()
{
    m_ShadowPassSpecification.name = "Shadow pass";

	std::shared_ptr<CubeFramebuffer> framebuffer = std::make_shared<CubeFramebuffer>(1.0f);
    framebuffer->Bind();
    framebuffer->CreateAttachment(FramebufferAttachmentType::Depth);
    framebuffer->Unbind();

    m_ShadowPassSpecification.framebuffer = framebuffer;

    m_ShadowPassSpecification.shader = std::make_shared<Shader>(Files::ContentFolderPath + R"(\shaders\shadow-pass.glsl)");

    m_ShadowPassSpecification.blending = false;

    m_ShadowPassSpecification.clearDepth = true;
}

void Renderer::SetupCombinationRenderPass()
{
    m_CombinationPassSpecification.name = "Combination pass";

	std::shared_ptr<Framebuffer> framebuffer = std::make_shared<Framebuffer>(1.0f, 1.0f);
    framebuffer->Bind();
    framebuffer->CreateAttachment(FramebufferAttachmentType::Color);
    framebuffer->CreateAttachment(FramebufferAttachmentType::Depth);
    framebuffer->Unbind();

    m_CombinationPassSpecification.framebuffer = framebuffer;

    m_CombinationPassSpecification.shader = std::make_shared<Shader>(Files::ContentFolderPath + R"(\shaders\deferred\combination-pass.glsl)");

    m_CombinationPassSpecification.blending = false;

    m_CombinationPassSpecification.clearColors = true;
    m_CombinationPassSpecification.clearDepth = true;
}

void Renderer::SetupGeometryRenderPass()
{
	m_GeometryPassSpecification.name = "Geometry pass";

	std::shared_ptr<Framebuffer> framebuffer = std::make_shared<Framebuffer>(1.0f, 1.0f);
    framebuffer->Bind();
    framebuffer->CreateAttachment(FramebufferAttachmentType::Color);
    framebuffer->CreateAttachment(FramebufferAttachmentType::Position);
    framebuffer->CreateAttachment(FramebufferAttachmentType::Direction);
    framebuffer->CreateAttachment(FramebufferAttachmentType::Color);
    framebuffer->CreateAttachment(FramebufferAttachmentType::Depth);
    framebuffer->Unbind();

	m_GeometryPassSpecification.framebuffer = framebuffer;

    m_GeometryPassSpecification.shader = std::make_shared<Shader>(Files::ContentFolderPath + R"(\shaders\deferred\geometry-pass.glsl)");

    m_GeometryPassSpecification.blending = false;

    m_GeometryPassSpecification.clearColors = true;
    m_GeometryPassSpecification.clearDepth = true;
}

void Renderer::CombinationPass(const std::vector<std::shared_ptr<Component>>& components, Camera* camera)
{
    m_GeometryPassSpecification.framebuffer->GetAttachment(0)->Bind(AlbedoTextureSlot);
    m_GeometryPassSpecification.framebuffer->GetAttachment(3)->Bind(RoughnessMetalicTextureSlot);

    m_LightPassSpecification.framebuffer->GetAttachment(0)->Bind(LightPassTextureSlot);
    if (m_BlurPassCount % 2) 
    {
		m_BlurFramebuffer2->GetAttachment(0)->Bind(BloomPassTextureSlot);
    }
    else
    {
		m_BlurFramebuffer1->GetAttachment(0)->Bind(BloomPassTextureSlot);
    }


    m_CombinationPassSpecification.projectionViewMatrix = camera->GetMatrix();
    m_CombinationPassSpecification.viewPosition = camera->GetPosition();

    m_RendererAPI->BeginRenderPass(m_CombinationPassSpecification);

    std::shared_ptr<Shader> shader = m_CombinationPassSpecification.shader;

    shader->SetInt("u_Albedo", AlbedoTextureSlot);
    shader->SetInt("u_RoughnessMetalic", RoughnessMetalicTextureSlot);
    shader->SetInt("u_Light", LightPassTextureSlot);
    shader->SetInt("u_Bloom", BloomPassTextureSlot);

    m_RendererAPI->SubmitQuad(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f);

    m_RendererAPI->EndRenderPass();
}
