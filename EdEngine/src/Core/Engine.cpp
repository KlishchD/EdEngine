#include "Engine.h"
#include "imgui.h"
#include "Window.h"
#include "Utils/RenderingHelper.h"
#include "Utils/Files.h"
#include "Widget.h"
#include "LogManager.h"
#include "Assets/AssetManager.h"
#include "Rendering/Renderer.h"
#include "Scene.h"
#include "Macros.h"

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
	ED_LOG(Engine, info, "Started initializing");

#undef CreateWindow // TODO: REMOVE IT :)
	m_Window = RenderingHelper::CreateWindow({ "EdEngine", 1280, 720 });
#define CreateWindow CreateWindowW

	std::shared_ptr<AssetManager> assetManager = std::make_shared<AssetManager>();
	m_Managers.push_back(assetManager);

	for (std::shared_ptr<BaseManager>& manager: m_Managers)
	{
		manager->Initialize(this);
	}

	m_Scene = assetManager->LoadScene(Files::ContentFolderPath + R"(scenes\main_test.edscene)");
	if (!m_Scene)
	{
		m_Scene = assetManager->CreateScene(Files::ContentFolderPath + R"(scenes\main_test.edscene)");
	}

	m_Scene->Initialize();

	m_Renderer = std::make_shared<Renderer>();
	m_Renderer->Initialize(this);

	ED_LOG(Engine, info, "Finished initializing")
}

void Engine::Deinitialize()
{
	m_Window->Close();

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

	m_DeltaSeconds = std::chrono::duration_cast<std::chrono::microseconds>(now - m_PreviousFrameTime).count() / 1000000.0f;

	m_PreviousFrameTime = now;

	m_Scene->Update(m_DeltaSeconds);

	PushUpdate(m_DeltaSeconds);
	
	m_Renderer->Update(m_DeltaSeconds);
   
	m_Renderer->BeginUIFrame();
	
	for (std::shared_ptr<Widget>& widget: m_Widgets)
	{
		widget->Tick(m_DeltaSeconds);
	}
	
	m_Renderer->EndUIFrame();

	m_Window->Update();
	m_IsRunning = m_Window->IsRunning();
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

void Engine::AddWidget(std::shared_ptr<Widget> widget)
{
	m_Widgets.push_back(widget);
	widget->Initialize();
}

void Engine::AddManager(std::shared_ptr<BaseManager> manager)
{
	m_Managers.push_back(manager);
	manager->Initialize(this);
}

std::shared_ptr<Scene> Engine::GetLoadedScene() const
{
	return m_Scene;
}

std::shared_ptr<Window> Engine::GetWindow() const
{
	return m_Window;
}

std::shared_ptr<Renderer> Engine::GetRenderer() const
{
	return m_Renderer;
}

float Engine::GetDeltaSeconds() const
{
	return m_DeltaSeconds;
}

Engine::~Engine()
{
	if (m_IsRunning)
	{
		Stop();
		Deinitialize();
	}
}
