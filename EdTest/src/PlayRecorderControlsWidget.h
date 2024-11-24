#pragma once

#include "Core/Ed.h"
#include "Core/Widget.h"

class PlayRecorder;
struct StatRecord;

class PlayRecorderControlsWidget : public Widget
{
public:
    virtual void Initialize();
    virtual void Deinitialize();
    virtual void Tick(float DeltaTime);
protected:
    void DisplayReplayStats(const StatRecord& stat);
protected:
    std::shared_ptr<PlayRecorder> m_Recorder;
};