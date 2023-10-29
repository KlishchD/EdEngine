#include "Renderer.h"
#include "Core/Engine.h"
#include "Utils/Files.h"

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
    m_ShadowMapsFramebuffer->CreateAttachment(FramebufferAttachmentType::Distance);
    m_ShadowMapsFramebuffer->CreateAttachment(FramebufferAttachmentType::Depth);
    m_ShadowMapsFramebuffer->Unbind();
    
    m_LightPassFramebuffer = std::make_shared<Framebuffer>(1.0f, 1.0f);
    m_LightPassFramebuffer->Bind();
    m_LightPassFramebuffer->CreateAttachment(FramebufferAttachmentType::Color);
    m_LightPassFramebuffer->Unbind();

    m_ViewportFramebuffer = std::make_shared<Framebuffer>(1.0f, 1.0f);
    m_ViewportFramebuffer->Bind();
    m_ViewportFramebuffer->CreateAttachment(FramebufferAttachmentType::Color);
    m_ViewportFramebuffer->CreateAttachment(FramebufferAttachmentType::Depth);
    m_ViewportFramebuffer->Unbind();
    
    m_GeometryPassShader = std::make_shared<Shader>(Files::ContentFolderPath + R"(\shaders\deferred\geometry-pass.glsl)");
    m_CombinationPassShader = std::make_shared<Shader>(Files::ContentFolderPath + R"(\shaders\deferred\combination-pass.glsl)");
    m_LightPassShader = std::make_shared<Shader>(Files::ContentFolderPath + R"(\shaders\deferred\light-pass-mesh.glsl)");
    m_ShadowPassShader = std::make_shared<Shader>(Files::ContentFolderPath + R"(\shaders\shadow-pass.glsl)");
}

void Renderer::Deinitialize()
{
    
}

void Renderer::Update()
{
	if (m_ViewportFramebuffer->GetWidth() != m_ViewportSize.x || m_ViewportFramebuffer->GetHeight() != m_ViewportSize.y)
	{
		m_GeometryFramebuffer->Resize(m_ViewportSize.x, m_ViewportSize.y);
		m_ShadowMapsFramebuffer->Resize(std::min(m_ViewportSize.x, 1024.0f));
		m_LightPassFramebuffer->Resize(m_ViewportSize.x, m_ViewportSize.y);
		m_ViewportFramebuffer->Resize(m_ViewportSize.x, m_ViewportSize.y);
		m_Engine->GetCamera()->SetProjection(90.0f, 1.0f * m_ViewportSize.x / m_ViewportSize.y, 1.0f, 15000.0f);
	}

    std::shared_ptr<Scene> scene = m_Engine->GetLoadedScene();
    Camera* camera = m_Engine->GetCamera();
    
    m_RendererAPI->SetClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    std::vector<std::shared_ptr<Component>> components = scene->GetAllComponents();

    GeometryPass(components, camera);
    LightPass(components, camera);
    CombinationPass(components, camera);

    while (m_Commands.size()) {
        m_Commands.front()(m_RendererAPI);
        m_Commands.pop();
    }
}

void Renderer::ResizeViewport(glm::vec2 size)
{
    m_ViewportSize = size;
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
            if (std::shared_ptr<StaticMesh> mesh = meshComponent->GetStaticMesh()) {
                m_RendererAPI->SubmitMesh(mesh, meshComponent->GetTransform()); // TODO: Need a hierarchical Transform
            }
        }
    }

    m_RendererAPI->EndRenderPass();
}

void Renderer::LightPass(const std::vector<std::shared_ptr<Component>>& components, Camera* camera)
{
    m_GeometryFramebuffer->GetAttachment(0)->Bind(10);
    m_GeometryFramebuffer->GetAttachment(1)->Bind(11);
    m_GeometryFramebuffer->GetAttachment(2)->Bind(12);
    m_GeometryFramebuffer->GetAttachment(3)->Bind(13);


    m_RendererAPI->BeginRenderPass("lighting pass", m_LightPassFramebuffer, m_LightPassShader, camera->GetMatrix(), camera->GetPosition());
    m_RendererAPI->ClearColorTarget();
    m_RendererAPI->ClearDepthTarget();
    m_RendererAPI->EndRenderPass();


    for (const std::shared_ptr<Component>& outerComponent : components)
    {
        if (std::shared_ptr<PointLightComponent> light = std::dynamic_pointer_cast<PointLightComponent>(outerComponent))
        {
            if (light->IsShadowCasting())
            {
                m_RendererAPI->BeginRenderPass("shadow maps", m_ShadowMapsFramebuffer, m_ShadowPassShader, glm::mat4(1), glm::vec3(0.0f));
                m_RendererAPI->DisableBlending();

                for (int32_t i = 0; i < 6; ++i) {
                    m_ShadowMapsFramebuffer->AttachFace(i);
                    m_RendererAPI->SetNewCameraInformation(m_LightPerspective * light->GetShadowMapPassCameraTransformation(i), light->GetPosition());

                    m_RendererAPI->ClearColorTarget();
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

                m_RendererAPI->EndRenderPass();
            }

            m_RendererAPI->BeginRenderPass("lighting pass", m_LightPassFramebuffer, m_LightPassShader, camera->GetMatrix(), camera->GetPosition());

            m_RendererAPI->EnableBlending();
            m_RendererAPI->SetBlendFunction(BlendFactor::One, BlendFactor::One);

            m_LightPassShader->SetInt("u_Albedo", 10);
            m_LightPassShader->SetInt("u_Position", 11);
            m_LightPassShader->SetInt("u_Normal", 12);
            m_LightPassShader->SetInt("u_RoughnessMetalic", 13);

            m_RendererAPI->EnableFaceCulling();
            m_RendererAPI->SubmitLightMesh(light, m_ShadowMapsFramebuffer->GetAttachment(0));
            m_RendererAPI->DisableFaceCulling();

            m_RendererAPI->EndRenderPass();
        }
    }
}

void Renderer::CombinationPass(const std::vector<std::shared_ptr<Component>>& components, Camera* camera)
{
    m_LightPassFramebuffer->GetAttachment(0)->Bind(14);

    m_RendererAPI->BeginRenderPass("combination pass", m_ViewportFramebuffer, m_CombinationPassShader, camera->GetMatrix(), camera->GetPosition());

    m_RendererAPI->DisableBlending();
    m_RendererAPI->ClearColorTarget();
    m_RendererAPI->ClearDepthTarget();

    m_CombinationPassShader->SetInt("u_Albedo", 10);
    m_CombinationPassShader->SetInt("u_RoughnessMetalic", 13);
    m_CombinationPassShader->SetInt("u_Light", 14);

    m_RendererAPI->SubmitQuad(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f);

    m_RendererAPI->EndRenderPass();
}
