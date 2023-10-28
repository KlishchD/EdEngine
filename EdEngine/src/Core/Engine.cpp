#include "Engine.h"

#include "imgui.h"

#include "LogManager.h"
#include "Window.h"
#include "Assets/AssetManager.h"

#include "Rendering/Renderer.h"
#include "Rendering/Framebuffer.h"
#include "Rendering/CubeFramebuffer.h"
#include "Utils/Files.h"

Engine* engine = nullptr;

Engine& Engine::Create()
{
    if (!engine)
    {
        engine = new Engine;
    }
    return *engine;
}

Engine& Engine::Get()
{
    return *engine;
}

void Engine::Delete()
{
    engine->Stop();
    while (engine->IsRunning()) {} // TODO: make it better
    delete engine;
    engine = nullptr;
}

void Engine::Start()
{
    m_IsRunning = true;
}

void Engine::Stop()
{
    m_IsRunning = false;
}

void Engine::PushUpdate(float DeltaTime)
{
    for (auto& object: m_UpdateSubscribers)
    {
        object(DeltaTime);
    }
}

void Engine::Initialize()
{
    Window::Create("Window", 1024, 960);

    m_Renderer = std::make_shared<Renderer>();
    m_Renderer->Initialize(this);
    
    std::shared_ptr<AssetManager> assetManager = std::make_shared<AssetManager>();
    m_Managers.push_back(assetManager);
    m_Managers.push_back(std::make_shared<LogManager>());

    for (std::shared_ptr<BaseManager>& manager: m_Managers)
    {
        manager->Initialize(this);
    }

    m_Scene = assetManager->CreateOrLoadScene(Files::ContentFolderPath + R"(scenes\main_test.edscene)");

    m_Camera = Camera(90.0f, 1240.0f / 960.0f, 1.0f, 15000.0f);
}

void Engine::Deinitialize()
{
    Window::Delete();
    
    for (std::shared_ptr<BaseManager>& manager: m_Managers)
    {
        manager->Deinitialize();
    }

    m_Renderer->Deinitialize();
}

bool Engine::IsRunning()
{
    return m_IsRunning;
}

void Engine::Update()
{
    std::chrono::time_point now = std::chrono::system_clock::now();

    float DeltaTime = std::chrono::duration_cast<std::chrono::microseconds>(now - m_PreviousFrameTime).count() / 1000000.0f;

    m_PreviousFrameTime = now;

    PushUpdate(DeltaTime);
    
    Window& window = Window::Get();
    
    window.Clear();

    m_Renderer->Update();
   
    window.BeginUIFrame();
    
    for (std::shared_ptr<Widget>& widget: m_Widgets)
    {
        widget->Tick(DeltaTime);
    }
    
    window.EndUIFrame();

    window.Update();

    m_IsRunning = window.IsRunning();
}

void Engine::InputAction(Key key, Action action)
{
    for (InputEvent& event: m_InputEvents)
    {
        if (event.Key == key && event.Action == action)
        {
            event.Response();
        }
    }
}


void Engine::SubscribeToInput(const InputEvent& inputEvent)
{
    m_InputEvents.push_back(inputEvent);
}

void Engine::SubscribeToInput(Key key, Action action, std::function<void()> response)
{
    SubscribeToInput( { key, action, response });
}

void Engine::SubscribeToInput(Key key, const std::vector<Action>& actions, std::function<void()> response)
{
    for (const Action& action: actions)
    {
        SubscribeToInput(key, action, response);
    }
}

void Engine::SubscribeToUpdate(std::function<void(float)> function)
{
    m_UpdateSubscribers.push_back(function);    
}

void Engine::AddWidget(const std::shared_ptr<Widget>& widget)
{
    m_Widgets.push_back(widget);
    widget->Initialize();
}

void Engine::AddManager(const std::shared_ptr<BaseManager>& manager)
{
    m_Managers.push_back(manager);
    manager->Initialize(this);
}

std::shared_ptr<Scene> Engine::GetLoadedScene() const
{
    return m_Scene;
}

Camera* Engine::GetCamera()
{
    return &m_Camera;
}

std::shared_ptr<Renderer> Engine::GetRenderer()
{
    return m_Renderer;
}
