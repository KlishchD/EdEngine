#pragma once

#include <chrono>
#include <functional>
#include <vector>

#include "BaseManager.h"
#include "Input.h"
#include "Scene.h"
#include "Widget.h"
#include "Math/Camera.h"

class Renderer;

class Engine
{
public:
    static Engine& Create();
    static Engine& Get();
    static void Delete();

    void Start();
    void Stop();

    void Initialize();
    void Deinitialize();
    
    bool IsRunning();
    
    void Update();

    // Input Events

    void InputAction(Key key, Action action);

    void SubscribeToInput(const InputEvent& inputEvent);
    void SubscribeToInput(Key key, Action action, std::function<void()> response);
    void SubscribeToInput(Key key, const std::vector<Action>& actions, std::function<void()> response);

    // Update Events

    void SubscribeToUpdate(std::function<void(float)> function);

    template <class T>
    void AddWidget()
    {
        std::shared_ptr<Widget> widget = std::make_shared<T>();
        widget->Initialize();
        m_Widgets.push_back(widget);
    }
    
    void AddWidget(const std::shared_ptr<Widget>& widget);
    void AddManager(const std::shared_ptr<BaseManager>& manager);
    
    template <class T>
    std::shared_ptr<T> GetManager() const
    {
        for (std::shared_ptr<BaseManager> manager: m_Managers)
        {
            if (dynamic_cast<T*>(manager.get()))
            {
                return std::static_pointer_cast<T>(manager);
            }
        }
        return nullptr;
    }

    std::shared_ptr<Scene> GetLoadedScene() const;

    Camera* GetCamera();
    
    std::shared_ptr<Renderer> GetRenderer();
private:
    std::chrono::time_point<std::chrono::system_clock> m_PreviousFrameTime = std::chrono::system_clock::now();
    
    std::vector<InputEvent> m_InputEvents;

    std::vector<std::function<void(float)>> m_UpdateSubscribers;

    std::vector<std::shared_ptr<BaseManager>> m_Managers;
    std::shared_ptr<Scene> m_Scene;

    std::vector<std::shared_ptr<Widget>> m_Widgets;

    std::shared_ptr<Renderer> m_Renderer;
    
    bool m_IsRunning = true;


    Camera m_Camera = Camera(glm::radians(90.0f), 1240.0f / 960.0f, 1.0f, 1500.0f);

// Rendering
   
// 
    
    Engine() = default;

    void PushUpdate(float DeltaTime);
};

