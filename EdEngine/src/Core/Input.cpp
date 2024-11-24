#include "Input.h"
#include "Rendering/EdRendering.h"

InputKey Input::ConvertGLFWInputKey(int32_t key)
{
    switch (key)
    {
    case GLFW_KEY_W:     return InputKey::W;
    case GLFW_KEY_S:     return InputKey::S;
    case GLFW_KEY_A:     return InputKey::A;
    case GLFW_KEY_D:     return InputKey::D;
    case GLFW_KEY_E:     return InputKey::E;
    case GLFW_KEY_Q:     return InputKey::Q;
    case GLFW_KEY_U:     return InputKey::U;
    case GLFW_KEY_B:     return InputKey::B;
    case GLFW_KEY_LEFT:  return InputKey::LeftArrow;
    case GLFW_KEY_RIGHT: return InputKey::RightArrow;
    case GLFW_KEY_UP:    return InputKey::UpArrow;
    case GLFW_KEY_DOWN:  return InputKey::DownArrow;
    case GLFW_KEY_SPACE: return InputKey::Space;
    case GLFW_MOUSE_BUTTON_LEFT: return InputKey::LeftMouseClick;
    case GLFW_MOUSE_BUTTON_RIGHT: return InputKey::RightMouseClick;
    }
    return InputKey::WrongKey;
}

InputAction Input::ConvertGLFWInputAction(int32_t action)
{
    switch (action)
    {
    case GLFW_PRESS:   return InputAction::Press;
    case GLFW_RELEASE: return InputAction::Release;
    case GLFW_REPEAT:  return InputAction::Repeat;
    }
    return InputAction::WrongAction;
}

InputEventHandle Input::GenerateHandle()
{
    static InputEventHandle handle = 0;
    return handle++;
}

bool Input::KeysMatch(InputKey expected, InputKey actual)
{
    return expected == actual || expected == InputKey::AnyKey;
}

bool Input::ActionsMatch(InputAction expected, InputAction actual)
{
    return static_cast<int32_t>(expected) & static_cast<int32_t>(actual);
}
