#include "Editor.h"
#include "Widgets/ActorDetailsWidget.h"
#include "Widgets/AssetDescriptorDetails.h"
#include "Widgets/CameraDetailsWidget.h"
#include "Widgets/ContentBrowserWidget.h"
#include "Widgets/OptionsMenuWidget.h"
#include "Widgets/SceneTreeWidget.h"
#include "Widgets/ViewportWidget.h"
#include "Utils/Files.h"
#include "Core/Rendering/Framebuffers/Framebuffer.h"
#include "Core/Rendering/Textures/Texture2D.h"
#include "Core/Rendering/Renderer.h"
#include "Core/Rendering/Shader.h"
#include "Core/Window.h"
#include "Core/Assets/AssetManager.h"
#include "Core/Scene.h"
#include "Utils/RenderingHelper.h"
#include "Core/Components/PointLightComponent.h"

void Editor::Deinitialize()
{
}

void Editor::Initialize(Engine* engine)
{
    m_Engine = engine;
    m_Renderer = engine->GetRenderer();
    m_Window = engine->GetWindow();

    m_AssetManager = engine->GetManager<AssetManager>();

    engine->SubscribeToUpdate([this](float DeltaTime) { Update(DeltaTime); });

    SetUpInputs(engine);

    m_MousePosition = m_Window->GetMousePosition();

    m_LightIcon = m_AssetManager->LoadTexture(RenderingHelper::GetDefaultBaseColorTexture2DImportParameters(Files::ContentFolderPath + R"(Editor\icons\light-bulb.png)"));

    m_Engine->AddWidget<OptionsMenuWidget>();
    m_Engine->AddWidget<SceneTreeWidget>();
    m_Engine->AddWidget<ActorDetailsWidget>();
    m_Engine->AddWidget<CameraDetailsWidget>();
    m_Engine->AddWidget<ViewportWidget>();
    m_Engine->AddWidget<ContentBrowserWidget>();
    m_Engine->AddWidget<AssetDescriptorDetails>();

    m_IconsPassSpecification.Name = "Editor icons";
    m_IconsPassSpecification.Framebuffer = m_Renderer->GetViewport();
    m_IconsPassSpecification.Shader = RenderingHelper::CreateShader(Files::ContentFolderPath + R"(Editor\shaders\IconShader.glsl)");
// 
//     std::shared_ptr<Actor> actor = engine->GetLoadedScene()->CreateActor<Actor>("Lights");
// 
//     for (int32_t i = -60; i <= 50; i += 10)
//     {
//         for (int32_t j = -10; j <= 5; j += 5)
//         {
//             std::shared_ptr<PointLightComponent> light = std::make_shared<PointLightComponent>();
//             light->GetTransform().SetTranslation(glm::vec3(i, 2.0f, j));
//             light->SetShadowCasting(false);
//             actor->RegisterComponent(light);
//         }
//     }
}

void Editor::Update(float DeltaTime)
{
    UpdateMousePosition(DeltaTime);

    Camera* camera = m_Engine->GetCamera();

    if (camera) {
        camera->AddPositionOffset(camera->GetForward() * m_MovementDirection.z * m_CameraSpeed * DeltaTime);
        camera->AddPositionOffset(camera->GetRight() * m_MovementDirection.x * m_CameraSpeed * DeltaTime);
        camera->AddPositionOffset(camera->GetUp() * m_MovementDirection.y * m_CameraSpeed * DeltaTime);
    }

//    std::shared_ptr<Actor> cube = m_Engine->GetLoadedScene()->GetActors().back();
//    cube->GetTransform().AddTranslation(DeltaTime * glm::vec3(-1.0f, 0.0f, 0.0f));

    m_Renderer->SubmitRenderCommand([this, camera](RenderingContext* context) {
        std::vector<std::shared_ptr<Component>> components = m_Engine->GetLoadedScene()->GetAllComponents();
        std::shared_ptr<Framebuffer> viewportFramebuffer = m_Renderer->GetViewport();

        m_IconsPassSpecification.ViewPosition = camera->GetPosition();

        m_Renderer->GetViewport()->CopyDepthAttachment(m_Renderer->GetGeometryPassFramebuffer());
        
        m_Renderer->BeginRenderPass(m_IconsPassSpecification, camera->GetView(), camera->GetProjection());
        
        glm::vec3 viewPosition = camera->GetPosition();
        for (const std::shared_ptr<Component>& component : components)
        {
            if (std::shared_ptr<PointLightComponent> light = std::dynamic_pointer_cast<PointLightComponent>(component))
            {
                glm::mat4 view = glm::lookAt(light->GetTransform().GetTranslation(), viewPosition, glm::vec3(0.0f, 1.0f, 0.0f));
                m_Renderer->SubmitIcon(m_LightIcon, glm::scale(glm::inverse(view), light->GetTransform().GetScale()));
            }
        }

        m_Renderer->EndRenderPass();
    });
}

void Editor::SetUpInputs(Engine* engine)
{
    engine->SubscribeToInput(Key::W, { Action::Repeat, Action::Press }, [this]() { if (m_IsViewportActive) m_MovementDirection.z = 1.0f; });
    engine->SubscribeToInput(Key::W, Action::Release, [this]() { m_MovementDirection.z = 0.0f; });

    engine->SubscribeToInput(Key::S, { Action::Repeat, Action::Press }, [this]() { if (m_IsViewportActive) m_MovementDirection.z = -1.0f; });
    engine->SubscribeToInput(Key::S, Action::Release, [this]() { m_MovementDirection.z = 0.0f; });

    engine->SubscribeToInput(Key::A, { Action::Repeat, Action::Press }, [this]() { if (m_IsViewportActive) m_MovementDirection.x = -1.0f; });
    engine->SubscribeToInput(Key::A, Action::Release, [this]() { m_MovementDirection.x = 0.0f; });

    engine->SubscribeToInput(Key::D, { Action::Repeat, Action::Press }, [this]() { if (m_IsViewportActive) m_MovementDirection.x = 1.0f; });
    engine->SubscribeToInput(Key::D, Action::Release, [this]() { m_MovementDirection.x = 0.0f; });

    engine->SubscribeToInput(Key::E, { Action::Repeat, Action::Press }, [this]() { if (m_IsViewportActive) m_MovementDirection.y = 1.0f; });
    engine->SubscribeToInput(Key::E, Action::Release, [this]() { m_MovementDirection.y = 0.0f; });

    engine->SubscribeToInput(Key::Q, { Action::Repeat, Action::Press }, [this]() { if (m_IsViewportActive) m_MovementDirection.y = -1.0f; });
    engine->SubscribeToInput(Key::Q, Action::Release, [this]() { m_MovementDirection.y = 0.0f; });
    
    engine->SubscribeToInput(Key::LeftMouseClick, Action::Press, [this]() { if (m_IsViewportActive) m_IsLeftMouseButtonClicked = true; });
    engine->SubscribeToInput(Key::LeftMouseClick, Action::Release, [this]() { m_IsLeftMouseButtonClicked = false; });
    
    engine->SubscribeToInput(Key::RightMouseClick, Action::Press, [this]() { if (m_IsViewportActive) m_IsRightMouseButtonClicked = true; });
    engine->SubscribeToInput(Key::RightMouseClick, Action::Release, [this]() { m_IsRightMouseButtonClicked = false; });

    engine->SubscribeToInput(Key::U, Action::Press, [this]() { m_Renderer->SetSSAOEnabled(!m_Renderer->IsSSAOEnabled()); });
    engine->SubscribeToInput(Key::B, Action::Press, [this]() { m_Renderer->SetAAMethod(m_Renderer->GetAAMethod() == AAMethod::None ? AAMethod::TAA : AAMethod::None); });
}

void Editor::SetSelectedActor(const std::shared_ptr<Actor>& actor)
{
    if (m_SelectedActor == actor)
    {
        m_SelectedActor.reset();
    }
    else
    {
        m_SelectedActor = actor;
    }

    m_SelectedComponent.reset();
}

std::shared_ptr<Actor> Editor::GetSelectedActor() const
{
    return m_SelectedActor;
}

void Editor::SetSelectedComponent(const std::shared_ptr<Component>& component)
{
    if (m_SelectedComponent == component)
    {
        m_SelectedComponent.reset();
    }
    else
    {
        m_SelectedComponent = component;
    }
}

std::shared_ptr<Component> Editor::GetSelectedComponent() const
{
    return m_SelectedComponent;
}

float Editor::GetCameraSpeed() const
{
    return m_CameraSpeed;
}

void Editor::SetCameraSpeed(float speed)
{
    m_CameraSpeed = speed;
}

glm::vec2 Editor::GetCameraRotationSpeed() const
{
    return m_CameraRotationSpeed;
}

void Editor::SetCameraRotationSpeed(glm::vec2 speed)
{
    m_CameraRotationSpeed = speed;
}

void Editor::SetSelectedAssetDescriptor(const std::shared_ptr<AssetDescriptor>& descriptor)
{
    m_SelectedAssetDescriptor = descriptor;
}

std::shared_ptr<AssetDescriptor> Editor::GetSelectedAssetDescriptor() const
{
    return m_SelectedAssetDescriptor;
}

void Editor::SetViewportIsActive(bool state)
{
    m_IsViewportActive = state;
}

void Editor::UpdateMousePosition(float DeltaTime)
{
    glm::vec2 newMousePosition = m_Window->GetMousePosition() / static_cast<glm::vec2>(m_ViewportSize);

    if (m_MousePosition != newMousePosition && m_IsRightMouseButtonClicked) //TODO: create transform class ;)
    {
        glm::vec2 delta = newMousePosition - m_MousePosition;

        float yaw = DeltaTime * m_CameraRotationSpeed.x * delta.x;
        float pitch = DeltaTime * m_CameraRotationSpeed.y * delta.y;

        m_Engine->GetCamera()->AddRotation(glm::vec3(pitch, yaw, 0.0f));
    }

    m_MousePosition = newMousePosition;
}

