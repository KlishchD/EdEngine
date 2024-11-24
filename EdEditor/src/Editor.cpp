#include "Editor.h"
#include "Widgets/ActorDetailsWidget.h"
#include "Widgets/AssetDetails.h"
#include "Widgets/CameraDetailsWidget.h"
#include "Widgets/ContentBrowserWidget.h"
#include "Widgets/OptionsMenuWidget.h"
#include "Widgets/SceneTreeWidget.h"
#include "Widgets/ViewportWidget.h"
#include "Widgets/RendererStatsWidget.h"
#include "Helpers/FilesHelper.h"
#include "Core/Rendering/Textures/Texture2D.h"
#include "Core/Rendering/Renderer.h"
#include "Core/Rendering/Shader.h"
#include "Core/Window.h"
#include "Core/Assets/AssetManager.h"
#include "Core/Scene.h"
#include "Helpers/RenderingHelper.h"
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

    SetUpInputs(engine);

    m_MousePosition = m_Window->GetMousePosition();

    m_Engine->AddWidget<OptionsMenuWidget>();
    m_Engine->AddWidget<SceneTreeWidget>();
    m_Engine->AddWidget<ActorDetailsWidget>();
    m_Engine->AddWidget<CameraDetailsWidget>();
    m_Engine->AddWidget<ViewportWidget>();
    m_Engine->AddWidget<ContentBrowserWidget>();
    m_Engine->AddWidget<AssetDetails>();
    m_Engine->AddWidget<RendererStatsWidget>();
}

void Editor::Update(float DeltaTime)
{
    UpdateMousePosition(DeltaTime);

    Camera& camera = m_Engine->GetLoadedScene()->GetPlayerActor()->GetCameraComponent()->GetCamera();
    camera.AddPositionOffset(camera.GetForward() * m_MovementDirection.z * m_CameraSpeed * DeltaTime);
    camera.AddPositionOffset(camera.GetRight() * m_MovementDirection.x * m_CameraSpeed * DeltaTime);
    camera.AddPositionOffset(camera.GetUp() * m_MovementDirection.y * m_CameraSpeed * DeltaTime);
}

void Editor::SetUpInputs(Engine* engine)
{
    engine->SubscribeToInput(InputKey::W, InputAction::PressAndRepeat,          [this](InputKey key, InputAction action) { m_MovementDirection.z = 1.0f;                                                                            });
    engine->SubscribeToInput(InputKey::W, InputAction::Release,                 [this](InputKey key, InputAction action) { m_MovementDirection.z = 0.0f;                                                                            });

    engine->SubscribeToInput(InputKey::S, InputAction::PressAndRepeat,          [this](InputKey key, InputAction action) { m_MovementDirection.z = -1.0f;                                                                           });
    engine->SubscribeToInput(InputKey::S, InputAction::Release,                 [this](InputKey key, InputAction action) { m_MovementDirection.z = 0.0f;                                                                            });

    engine->SubscribeToInput(InputKey::A, InputAction::PressAndRepeat,          [this](InputKey key, InputAction action) { m_MovementDirection.x = -1.0f;                                                                           });
    engine->SubscribeToInput(InputKey::A, InputAction::Release,                 [this](InputKey key, InputAction action) { m_MovementDirection.x = 0.0f;                                                                            });

    engine->SubscribeToInput(InputKey::D, InputAction::PressAndRepeat,          [this](InputKey key, InputAction action) { m_MovementDirection.x = 1.0f;                                                                            });
    engine->SubscribeToInput(InputKey::D, InputAction::Release,                 [this](InputKey key, InputAction action) { m_MovementDirection.x = 0.0f;                                                                            });

    engine->SubscribeToInput(InputKey::E, InputAction::PressAndRepeat,          [this](InputKey key, InputAction action) { m_MovementDirection.y = 1.0f;                                                                            });
    engine->SubscribeToInput(InputKey::E, InputAction::Release,                 [this](InputKey key, InputAction action) { m_MovementDirection.y = 0.0f;                                                                            });

    engine->SubscribeToInput(InputKey::Q, InputAction::PressAndRepeat,          [this](InputKey key, InputAction action) { m_MovementDirection.y = -1.0f;                                                                           });
    engine->SubscribeToInput(InputKey::Q, InputAction::Release,                 [this](InputKey key, InputAction action) { m_MovementDirection.y = 0.0f;                                                                            });
    
    engine->SubscribeToInput(InputKey::LeftMouseClick, InputAction::Press,      [this](InputKey key, InputAction action) { m_IsLeftMouseButtonClicked = true;                                                                       });
    engine->SubscribeToInput(InputKey::LeftMouseClick, InputAction::Release,    [this](InputKey key, InputAction action) { m_IsLeftMouseButtonClicked = false;                                                                      });
    
    engine->SubscribeToInput(InputKey::RightMouseClick, InputAction::Press,     [this](InputKey key, InputAction action) { m_IsRightMouseButtonClicked = true;                                                                      });
    engine->SubscribeToInput(InputKey::RightMouseClick, InputAction::Release,   [this](InputKey key, InputAction action) { m_IsRightMouseButtonClicked = false;                                                                     });

    engine->SubscribeToInput(InputKey::U, InputAction::Press,                   [this](InputKey key, InputAction action) { m_Renderer->SetSSAOEnabled(!m_Renderer->IsSSAOEnabled());                                                });
    engine->SubscribeToInput(InputKey::B, InputAction::Press,                   [this](InputKey key, InputAction action) { m_Renderer->SetAAMethod(m_Renderer->GetAAMethod() == AAMethod::None ? AAMethod::TAA : AAMethod::None);   });
}

void Editor::SetSelectedActor(std::shared_ptr<Actor> actor)
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

void Editor::SetSelectedComponent(std::shared_ptr<Component> component)
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

void Editor::SetSelectedAsset(std::shared_ptr<Asset> asset)
{
    m_SelectedAsset = asset;
}

std::shared_ptr<Asset> Editor::GetSelectedAsset() const
{
    return m_SelectedAsset;
}

void Editor::SetViewportIsActive(bool state)
{
    m_IsViewportActive = state;
}

void Editor::UpdateMousePosition(float DeltaTime)
{
    glm::vec2 newMousePosition = m_Window->GetMousePosition() / static_cast<glm::vec2>(m_ViewportSize);

    if (m_MousePosition != newMousePosition && m_IsRightMouseButtonClicked)
    {
        glm::vec2 delta = newMousePosition - m_MousePosition;

        float yaw = DeltaTime * m_CameraRotationSpeed.x * delta.x;
        float pitch = DeltaTime * m_CameraRotationSpeed.y * delta.y;

        Camera& camera = m_Engine->GetLoadedScene()->GetPlayerActor()->GetCameraComponent()->GetCamera();
        camera.AddRotation(glm::vec3(pitch, yaw, 0.0f));
    }

    m_MousePosition = newMousePosition;
}

