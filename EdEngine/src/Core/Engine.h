#pragma once

#include "Input.h"
#include "BaseManager.h"

class Scene;
class Renderer;
class Widget;
class Window;

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

    // TODO: Inputs should not be here, engine should not know anything about them
    void RecieveInputAction(InputKey key, InputAction action);

    InputEventHandle SubscribeToInput(const InputEvent& inputEvent);
    InputEventHandle SubscribeToInput(InputKey key, InputAction action, std::function<void(InputKey, InputAction)> response);
    InputEventHandle SubscribeToInput(std::function<void(InputKey, InputAction)> response);

    void UnsubscribeFromInput(InputEventHandle handle);


    void SubscribeToUpdate(std::function<void(float)> response);

    template<typename T>
    void AddWidget()
    {
        std::shared_ptr<Widget> widget = std::make_shared<T>();
        widget->Initialize();
        m_Widgets.push_back(widget);
    }
    
    void AddWidget(std::shared_ptr<Widget> widget);
    void AddManager(std::shared_ptr<BaseManager> manager);
    
    template<typename T>
    std::shared_ptr<T> AddManager()
    {
        std::shared_ptr<T> manager = std::make_shared<T>();
        m_Managers.push_back(manager);
        manager->Initialize(this);
        return manager;
    }

    template<typename T>
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

	std::shared_ptr<Window> GetWindow() const;

    std::shared_ptr<Renderer> GetRenderer() const;

    float GetDeltaSeconds() const;

    ~Engine();
protected:
	void PushUpdate(float DeltaTime);

protected:
    std::shared_ptr<Window> m_Window;
    
    std::chrono::time_point<std::chrono::system_clock> m_PreviousFrameTime = std::chrono::system_clock::now();
    float m_DeltaSeconds;

    std::vector<InputEvent> m_InputEvents;

    std::vector<std::function<void(float)>> m_UpdateSubscribers;

    std::vector<std::shared_ptr<BaseManager>> m_Managers;
    std::shared_ptr<Scene> m_Scene;

    std::vector<std::shared_ptr<Widget>> m_Widgets;

    std::shared_ptr<Renderer> m_Renderer;
    
    bool m_IsRunning = true;

    Engine() = default;
};

