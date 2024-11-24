#pragma once

#include "Core/Engine.h"
#include "Core/Rendering/RenderPassSpecification.h"

class Actor;
class Component;
class Texture2D;
class Shader;

ED_CLASS(Editor) : public BaseManager
{
    ED_CLASS_BODY(Editor, BaseManager)
public:
    virtual void Initialize(Engine* engine) override;
    virtual void Deinitialize() override;

    virtual void Update(float DeltaTime) override;

    void SetUpInputs(Engine* engine);
    
    void SetSelectedActor(std::shared_ptr<Actor> actor);
    std::shared_ptr<Actor> GetSelectedActor() const;

    void SetSelectedComponent(std::shared_ptr<Component> component);
    std::shared_ptr<Component> GetSelectedComponent() const;

    float GetCameraSpeed() const;
    void SetCameraSpeed(float speed);

    glm::vec2 GetCameraRotationSpeed() const;
    void SetCameraRotationSpeed(glm::vec2 speed);

    void SetSelectedAsset(std::shared_ptr<Asset> asset);
    std::shared_ptr<Asset> GetSelectedAsset() const;

    void SetViewportIsActive(bool state);
protected:
	void UpdateMousePosition(float DeltaTime);

protected:
    std::shared_ptr<class AssetManager> m_AssetManager;

    glm::vec3 m_MovementDirection = glm::vec3(0.0f);
    
    float m_CameraSpeed = 100.0f;
    glm::vec2 m_CameraRotationSpeed = glm::vec2(10.0f);
    
    glm::vec2 m_MousePosition = glm::vec2(0.0f);

    bool m_IsLeftMouseButtonClicked = false;
    bool m_IsRightMouseButtonClicked = false;

    std::shared_ptr<Texture2D> m_LightIcon;

    std::shared_ptr<Asset> m_SelectedAsset;
    
    std::shared_ptr<Actor> m_SelectedActor;
    std::shared_ptr<Component> m_SelectedComponent;

    glm::i32vec2 m_ViewportSize = { 1.0f, 1.0f };
    bool m_IsViewportActive = false;

    std::shared_ptr<class Renderer> m_Renderer;
    std::shared_ptr<class Window> m_Window;
    Engine* m_Engine = nullptr;
};
