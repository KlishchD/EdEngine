﻿#pragma once

#include "Core/Engine.h"

class Actor;
class Component;
struct AssetDescriptor;
class Texture2D;
class Shader;

class Editor: public BaseManager
{
public:
    virtual void Initialize(Engine* engine) override;
    virtual void Deinitialize() override;

    void Update(float DeltaTime);

    void SetUpInputs(Engine* engine);
    
    void SetSelectedActor(const std::shared_ptr<Actor>& actor);
    std::shared_ptr<Actor> GetSelectedActor() const;

    void SetSelectedComponent(const std::shared_ptr<Component>& component);
    std::shared_ptr<Component> GetSelectedComponent() const;

    float GetCameraSpeed() const;
    void SetCameraSpeed(float speed);

    glm::vec2 GetCameraRotationSpeed() const;
    void SetCameraRotationSpeed(glm::vec2 speed);

    void SetSelectedAssetDescriptor(const std::shared_ptr<AssetDescriptor>& descriptor);
    std::shared_ptr<AssetDescriptor> GetSelectedAssetDescriptor() const;

    void SetViewportIsActive(bool state);
private:
    std::shared_ptr<class AssetManager> m_AssetManager;
    
    glm::vec3 m_MovementDirection = glm::vec3(0.0f);
    
    float m_CameraSpeed = 100.0f;
    glm::vec2 m_CameraRotationSpeed = glm::vec2(10.0f);
    
    glm::vec2 m_MousePosition = glm::vec2(0.0f);

    bool m_IsLeftMouseButtonClicked = false;
    bool m_IsRightMouseButtonClicked = false;

    std::shared_ptr<Texture2D> m_LightIcon;

    std::shared_ptr<AssetDescriptor> m_SelectedAssetDescriptor;
    
    std::shared_ptr<Actor> m_SelectedActor;
    std::shared_ptr<Component> m_SelectedComponent;
    
    std::shared_ptr<Shader> m_IconShader;

    glm::i32vec2 m_ViewportSize = { 1.0f, 1.0f };
    bool m_IsViewportActive = false;

    std::shared_ptr<class Renderer> m_Renderer;
    class Window* m_Window = nullptr;
    Engine* m_Engine = nullptr;
    
    void UpdateMousePosition(float DeltaTime);
};
