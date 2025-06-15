#pragma once

#include "EditorCamera.h"

class Texture;
struct Asset;

class Editor
{
public:
    Editor();

    static inline Editor& Get()
    {
        ED_ASSERT(s_Editor, "Editor was not yet created.");
        return *s_Editor;
    }

    void Initialize(Engine* engine);
    void Deinitialize();

    void Update(f32 deltaSeconds);

    void SetUpInputs(Engine* engine);
    
    void SetSelectedEntity(Entity* entity) { m_SelectedEntity = entity; }
    Entity* GetSelectedEntity() const { return m_SelectedEntity; }

    inline void SetFeatureParametersLogic(std::function<void()> func) { m_FeatureParametersLogic = func; }
    inline std::function<void()> GetFeatureParametersLogic() const { return m_FeatureParametersLogic; }

    const EditorCamera& GetCamera() const { return m_Camera; }
    EditorCamera& GetCamera() { return m_Camera; }

    f32 GetCameraSpeed() const { return m_Camera.CameraSpeed; }
    void SetCameraSpeed(f32 speed) { m_Camera.CameraSpeed = speed; }

    glm::vec2 GetCameraRotationSpeed() const { m_Camera.CameraRotationSpeed; }
    void SetCameraRotationSpeed(glm::vec2 speed) { m_Camera.CameraRotationSpeed = speed; }

    void SetSelectedAsset(Asset* asset) { m_SelectedAsset = asset; }
    Asset* GetSelectedAsset() const { return m_SelectedAsset; }

    void SetViewportIsActive(bool state) { m_IsViewportActive = state; }
    bool IsViewportActive() const { return m_IsViewportActive; }

    template<typename T>
    T* CreateWidget()
    {
        // TODO: Add widget pool? and reuse it)
        T* widget = new T();
        widget->Initialize();
        m_Widgets.Add(widget);
        return widget;
    }

    virtual ~Editor();
protected:
	void UpdateMousePosition(f32 DeltaTime);

    void BeginUIFrame();
    void EndUIFrame();

    static inline void StaticUpdate(f32 deltaTime)
    {
        Get().Update(deltaTime);
    }
protected:
    static inline Editor* s_Editor = nullptr;
    
    EditorCamera m_Camera;

    glm::vec2 m_MousePosition = glm::vec2(0.0f);

    bool m_IsLeftMouseButtonClicked = false;
    bool m_IsRightMouseButtonClicked = false;

    Texture* m_LightIcon = nullptr;
    Asset* m_SelectedAsset = nullptr;
    
    Entity* m_SelectedEntity = nullptr;

    glm::i32vec2 m_ViewportSize = { 1.0f, 1.0f };
    bool m_IsViewportActive = false;

    Window* m_Window = nullptr;
    Engine* m_Engine = nullptr;

    Array<Widget*> m_Widgets;

    std::function<void()> m_FeatureParametersLogic;
};
