#pragma once

enum class InputKey
{
    Num0,
    Num1,
    Num2,
    Num3,
    Num4,
    Num5,
    Num6,
    Num7,
    Num8,
    Num9,

    Numpad0,
    Numpad1,
    Numpad2,
    Numpad3,
    Numpad4,
    Numpad5,
    Numpad6,
    Numpad7,
    Numpad8,
    Numpad9,

    NumpadDecimal,
    NumpadDivide,
    NumpadMultiply,
    NumpadSubtract,
    NumpadAdd,
    NumpadEnter,
    NumpadEqual,

    A,
    B,
    C,
    D,
    E,
    F,
    G,
    H,
    I,
    J,
    K,
    L,
    M,
    N,
    O,
    P,
    Q,
    R,
    S,
    T,
    U,
    V,
    W,
    X,
    Y,
    Z,

    Comma,
    Period,
    Slash,
    BackSlash,

    Minus,
    Equal,

    Space,
    Apostrophe,

    Semicolon,

    LeftBracket,
    RightBracket,

    GraveAccent,

    Escape,             
    Enter,
    Tab,             
    Backspace,
    Insert,         
    Delete,
    Right,
    Left,
    Down,
    Up,
    PageUp,
    PageDown,
    Home,
    End,
    CapsLock,
    ScrollLock,
    NumLock,
    PrintScreen,
    Pause,

    F1,
    F2,
    F3,
    F4,
    F5,
    F6,
    F7,
    F8,
    F9,
    F10,
    F11,
    F12,
    F13,
    F14,
    F15,
    F16,
    F17,
    F18,
    F19,
    F20,
    F21,
    F22,
    F23,
    F24,
    F25,

    LeftShift,
    RightShift,

    LeftControl,
    RightControl,
    
    LeftAlt,
    RightAlt,
    
    LeftSuper,
    RightSuper,

    Menu,

    MouseButton1, // Left
    MouseButton2, // Right
    MouseButton3, // Middle
    MouseButton4,
    MouseButton5,
    MouseButton6,
    MouseButton7,
    MouseButton8, // Last

    JoyStick1,
    JoyStick2,
    JoyStick3,
    JoyStick4,
    JoyStick5,
    JoyStick6,
    JoyStick7,
    JoyStick8,
    JoyStick9,
    JoyStick10,
    JoyStick11,
    JoyStick12,
    JoyStick13,
    JoyStick14,
    JoyStick15,
    JoyStick16,

    GamepadA,
    GamepadB,
    GamepadX,
    GamepadY,
    GamepadLeftBumper,
    GamepadRightBumper,
    GamepadBack,
    GamepadStart,
    GamepadGuide,
    GamepadLeftThumb,
    GamepadRightThumb,
    GamepadDpadAdd,
    GamepadDpadRight,
    GamepadDpadDown,
    GamepadDpadLeft,

    GamepadAxisLeftX,       
    GamepadAxisLeftY,      
    GamepadAxisRightX,      
    GamepadAxisRightY, 
    GamepadAxisLeftTrigger,
    GamepadAxisRightTrigger, 

    AnyKey,
    WrongKey
};

using InputEventHandle = u32;

enum class InputAction : u32
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

struct InputEvent
{
    InputEvent(InputKey key, InputAction action, std::function<void(InputKey, InputAction)> response);

    InputEventHandle Handle;
    InputKey Key;
    InputAction Action;
    std::function<void(InputKey, InputAction)> Response;
};

class InputManager
{
public:
    inline static InputManager& Get()
    {
        ED_ASSERT(s_Manager, "Input manager was not yet initialized.");
        return *s_Manager;
    }

    void Initialize() {}
    void Deinitialize() {}

    void Update(f32 DeltaSeconds);

    void RecieveInputAction(InputKey key, InputAction action);

    InputEventHandle SubscribeToInput(const InputEvent& inputEvent);
    InputEventHandle SubscribeToInput(InputKey key, InputAction action, std::function<void(InputKey, InputAction)> response);
    InputEventHandle SubscribeToInput(std::function<void(InputKey, InputAction)> response);

    void UnsubscribeFromInput(InputEventHandle handle);
protected:
    InputManager();
    ~InputManager();
    friend class Engine;

    bool KeysMatch(InputKey expected, InputKey actual);
    bool ActionsMatch(InputAction expected, InputAction actual);
protected:
    inline static InputManager* s_Manager;
    std::vector<InputEvent> m_InputEvents;
};
