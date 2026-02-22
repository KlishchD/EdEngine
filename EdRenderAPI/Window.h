#pragma once
 
class RenderingContext;

struct WindowSpecification
{
    std::string Title;
    ContentPath Icon;
    u32 Width;
    u32 Height;
};

class Window {
public:
    Window(WindowSpecification specification);

    void Update();
    bool IsRunning() const;

    void Resize(i32 width, i32 height);

    glm::vec2 GetMousePosition() const;
    glm::vec2 GetMousePositionNormalized() const;

    void SetMousePositionOverideState(bool state);
    void SetNormalizedMousePosition(glm::vec2 position);

    inline void SetDragging(bool state) { m_DraggingEnabled = state; }
    inline bool IsDraggingEnabled() const { return m_DraggingEnabled; }

    std::string GetTitle() const;
    glm::u32vec2 GetSize() const;
    u32 GetWidth() const;
    u32 GetHeight() const;
    
    uptr GetPlatformNativeWindow() const;
    uptr GetNativeWindow() const;

    void Close();
    ~Window();
protected:
	std::string m_Title;
	u32 m_Width;
	u32 m_Height;

    bool m_MousePositionOverideEnabled = false;
    glm::vec2 m_MousePositionOverride;

    bool m_DraggingEnabled = false;
};
