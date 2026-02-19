#include "EdEngine.h"
#include "Engine.h"
#include "RenderScene.h"
#include "Helpers/RenderingHelper.h"

Engine* engine = nullptr;

static auto& initial_window_width = console::create_u32("-window_width", 1080, 100, 16384);
static auto& initial_window_height = console::create_u32("-window_height", 720, 100, 16384);

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
  for (auto& object : m_PostUpdateSubscribers)
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

void Engine::Initialize(u32 arguments_count, ccstr8* arguments)
{
  ED_LOG(Engine, info, "Started initializing.");

  using path_parameter = estd::console::path_parameter;

  estd::path resolved_config_path;

  auto& config_path_parameter = console::get_console().add_parameter<path_parameter>("-config_path", &config_path.get())
    .set_help("Set configuration path.")
    .set_mandatory(false)
    .set_file(true);

  auto& resources_path_parameter = console::get_console().add_parameter<path_parameter>("-resources_path", &resources_path.get())
    .set_help("Set resources source directory.")
    .set_mandatory(true)
    .set_directory(true);

  auto& shader_path_parameter = console::get_console().add_parameter<path_parameter>("-shaders_path", &shaders_path.get())
    .set_help("Set shaders source directory.")
    .set_mandatory(true)
    .set_directory(true);

  console::get_console().parse(arguments_count, arguments, "-config_path");

  if (config_path_parameter.was_processed())
  {
    ED_LOG(Engine, info, "-config_path=[{}].", config_path_parameter.get_value().c_str());
    resolved_config_path = config_path_parameter.get_value();
  }
  else
  {
    console::get_console().parse(arguments_count, arguments, "-resources_path");

    if (resources_path_parameter.was_processed())
    {
      ED_LOG(Engine, info, "-resources_path=[{}].", resources_path_parameter.get_value().c_str());
      resolved_config_path = resources_path_parameter.get_value();
      resolved_config_path.append("config.json");
    }
    else
    {
      estd::assert_condition(false, "-config_path or -resoruce_path must be provided to resolve configuration.");
    }
  }

  ED_LOG(Engine, info, "Resolved config path: [{}].", resolved_config_path.c_str());
  config_path = resolved_config_path.c_str();

  console::load(resolved_config_path);

  // Arguments from CLI have higher priority and so ability to overwrite config.
  console::get_console().parse(arguments_count, arguments);

  console::get_console().verify_mandatory();

  m_Frame = 0;

  WindowSpecification specification{};
  specification.Title = "Project the Chronicler";
  specification.Width = initial_window_width();
  specification.Height = initial_window_height();

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
  console::save(config_path);

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
  static Path path = Engine::Get().get_resources_path().c_str();
  return path;
}

const Path& Files::GetShadersPath()
{
  static Path path = Engine::Get().get_shaders_path().c_str();
  return path;
}