#include "EdRenderApiPrivate.h"
#include "Window.h"

glm::f64vec2 m_MouseDragPreviousPosition;
i32 m_DragState = 0;

Window* m_Window = nullptr;
GLFWwindow* m_NativeWindow = nullptr;

InputKey ConvertKeyCodeToWindowsCodes(u32 GLFWKeyCode, i32 GLFWMods)
{
	switch (GLFWKeyCode)
	{
	case GLFW_KEY_SPACE:                    return InputKey::Space;
	case GLFW_KEY_APOSTROPHE:               return InputKey::Apostrophe;
	case GLFW_KEY_COMMA:                    return InputKey::Comma;
	case GLFW_KEY_MINUS:                    return InputKey::Minus;
	case GLFW_KEY_PERIOD:                   return InputKey::Period;
	case GLFW_KEY_SLASH:                    return InputKey::Slash;
	case GLFW_KEY_0:                        return InputKey::Num0;
	case GLFW_KEY_1:                        return InputKey::Num1;
	case GLFW_KEY_2:                        return InputKey::Num2;
	case GLFW_KEY_3:                        return InputKey::Num3;
	case GLFW_KEY_4:                        return InputKey::Num4;
	case GLFW_KEY_5:                        return InputKey::Num5;
	case GLFW_KEY_6:                        return InputKey::Num6;
	case GLFW_KEY_7:                        return InputKey::Num7;
	case GLFW_KEY_8:                        return InputKey::Num8;
	case GLFW_KEY_9:                        return InputKey::Num9;
	case GLFW_KEY_SEMICOLON:	            return InputKey::Semicolon;
	case GLFW_KEY_EQUAL:                    return InputKey::Equal;
	case GLFW_KEY_A:			            return InputKey::A;
	case GLFW_KEY_B:			            return InputKey::B;
	case GLFW_KEY_C:			            return InputKey::C;
	case GLFW_KEY_D:			            return InputKey::D;
	case GLFW_KEY_E:			            return InputKey::E;
	case GLFW_KEY_F:			            return InputKey::F;
	case GLFW_KEY_G:			            return InputKey::G;
	case GLFW_KEY_H:			            return InputKey::H;
	case GLFW_KEY_I:			            return InputKey::I;
	case GLFW_KEY_J:			            return InputKey::J;
	case GLFW_KEY_K:			            return InputKey::K;
	case GLFW_KEY_L:			            return InputKey::L;
	case GLFW_KEY_M:			            return InputKey::M;
	case GLFW_KEY_N:			            return InputKey::N;
	case GLFW_KEY_O:			            return InputKey::O;
	case GLFW_KEY_P:			            return InputKey::P;
	case GLFW_KEY_Q:			            return InputKey::Q;
	case GLFW_KEY_R:			            return InputKey::R;
	case GLFW_KEY_S:			            return InputKey::S;
	case GLFW_KEY_T:			            return InputKey::T;
	case GLFW_KEY_U:			            return InputKey::U;
	case GLFW_KEY_V:			            return InputKey::V;
	case GLFW_KEY_W:			            return InputKey::W;
	case GLFW_KEY_X:			            return InputKey::X;
	case GLFW_KEY_Y:			            return InputKey::Y;
	case GLFW_KEY_Z:			            return InputKey::Z;
	case GLFW_KEY_LEFT_BRACKET:             return InputKey::LeftBracket;
	case GLFW_KEY_BACKSLASH:                return InputKey::BackSlash;
	case GLFW_KEY_RIGHT_BRACKET:            return InputKey::RightBracket;
	case GLFW_KEY_GRAVE_ACCENT:             return InputKey::GraveAccent;
	case GLFW_KEY_ESCAPE:		            return InputKey::Escape;
	case GLFW_KEY_ENTER:      	            return InputKey::Enter;
	case GLFW_KEY_TAB:       	            return InputKey::Tab;
	case GLFW_KEY_BACKSPACE:	            return InputKey::Backspace;
	case GLFW_KEY_INSERT:   	            return InputKey::Insert;
	case GLFW_KEY_DELETE:      	            return InputKey::Delete;
	case GLFW_KEY_RIGHT:      	            return InputKey::Right;
	case GLFW_KEY_LEFT:       	            return InputKey::Left;
	case GLFW_KEY_DOWN:        	            return InputKey::Down;
	case GLFW_KEY_UP:        	            return InputKey::Up;
	case GLFW_KEY_PAGE_UP:		            return InputKey::PageUp;
	case GLFW_KEY_PAGE_DOWN:	            return InputKey::PageDown;
	case GLFW_KEY_HOME:			            return InputKey::Home;
	case GLFW_KEY_END:			            return InputKey::End;
	case GLFW_KEY_CAPS_LOCK:	            return InputKey::CapsLock;
	case GLFW_KEY_SCROLL_LOCK:	            return InputKey::ScrollLock;
	case GLFW_KEY_NUM_LOCK: 	            return InputKey::NumLock;
	case GLFW_KEY_PRINT_SCREEN:	            return InputKey::PrintScreen;
	case GLFW_KEY_PAUSE:		            return InputKey::Pause;
	case GLFW_KEY_F1:			            return InputKey::F1;
	case GLFW_KEY_F2:			            return InputKey::F2;
	case GLFW_KEY_F3:			            return InputKey::F3;
	case GLFW_KEY_F4:			            return InputKey::F4;
	case GLFW_KEY_F5:			            return InputKey::F5;
	case GLFW_KEY_F6:			            return InputKey::F6;
	case GLFW_KEY_F7:			            return InputKey::F7;
	case GLFW_KEY_F8:			            return InputKey::F8;
	case GLFW_KEY_F9:			            return InputKey::F9;
	case GLFW_KEY_F10:			            return InputKey::F10;
	case GLFW_KEY_F11:			            return InputKey::F11;
	case GLFW_KEY_F12:			            return InputKey::F12;
	case GLFW_KEY_F13:			            return InputKey::F13;
	case GLFW_KEY_F14:			            return InputKey::F14;
	case GLFW_KEY_F15:			            return InputKey::F15;
	case GLFW_KEY_F16:			            return InputKey::F16;
	case GLFW_KEY_F17:			            return InputKey::F17;
	case GLFW_KEY_F18:			            return InputKey::F18;
	case GLFW_KEY_F19:			            return InputKey::F19;
	case GLFW_KEY_F20:			            return InputKey::F20;
	case GLFW_KEY_F21:			            return InputKey::F21;
	case GLFW_KEY_F22:			            return InputKey::F22;
	case GLFW_KEY_F23:			            return InputKey::F23;
	case GLFW_KEY_F24:			            return InputKey::F24;
	case GLFW_KEY_F25:			            return InputKey::F25;
	case GLFW_KEY_KP_0:			            return InputKey::Numpad0;
	case GLFW_KEY_KP_1:			            return InputKey::Numpad1;
	case GLFW_KEY_KP_2:			            return InputKey::Numpad2;
	case GLFW_KEY_KP_3:			            return InputKey::Numpad3;
	case GLFW_KEY_KP_4:			            return InputKey::Numpad4;
	case GLFW_KEY_KP_5:			            return InputKey::Numpad5;
	case GLFW_KEY_KP_6:			            return InputKey::Numpad6;
	case GLFW_KEY_KP_7:			            return InputKey::Numpad7;
	case GLFW_KEY_KP_8:			            return InputKey::Numpad8;
	case GLFW_KEY_KP_9:			            return InputKey::Numpad9;
	case GLFW_KEY_KP_DECIMAL:	            return InputKey::NumpadDecimal;
	case GLFW_KEY_KP_DIVIDE:  	            return InputKey::NumpadDivide;
	case GLFW_KEY_KP_MULTIPLY:	            return InputKey::NumpadMultiply;
	case GLFW_KEY_KP_SUBTRACT:	            return InputKey::NumpadSubtract;
	case GLFW_KEY_KP_ADD: 		            return InputKey::NumpadAdd;
	case GLFW_KEY_KP_ENTER:		            return InputKey::NumpadEnter;
	case GLFW_KEY_KP_EQUAL:		            return InputKey::NumpadEqual;
	case GLFW_KEY_LEFT_SHIFT:	            return InputKey::LeftShift;
	case GLFW_KEY_LEFT_CONTROL:	            return InputKey::LeftControl;
	case GLFW_KEY_LEFT_ALT:		            return InputKey::LeftAlt;
	case GLFW_KEY_LEFT_SUPER:	            return InputKey::LeftSuper;
	case GLFW_KEY_RIGHT_SHIFT:	            return InputKey::RightShift;
	case GLFW_KEY_RIGHT_CONTROL:            return InputKey::RightControl;
	case GLFW_KEY_RIGHT_ALT:	            return InputKey::RightAlt;
	case GLFW_KEY_RIGHT_SUPER:	            return InputKey::RightSuper;
	case GLFW_KEY_MENU:			            return InputKey::Menu;

	case GLFW_MOUSE_BUTTON_1:               return InputKey::MouseButton1;
	case GLFW_MOUSE_BUTTON_2:               return InputKey::MouseButton2;
	case GLFW_MOUSE_BUTTON_3:               return InputKey::MouseButton3;
	case GLFW_MOUSE_BUTTON_4:               return InputKey::MouseButton4;
	case GLFW_MOUSE_BUTTON_5:               return InputKey::MouseButton5;
	case GLFW_MOUSE_BUTTON_6:               return InputKey::MouseButton6;
	case GLFW_MOUSE_BUTTON_7:               return InputKey::MouseButton7;
	case GLFW_MOUSE_BUTTON_8:               return InputKey::MouseButton8;

		/* TODO: These need a separate handling and investigation, so putting this off for time being.
		case GLFW_JOYSTICK_1:                   return InputKey::JoyStick1;
		case GLFW_JOYSTICK_2:                   return InputKey::JoyStick2;
		case GLFW_JOYSTICK_3:                   return InputKey::JoyStick3;
		case GLFW_JOYSTICK_4:                   return InputKey::JoyStick4;
		case GLFW_JOYSTICK_5:                   return InputKey::JoyStick5;
		case GLFW_JOYSTICK_6:                   return InputKey::JoyStick6;
		case GLFW_JOYSTICK_7:                   return InputKey::JoyStick7;
		case GLFW_JOYSTICK_8:                   return InputKey::JoyStick8;
		case GLFW_JOYSTICK_9:                   return InputKey::JoyStick9;
		case GLFW_JOYSTICK_10:                  return InputKey::JoyStick10;
		case GLFW_JOYSTICK_11:                  return InputKey::JoyStick11;
		case GLFW_JOYSTICK_12:                  return InputKey::JoyStick12;
		case GLFW_JOYSTICK_13:                  return InputKey::JoyStick13;
		case GLFW_JOYSTICK_14:                  return InputKey::JoyStick14;
		case GLFW_JOYSTICK_15:                  return InputKey::JoyStick15;
		case GLFW_JOYSTICK_16:                  return InputKey::JoyStick16;

		case GLFW_GAMEPAD_BUTTON_A:				return InputKey::GamepadA;
		case GLFW_GAMEPAD_BUTTON_B:				return InputKey::GamepadB;
		case GLFW_GAMEPAD_BUTTON_X:				return InputKey::GamepadX;
		case GLFW_GAMEPAD_BUTTON_Y:				return InputKey::GamepadY;
		case GLFW_GAMEPAD_BUTTON_LEFT_BUMPER:	return InputKey::GamepadLeftBumper;
		case GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER:	return InputKey::GamepadRightBumper;
		case GLFW_GAMEPAD_BUTTON_BACK:			return InputKey::GamepadBack;
		case GLFW_GAMEPAD_BUTTON_START:			return InputKey::GamepadStart;
		case GLFW_GAMEPAD_BUTTON_GUIDE:			return InputKey::GamepadGuide;
		case GLFW_GAMEPAD_BUTTON_LEFT_THUMB:	return InputKey::GamepadLeftThumb;
		case GLFW_GAMEPAD_BUTTON_RIGHT_THUMB:	return InputKey::GamepadRightThumb;
		case GLFW_GAMEPAD_BUTTON_DPAD_UP:		return InputKey::GamepadDpadAdd;
		case GLFW_GAMEPAD_BUTTON_DPAD_RIGHT:	return InputKey::GamepadDpadRight;
		case GLFW_GAMEPAD_BUTTON_DPAD_DOWN:		return InputKey::GamepadDpadDown;
		case GLFW_GAMEPAD_BUTTON_DPAD_LEFT:     return InputKey::GamepadDpadLeft;

		case GLFW_GAMEPAD_AXIS_LEFT_X:          return InputKey::GamepadAxisLeftX;
		case GLFW_GAMEPAD_AXIS_LEFT_Y:          return InputKey::GamepadAxisLeftY;
		case GLFW_GAMEPAD_AXIS_RIGHT_X:         return InputKey::GamepadAxisRightX;
		case GLFW_GAMEPAD_AXIS_RIGHT_Y:         return InputKey::GamepadAxisRightY;
		case GLFW_GAMEPAD_AXIS_LEFT_TRIGGER:    return InputKey::GamepadAxisLeftTrigger;
		case GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER:   return InputKey::GamepadAxisRightTrigger;
		*/
	}

	return InputKey::WrongKey;
}

InputAction ConvertGLFWInputAction(i32 action)
{
	switch (action)
	{
	case GLFW_PRESS:   return InputAction::Press;
	case GLFW_RELEASE: return InputAction::Release;
	case GLFW_REPEAT:  return InputAction::Repeat;
		// TODO: Joystick
	}
	return InputAction::WrongAction;
}

Window::Window(WindowSpecification specification) : m_Width(specification.Width), m_Height(specification.Height), m_Title(specification.Title)
{
	ED_LOG(Window, info, "Creating window");

	m_Window = this;

	ED_ASSERT(glfwInit() == GLFW_TRUE, "Failed to initialize GLFW");

	glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	m_NativeWindow = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);

	if (!m_NativeWindow)
	{
		glfwTerminate();
		ED_ASSERT(0, "Failed to initialize GLFW")
	}

	glfwMakeContextCurrent(m_NativeWindow);

	glfwSetWindowSizeCallback(m_NativeWindow, [](GLFWwindow* inWindow, i32 width, i32 height)
		{
			m_Window->Resize(width, height);
		});

	glfwSetKeyCallback(m_NativeWindow, [](GLFWwindow* inWindow, i32 key, i32 scancode, i32 action, i32 mods)
		{
			InputManager::Get().RecieveInputAction(ConvertKeyCodeToWindowsCodes(key, mods), ConvertGLFWInputAction(action));
		});

	glfwSetMouseButtonCallback(m_NativeWindow, [](GLFWwindow* inWindow, i32 button, i32 action, i32 mods)
		{
			InputManager::Get().RecieveInputAction(ConvertKeyCodeToWindowsCodes(button, mods), ConvertGLFWInputAction(action));
		});
}

void Window::Update()
{
	if (glfwGetMouseButton(m_NativeWindow, 0) == GLFW_PRESS && (m_DraggingEnabled || m_DragState))
	{
		if (m_DragState)
		{
			glm::f64vec2 mousePosition;
			glfwGetCursorPos(m_NativeWindow, &mousePosition.x, &mousePosition.y);

			glm::i32vec2 windowPosition;
			glfwGetWindowPos(m_NativeWindow, &windowPosition.x, &windowPosition.y);

			glfwSetWindowPos(m_NativeWindow, windowPosition.x + static_cast<i32>(mousePosition.x - m_MouseDragPreviousPosition.x), windowPosition.y + static_cast<i32>(mousePosition.y - m_MouseDragPreviousPosition.y));
		}
		else
		{
			glfwGetCursorPos(m_NativeWindow, &m_MouseDragPreviousPosition.x, &m_MouseDragPreviousPosition.y);
			m_DragState = true;
		}
	}
	else
	{
		m_DragState = false;
	}

	glfwPollEvents();
}

bool Window::IsRunning() const
{
	return !glfwWindowShouldClose(m_NativeWindow);
}

void Window::Resize(i32 width, i32 height)
{
	ED_ASSERT(0, "Window resize is not supported.");
}

glm::vec2 Window::GetMousePositionNormalized() const
{
    if (m_MousePositionOverideEnabled)
    {
        return m_MousePositionOverride;
    }

    glm::dvec2 position;
    glfwGetCursorPos(m_NativeWindow, &position.x, &position.y);
    return { position.x / m_Width, position.y / m_Height };
}

uptr Window::GetPlatformNativeWindow() const
{
    return (uptr)glfwGetWin32Window(m_NativeWindow);
}

uptr Window::GetNativeWindow() const
{
    return reinterpret_cast<uptr>(m_NativeWindow);
}

void Window::Close()
{
    if (m_NativeWindow)
    {
        glfwTerminate();

        m_NativeWindow = nullptr;

        ED_LOG(Window, info, "Window is closed")
    }
}

Window::~Window()
{
    Close();
}

glm::vec2 Window::GetMousePosition() const
{
    return GetMousePositionNormalized() * glm::vec2(m_Width, m_Height);
}

void Window::SetMousePositionOverideState(bool state)
{
    m_MousePositionOverideEnabled = state;
}

void Window::SetNormalizedMousePosition(glm::vec2 position)
{
    m_MousePositionOverride = position;
}

std::string Window::GetTitle() const
{
    return m_Title;
}

u32 Window::GetWidth() const
{
    return m_Width;
}

u32 Window::GetHeight() const
{
    return m_Height;
}