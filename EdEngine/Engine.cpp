#include "EdEngine.h"
#include "Engine.h"
#include "RenderScene.h"
#include "Helpers/RenderingHelper.h"

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

void Engine::PushPostUpdate(f32 DeltaTime)
{
	for (auto& object: m_PostUpdateSubscribers)
	{
		object(DeltaTime);
	}
}

void Engine::RenderFrame()
{
	RenderScene renderScene = m_EntityManager->CollectRenderScene();

#ifdef ED_EDITOR
	if (m_IsCameraOverrideActive)
	{
		renderScene.Camera = m_CameraOverride;
		m_IsCameraOverrideActive = false;
	}
#endif

	m_Renderer->RenderFrame(renderScene, m_DeltaSeconds);
}

void Engine::Initialize(u32 argc, ccstr8* argvs)
{
	ED_LOG(Engine, info, "Started initializing.");

	for (u32 index = 0; index < argc; ++index)
	{
		ED_LOG(Engine, info, "CLI entry: [{}].", argvs[index]);

		if (index + 1 < argc)
		{
			if (strcmp(argvs[index], "-resources_path") == 0)
			{
				m_ResourcesPath = argvs[index + 1];
			}
			else if (strcmp(argvs[index], "-shaders_path") == 0)
			{
				m_ShadersPath = argvs[index + 1];
			}
		}
	}

	ED_ASSERT(m_ResourcesPath.IsValid() && m_ResourcesPath.IsDirectory(), "Resoruces path was not provided.");
	ED_ASSERT(m_ShadersPath.IsValid() && m_ShadersPath.IsDirectory(), "Shaders path was not provided.");

	m_Frame = 0;

	WindowSpecification specification{};
  specification.Title = "Project the Chronicler";
  specification.Width = 1280;
  specification.Height = 720;

  m_Window = new Window(specification);

	m_EntityManager = new EntityManager();
	m_EntityManager->Initialize();

	m_AssetManager = new AssetManager();
	m_AssetManager->Initialize();

    m_Renderer = new Renderer();
    m_Renderer->Initialize(m_Window);

	m_InputManager = new InputManager();
	m_InputManager->Initialize();

	ED_LOG(Engine, info, "Finished initializing.")
}

void Engine::Deinitialize()
{
	m_InputManager->Deinitialize();
    delete m_InputManager;
	m_InputManager = nullptr;

	m_Renderer->Deinitialize();
	delete m_Renderer;
	m_Renderer = nullptr;

	m_Window->Close();
	delete m_Window;
	m_Window = nullptr;

	m_AssetManager->Deinitialize();
    delete m_AssetManager;
	m_AssetManager = nullptr;

	m_EntityManager->Deinitialize();
	delete m_EntityManager;
	m_EntityManager = nullptr;
}

bool Engine::IsRunning()
{
	return m_IsRunning;
}

void Engine::Update()
{
	m_Renderer->PreFrameUpdate();

	Memory::Get().OnNewFrame();

	TimePoint now = Clock::now();

	m_DeltaSeconds = DurationAsSeconds(now - m_PreviousFrameTime);

	m_PreviousFrameTime = now;

	m_EntityManager->Update(m_DeltaSeconds);
	m_AssetManager->Update(m_DeltaSeconds);

	RenderFrame();

    PushPostUpdate(m_DeltaSeconds);

	m_Window->Update();
	m_IsRunning &= m_Window->IsRunning();

    m_Frame++;

	m_Renderer->PostFrameUpdate();
}

void Engine::SubscribeToPostUpdate(std::function<void(f32)> response)
{
	m_PostUpdateSubscribers.push_back(response);
}

f32 Engine::GetDeltaSeconds() const
{
	return m_DeltaSeconds;
}

#ifdef ED_EDITOR
void Engine::OverrideCameraForNextFrame(const CameraElement& element)
{
	m_CameraOverride = element;
	m_IsCameraOverrideActive = true;
}
#endif

Engine::~Engine()
{
	if (m_IsRunning)
	{
		Stop();
		Deinitialize();
	}
}

const Path& Files::GetContentPath()
{
  return Engine::Get().GetResourcesPath();
}

const Path& Files::GetShadersPath()
{
  return Engine::Get().GetShadersPath();
}