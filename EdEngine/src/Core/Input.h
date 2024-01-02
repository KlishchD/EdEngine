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
    B,
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
    static Key ConvertGLFWInputKey(int32_t key);
    static Action ConvertGLFWInputAction(int32_t action);
};

struct InputEvent
{
    Key Key;
    Action Action;
    std::function<void()> Response;
};
