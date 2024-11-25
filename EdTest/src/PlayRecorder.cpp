#include "PlayRecorder.h"
#include "Core/Engine.h"
#include "Core/Scene.h"
#include "Core/Objects/PlayerActor.h"
#include "Core/Window.h"
#include "Core/Rendering/Renderer.h"
#include "Helpers/FilesHelper.h"
#include "PlayRecorderControlsWidget.h"

void PlayRecorder::Initialize(Engine* engine)
{
    m_Engine = engine;

    FilesHelper::CreateDirectory(FilesHelper::PlayRecordingsFolderPath);
    LoadAllPlayRecordings();

    engine->AddWidget<PlayRecorderControlsWidget>();
    m_Window = engine->GetWindow();
}

void PlayRecorder::Update(float DeltaSeconds)
{
#if RUN_PLAY_TESTS_AND_EXIT
    if (m_AutoPlayDelayTimer >= 0)
    {
        m_AutoPlayDelayTimer -= DeltaSeconds;
    }
    else if (!IsRecordingReplayActive())
    {
        if (m_AutoPlayedRecording >= m_AutoPlayRecodings.size())
        {
            m_Engine->Stop();
        }
        else
        {
            int32_t index = -1;

            for (int32_t i = 0; i < m_AutoPlayRecodings.size(); ++i)
            {
                const PlayRecording& recording = m_PlayRecordigs[i];
                if (recording.Name == m_AutoPlayRecodings[i])
                {
                    index = i;
                    break;
                }
            }

            if (index != -1)
            {
                ReplayRecording(m_AutoPlayedRecording);
            }

            m_AutoPlayedRecording++;
        }
    }
#endif

    if (IsRecordingActive())
    {
        PlayRecording& recording = m_PlayRecordigs[m_CurrentlyRecordedRecording];
        
        {
            FrameRecord frame;

            frame.Time = DurationAsSeconds(Clock::now() - recording.CreationTime);
            frame.MousePosition = m_Window->GetMousePositionNormalized(); // TODO: think of reworking this as a part of input system

            recording.Frames.push_back(frame);
        }

        CollectRunStats(m_CurrentlyRecordedRecording);
    }

    if (IsRecordingReplayActive() && !m_IsReplayPaused)
    {
        const PlayRecording& recording = m_PlayRecordigs[m_CurrentlyReplayedRecording];
        
        if (m_ReplayTime > recording.DurationSeconds)
        {
            EndReplay();
        }
        else
        {
            std::vector<ReplayEvent> events;
            
            for (uint32_t i = m_LastReplayedInput; i < recording.Inputs.size(); ++i)
            {
                if (recording.Inputs[i].Time < m_ReplayTime)
                {
                    ReplayEvent event;
                    event.Type = ReplayEventType::Input;
                    event.Input = recording.Inputs[i];
                    events.push_back(event);
                }
            }

            for (uint32_t i = m_LastReplayedFrame; i < recording.Frames.size(); ++i)
            {
                if (recording.Frames[i].Time < m_ReplayTime)
                {
                    ReplayEvent event;
                    event.Type = ReplayEventType::Frame;
                    event.Frame = recording.Frames[i];
                    events.push_back(event);
                }
            }

            std::sort(events.begin(), events.end());

            for (const ReplayEvent& event : events)
            {
                switch (event.Type)
                {
                case ReplayEventType::Input: m_Engine->RecieveInputAction(event.Input.Key, event.Input.Action); break;
                case ReplayEventType::Frame: m_Window->SetNormalizedMousePosition(event.Frame.MousePosition); break;
                default:
                    ED_ASSERT(0, "Unknown replay event type");
                }
            }

            m_ReplayTime += DeltaSeconds;

            CollectRunStats(m_CurrentlyReplayedRecording);
        }
    }
}

void PlayRecorder::Deinitialize()
{
    if (IsRecordingActive())
    {
        StopRrecording();
    }
}

void PlayRecorder::StartRecording(const std::string& name)
{
    ED_ASSERT(m_CurrentlyRecordedRecording == -1, "There should be no active recording.");

    m_CurrentlyRecordedRecording = m_PlayRecordigs.size();
   
    PlayRecording recording;
    recording.Name = name;
    recording.CreationTime = std::chrono::system_clock::now();
    
    std::shared_ptr<Scene> scene = m_Engine->GetLoadedScene();
    recording.Setup.SceneName = scene->GetName();
    
    Camera& camera = scene->GetPlayerActor()->GetCameraComponent()->GetCamera();
    recording.Setup.CameraLocation = camera.GetPosition();
    recording.Setup.CameraRotation = camera.GetRotation();

    RunStats run;
    run.RunTime = Clock::now();
    recording.Runs.push_back(run);

    m_PlayRecordigs.push_back(recording);

    m_InputEventHandle = Engine::Get().SubscribeToInput(InputKey::AnyKey, InputAction::AnyAction, [this](InputKey key, InputAction action) { RecordInput(key, action); });
}

void PlayRecorder::StopRrecording()
{
    Engine::Get().UnsubscribeFromInput(m_InputEventHandle);

    PlayRecording& recording = m_PlayRecordigs[m_CurrentlyRecordedRecording];
    recording.DurationSeconds = DurationAsSeconds(Clock::now() - recording.CreationTime);

    SavePlayRecording(m_CurrentlyRecordedRecording);
    m_CurrentlyRecordedRecording = -1;
}

void PlayRecorder::RecordInput(InputKey key, InputAction action)
{
    ED_ASSERT(m_CurrentlyRecordedRecording >= 0 && m_CurrentlyRecordedRecording < m_PlayRecordigs.size(), "No recording is beeing recorded.");

    PlayRecording& recording = m_PlayRecordigs[m_CurrentlyRecordedRecording];

    TimePoint now = Clock::now();
    DurationType deltaSeconds = DurationAsSeconds(now - recording.CreationTime);
    m_PlayRecordigs[m_CurrentlyRecordedRecording].Inputs.emplace_back(deltaSeconds, key, action);
}

bool PlayRecorder::IsRecordingActive() const
{
    return m_CurrentlyRecordedRecording >= 0 && m_CurrentlyRecordedRecording < m_PlayRecordigs.size();
}

const PlayRecording* PlayRecorder::GetActivePlayRecording() const
{
    return m_CurrentlyRecordedRecording >= 0 && m_CurrentlyRecordedRecording < m_PlayRecordigs.size() ? &m_PlayRecordigs[m_CurrentlyRecordedRecording] : nullptr;
}

const std::vector<PlayRecording> PlayRecorder::GetRecorings() const
{
    return m_PlayRecordigs;
}

bool PlayRecorder::IsRecordingNameValid(const std::string name) const
{
    for (const PlayRecording& recording : m_PlayRecordigs)
    {
        if (recording.Name == name)
        {
            return false;
        }
    }

    return true;
}

void PlayRecorder::ReplayRecording(uint32_t index)
{
    ED_ASSERT(m_CurrentlyReplayedRecording == -1, "Can not start replay when another is not finished.");
    ED_ASSERT(m_CurrentlyRecordedRecording == -1, "Can not replay a recording when one is being recorded.");
    ED_ASSERT(index >= 0 && index < m_PlayRecordigs.size(), "Incorrect recording index.");

    m_CurrentlyReplayedRecording = index;
    m_IsReplayPaused = false;
    m_ReplayTime = 0;

    PlayRecording& recording = m_PlayRecordigs[m_CurrentlyReplayedRecording];

    RunStats run;
    run.RunTime = Clock::now();
    recording.Runs.push_back(run);

    Camera& camera = m_Engine->GetLoadedScene()->GetPlayerActor()->GetCameraComponent()->GetCamera();
    camera.SetPosition(recording.Setup.CameraLocation);
    camera.SetRotation(recording.Setup.CameraRotation);

    m_Window->SetMousePositionOverideState(true);
}

void PlayRecorder::SetPause(bool paused)
{
    m_IsReplayPaused = paused;
    m_Window->SetMousePositionOverideState(!paused);
}

void PlayRecorder::EndReplay()
{
    SavePlayRecording(m_CurrentlyReplayedRecording);
 
    m_CurrentlyReplayedRecording = -1;
    m_IsReplayPaused = false;

    m_Window->SetMousePositionOverideState(false);
}

bool PlayRecorder::IsRecordingReplayActive() const
{
    return m_CurrentlyReplayedRecording != -1;
}

void PlayRecorder::SavePlayRecording(uint32_t index)
{
    ED_ASSERT(index >= 0 && index < m_PlayRecordigs.size(), "Attempted to save non existing recording.");
    
    PlayRecording& recording = m_PlayRecordigs[index];
    Archive archive(FilesHelper::MakeRecordingsPath(recording.Name), ArchiveMode::Write);
    archive & recording;
}

void PlayRecorder::CollectRunStats(uint32_t index)
{
    ED_ASSERT(index >= 0 && index < m_PlayRecordigs.size(), "Attempted collect stats for non existing recording.");

    PlayRecording& recording = m_PlayRecordigs[index];

    StatRecord stat;
    stat.FrameTime = m_Engine->GetDeltaSeconds();

    std::shared_ptr<Renderer> renderer = m_Engine->GetRenderer();
    stat.StaticMeshesDrawn = renderer->StaticMeshesDrawn;

    stat.PointLightsShaded = renderer->PointLightsShaded;
    stat.PointLightsShadowsDrawn = renderer->PointLightsShadowsDrawn;

    stat.SpotLightsShaded = renderer->SpotLightsShaded;
    stat.SpotLightsShadowsDrawn = renderer->SpotLightsShadowsDrawn;

    stat.DirectionalLightsShaded = renderer->DirectionalLightsShaded;
    stat.DirectionalLightsShadowsDrawn = renderer->DirectionalLightsShadowsDrawn;

    Camera& camera = m_Engine->GetLoadedScene()->GetPlayerActor()->GetCameraComponent()->GetCamera();
    stat.CameraPosition = camera.GetPosition();
    stat.CameraRotation = camera.GetRotation();

    recording.Runs.back().Stats.push_back(stat);
}

void PlayRecorder::LoadAllPlayRecordings()
{
    std::filesystem::recursive_directory_iterator iterator(FilesHelper::PlayRecordingsFolderPath);
    for (const std::filesystem::directory_entry& entry : iterator)
    {
        if (entry.is_directory()) continue;
        std::string extension = entry.path().extension().string();
        if (FilesHelper::IsPlayRecordingExtension(extension))
        {
            std::string path = entry.path().string();

            PlayRecording recording;
            
            Archive archive(path, ArchiveMode::Read);
            archive & recording;

            m_PlayRecordigs.push_back(recording);
        }
    }
}
