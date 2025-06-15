#pragma once

#include "EdCore.h"
#include "Widget.h"

class PlayRecorder;
struct StatRecord;

class PlayRecorderControlsWidget : public Widget
{
public:
    virtual void Initialize();
    virtual void Deinitialize();
    virtual void Tick(f32 DeltaTime);
protected:
    void DisplayReplayStats(const StatRecord& stat);
protected:
    std::shared_ptr<PlayRecorder> m_Recorder;
};