#pragma once

#ifdef ED_EDITOR
#include "RenderScene.h"
#endif

class Memory;
class EntityManager;
class AssetManager;
class InputManager;
class Renderer;
class Window;

class Engine
{
public:
    static Engine& Create();
    static Engine& Get();
    static void Delete();

    void Start();
    void Stop();

    void Initialize(u32 argc, c8** argvs);
    void Deinitialize();
    
    bool IsRunning();
    
    void Update();

    void SubscribeToPostUpdate(std::function<void(f32)> response);

    Window* GetWindow() const { return m_Window; }
    Renderer* GetRenderer() const { return m_Renderer; }

    f32 GetDeltaSeconds() const;

    u32 GetFrame() const { return m_Frame; }

#ifdef ED_EDITOR
    void OverrideCameraForNextFrame(const CameraElement& element);
#endif

    const Path& GetResourcesPath() const { return m_ResourcesPath; }
    const Path& GetShadersPath() const { return m_ShadersPath; }

    ~Engine();
protected:
	  void PushPostUpdate(f32 DeltaTime);

    void RenderFrame();
protected:
    Path m_ResourcesPath;
    Path m_ShadersPath;

    EntityManager* m_EntityManager;
    AssetManager* m_AssetManager;
    Window* m_Window;
    Renderer* m_Renderer;
    InputManager* m_InputManager;

    TimePoint m_PreviousFrameTime = Clock::now();
    f32 m_DeltaSeconds;

    std::vector<std::function<void(f32)>> m_PostUpdateSubscribers;

    bool m_IsRunning = true;

    u32 m_Frame;

#ifdef ED_EDITOR
    bool m_IsCameraOverrideActive;
    CameraElement m_CameraOverride;
#endif

    Engine() = default;
};

