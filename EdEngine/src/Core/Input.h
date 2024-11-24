#pragma once

#include "Ed.h"

using InputEventHandle = uint32_t;

enum class InputKey: uint32_t
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
 
    AnyKey,
    WrongKey
};

enum class InputAction : uint32_t
{
    Press = 1,
    Release = 2,
    PressAndRelease = 3,
    Repeat = 4,
    PressAndRepeat = 5,
    ReleaseAndRepeat = 6,
    AnyAction = 7,
    WrongAction = 8
};

class Input
{
public:
    static InputKey ConvertGLFWInputKey(int32_t key);
    static InputAction ConvertGLFWInputAction(int32_t action);
    static InputEventHandle GenerateHandle();

    static bool KeysMatch(InputKey expected, InputKey actual);
    static bool ActionsMatch(InputAction expected, InputAction actual);
};

struct InputEvent
{
    InputEvent(InputKey key, InputAction action, std::function<void(InputKey, InputAction)> response) : Handle(Input::GenerateHandle()), Key(key), Action(action), Response(response)
    {
    }

    InputEventHandle Handle;
    InputKey Key;
    InputAction Action;
    std::function<void(InputKey, InputAction)> Response;
};
