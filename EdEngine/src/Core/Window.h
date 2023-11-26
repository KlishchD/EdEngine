#pragma once

#include "Ed.h"

#include <string>
#include <glm/vec2.hpp>

class Window {
public:
    static Window& Create(const std::string& name, uint32_t width, uint32_t height);
    static Window& Get();
    static void Delete();
    
    virtual void Update();

    bool IsRunning();

    void Close();

    void Resize(int32_t width, int32_t height);

    glm::vec2 GetMousePosition();
    glm::vec2 GetMousePositionNormalized();

    uint32_t GetWidth() const;
    uint32_t GetHeight() const;
    
    GLFWwindow* GetWindow() const;
private:
    GLFWwindow* m_Window;
	std::string m_Name;
	uint32_t m_Width;
	uint32_t m_Height;

    class RendererAPI* m_RenderAPI;

    Window(const std::string& name, uint32_t width, uint32_t height);
};
