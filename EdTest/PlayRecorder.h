#pragma once

#include "EdCore.h"

class Window;

struct InputRecord
{
    DurationType Time;
    InputKey Key;
    InputAction Action;

    void Serialize(Archive& archive)
    {
        archive & Time;
        archive & Key;
        archive & Action;
    }
};

struct FrameRecord
{
    DurationType Time;
    glm::vec2 MousePosition;

    void Serialize(Archive& archive)
    {
        archive & Time;
        archive & MousePosition;
    }
};

struct RecordSetup
{
    std::string SceneName;
    glm::vec3 CameraLocation;
    glm::vec3 CameraRotation;

    void Serialize(Archive& archive)
    {
        archive & SceneName;
        archive & CameraLocation;
        archive & CameraRotation;
    }
};

struct StatRecord
{
    DurationType FrameTime;

    u32 StaticMeshesDrawn;

    u32 PointLightsShaded;
    u32 PointLightsShadowsDrawn;

    u32 SpotLightsShaded;
    u32 SpotLightsShadowsDrawn;

    u32 DirectionalLightsShaded;
    u32 DirectionalLightsShadowsDrawn;

    glm::vec3 CameraPosition;
    glm::vec3 CameraRotation;

    void Serialize(Archive& archive)
    {
        archive & FrameTime;

        archive & StaticMeshesDrawn;

        archive & PointLightsShaded;
        archive & PointLightsShadowsDrawn;

        archive & SpotLightsShaded;
        archive & SpotLightsShadowsDrawn;

        archive & DirectionalLightsShaded;
        archive & DirectionalLightsShadowsDrawn;

        archive & CameraPosition;
        archive & CameraRotation;
    }
};

struct RunStats
{
    TimePoint RunTime;

    std::vector<StatRecord> Stats;

    void Serialize(Archive& archive)
    {
        archive & RunTime;

        archive & Stats;
    }
};

struct PlayRecording
{
    std::string Name;
    TimePoint CreationTime;
    DurationType DurationSeconds;
    RecordSetup Setup;
    std::vector<InputRecord> Inputs;
    std::vector<FrameRecord> Frames;
    std::vector<RunStats> Runs;

    void Serialize(Archive& archive)
    {
        archive & Name;
        archive & CreationTime;
        archive & DurationSeconds;
        archive & Setup;
        archive & Inputs;
        archive & Frames;
        archive & Runs;
    }
};

class PlayRecorder : public BaseManager
{
public:
    virtual void Initialize(Engine* engine) override;
    virtual void Update(f32 DeltaSeconds) override;
    virtual void Deinitialize() override;

    void StartRecording(const std::string& name);
    void StopRrecording();

    void RecordInput(InputKey key, InputAction action);
    
    bool IsRecordingActive() const;

    const PlayRecording* GetActivePlayRecording() const;
    const std::vector<PlayRecording> GetRecorings() const;

    bool IsRecordingNameValid(const std::string name) const;

    void ReplayRecording(u32 index);
    void SetPause(bool paused);
    void EndReplay();

    bool IsRecordingReplayActive() const;
protected:
    void LoadAllPlayRecordings();
    void SavePlayRecording(u32 index);

    void CollectRunStats(u32 index);

protected:
    Engine* m_Engine;
    std::shared_ptr<Window> m_Window;

    std::vector<PlayRecording> m_PlayRecordigs;

    InputEventHandle m_InputEventHandle;

    i32 m_CurrentlyRecordedRecording = -1;
    i32 m_CurrentlyReplayedRecording = -1;

    f32 m_ReplayTime = 0.0f;
    bool m_IsReplayPaused = false;

    u32 m_LastReplayedInput = 0;
    u32 m_LastReplayedFrame = 0;

    enum ReplayEventType {
        Input,
        Frame
    };

    struct ReplayEvent {
        ReplayEventType Type;
        union { // Copying here is not the best, but it is acceptable for now
            InputRecord Input;
            FrameRecord Frame;
        };

        DurationType GetTime() const
        {
            switch (Type)
            {
            case ReplayEventType::Input: return Input.Time;
            case ReplayEventType::Frame: return Frame.Time;
            default:
                ED_ASSERT(0, "Unknown replay event type");
            }

            return 0;
        }

        bool operator<(const ReplayEvent& event) const
        {
            return GetTime() < event.GetTime();
        }
    };

#ifdef RUN_PLAY_TESTS_AND_EXIT
    std::vector<std::string> m_AutoPlayRecodings = { PLAY_RECORDINGS_NAMES };
    u32 m_AutoPlayedRecording = 0;
    f32 m_AutoPlayDelayTimer = 3.0f;
#endif
};