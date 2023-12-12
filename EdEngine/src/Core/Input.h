#pragma once

#include "Ed.h"

#include <cstdint>
#include <functional>

enum class Key: uint32_t
{
    W,
    S,
    A,
    D,
    E,
    Q,
    U,
    LeftArrow,
    RightArrow,
    UpArrow,
    DownArrow,
    Space,

    LeftMouseClick,
    RightMouseClick,
    
    WrongKey
};

enum class Action : uint32_t
{
    Press = 1,
    Release = 2,
    Repeat = 4,

    WrongAction
};

class Input
{
public:
    static Key ConvertGLFWInputKey(int32_t key)
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

    static Action ConvertGLFWInputAction(int32_t action)
    {
        switch (action)
        {
        case GLFW_PRESS:   return Action::Press;
        case GLFW_RELEASE: return Action::Release; 
        case GLFW_REPEAT:  return Action::Repeat; 
        }
        return Action::WrongAction;
    }
};

struct InputEvent
{
    Key Key;
    Action Action;
    std::function<void()> Response;
};
