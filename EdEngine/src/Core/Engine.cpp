#include "Engine.h"
#include "imgui.h"
#include "Window.h"
#include "Helpers/RenderingHelper.h"
#include "Helpers/FilesHelper.h"
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
	m_Window = RenderingHelper::CreateWindow({ "EdEngine", 1920, 1080 });
#define CreateWindow CreateWindowW

	std::shared_ptr<AssetManager> assetManager = std::make_shared<AssetManager>();
	m_Managers.push_back(assetManager);

	for (std::shared_ptr<BaseManager>& manager: m_Managers)
	{
		manager->Initialize(this);
	}

	m_Scene = assetManager->LoadScene(FilesHelper::ContentFolderPath + R"(\scenes\main_test.edscene)");
	if (!m_Scene)
	{
		m_Scene = assetManager->CreateScene(FilesHelper::ContentFolderPath + R"(\scenes\main_test.edscene)");
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

	for (std::shared_ptr<BaseManager> manager : m_Managers)
	{
		manager->Update(m_DeltaSeconds);
	}

	PushUpdate(m_DeltaSeconds);
	
	m_Renderer->Update(m_DeltaSeconds);
   
	m_Renderer->BeginUIFrame();
	
	for (std::shared_ptr<Widget>& widget: m_Widgets)
	{
		widget->Tick(m_DeltaSeconds);
	}
	
	m_Renderer->EndUIFrame();

	m_Window->Update();
	m_IsRunning &= m_Window->IsRunning();
}

void Engine::RecieveInputAction(InputKey key, InputAction action)
{
	for (InputEvent& event : m_InputEvents)
	{
		if (Input::KeysMatch(event.Key, key) && Input::ActionsMatch(event.Action, action))
		{
			event.Response(key, action);
		}
	}
}

InputEventHandle Engine::SubscribeToInput(const InputEvent& inputEvent)
{
	m_InputEvents.push_back(inputEvent);
	return inputEvent.Handle;
}

InputEventHandle Engine::SubscribeToInput(InputKey key, InputAction action, std::function<void(InputKey, InputAction)> response)
{
	return SubscribeToInput( { key, action, response });
}

InputEventHandle Engine::SubscribeToInput(std::function<void(InputKey, InputAction)> response)
{
	return SubscribeToInput( { InputKey::AnyKey, InputAction::AnyAction, response });
}

void Engine::UnsubscribeFromInput(InputEventHandle handle)
{
	int32_t index = -1;

	for (uint32_t i = 0; i < m_InputEvents.size(); ++i)
	{
		if (m_InputEvents[i].Handle == handle)
		{
			index = i;
			break;
		}
	}

	if (index != -1)
	{
		m_InputEvents.erase(m_InputEvents.begin() + index);
	}
}

void Engine::SubscribeToUpdate(std::function<void(float)> response)
{
	m_UpdateSubscribers.push_back(response);
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
