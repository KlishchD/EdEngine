#pragma once

class Editor;
enum class AAMethod : u8;

class CameraDetailsWidget: public Widget
{
    inline static i32 InstancesCount = 0;
public:
    CameraDetailsWidget();
    
    virtual void Tick(f32 DeltaTime) override;

private:
    bool PositionSliders(glm::vec3& position, glm::vec2 range);
    bool RotationSliders(glm::vec3& angles);

    static std::string ConvertAAMethodToString(AAMethod method);

private:
    Array<f32> m_Times;
};
