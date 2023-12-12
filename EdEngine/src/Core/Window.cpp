#include "Window.h"

Window::Window(WindowSpecification specification): m_Width(specification.Width), m_Height(specification.Height), m_Title(specification.Title)
{
}

std::string Window::GetTitle() const
{
    return m_Title;
}

uint32_t Window::GetWidth() const
{
    return m_Width;
}

uint32_t Window::GetHeight() const
{
    return m_Height;
}
