#pragma once

#include <memory>
#include <string>
#include <glm/vec2.hpp>

struct WindowSpecification
{
    std::string Title;
    uint32_t Width;
    uint32_t Height;
};

class Window {
public:
    Window(WindowSpecification specification);

    virtual void Update() = 0;

    virtual bool IsRunning() = 0;

    virtual void Resize(int32_t width, int32_t height) = 0;

    virtual glm::vec2 GetMousePosition() = 0;
    virtual glm::vec2 GetMousePositionNormalized() = 0;

    void SetMousePositionOverideState(bool state);
    void SetNormalizedMousePosition(glm::vec2 position);

    std::string GetTitle() const;
    uint32_t GetWidth() const;
    uint32_t GetHeight() const;
    
    virtual void* GetNativeWindow() = 0;

    virtual std::shared_ptr<class RenderingContext> GetContext() = 0;

    virtual void Close() = 0;
    virtual ~Window() = default;
protected:
	std::string m_Title;
	uint32_t m_Width;
	uint32_t m_Height;

    bool m_MousePositionOverideEnabled = false;
    glm::vec2 m_MousePositionOverride;
};
