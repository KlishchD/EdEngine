#include "Input.h"
#include "Rendering/EdRendering.h"

Key Input::ConvertGLFWInputKey(int32_t key)
{
    switch (key)
    {
    case GLFW_KEY_W:     return Key::W;
    case GLFW_KEY_S:     return Key::S;
    case GLFW_KEY_A:     return Key::A;
    case GLFW_KEY_D:     return Key::D;
    case GLFW_KEY_E:     return Key::E;
    case GLFW_KEY_Q:     return Key::Q;
    case GLFW_KEY_U:     return Key::U;
    case GLFW_KEY_B:     return Key::B;
    case GLFW_KEY_LEFT:  return Key::LeftArrow;
    case GLFW_KEY_RIGHT: return Key::RightArrow;
    case GLFW_KEY_UP:    return Key::UpArrow;
    case GLFW_KEY_DOWN:  return Key::DownArrow;
    case GLFW_KEY_SPACE: return Key::Space;
    case GLFW_MOUSE_BUTTON_LEFT: return Key::LeftMouseClick;
    case GLFW_MOUSE_BUTTON_RIGHT: return Key::RightMouseClick;
    }
    return Key::WrongKey;
}

Action Input::ConvertGLFWInputAction(int32_t action)
{
    switch (action)
    {
    case GLFW_PRESS:   return Action::Press;
    case GLFW_RELEASE: return Action::Release;
    case GLFW_REPEAT:  return Action::Repeat;
    }
    return Action::WrongAction;
}
