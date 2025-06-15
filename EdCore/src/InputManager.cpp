#include "EdCore.h"
#include "InputManager.h"
#include "Helpers/InputHelper.h"

void InputManager::Update(f32 DeltaSeconds)
{
    
}

void InputManager::RecieveInputAction(InputKey key, InputAction action)
{
    ED_LOG(Input, info, "Keyboard key {} action {}", static_cast<i32>(key), static_cast<i32>(action));

    for (InputEvent& event : m_InputEvents)
    {
        if (KeysMatch(event.Key, key) && ActionsMatch(event.Action, action))
        {
            event.Response(key, action);
        }
    }
}

InputEventHandle InputManager::SubscribeToInput(const InputEvent& inputEvent)
{
    m_InputEvents.push_back(inputEvent);
    return inputEvent.Handle;
}

InputEventHandle InputManager::SubscribeToInput(InputKey key, InputAction action, std::function<void(InputKey, InputAction)> response)
{
    return SubscribeToInput({ key, action, response });
}

InputEventHandle InputManager::SubscribeToInput(std::function<void(InputKey, InputAction)> response)
{
    return SubscribeToInput({ InputKey::AnyKey, InputAction::AnyAction, response });
}

void InputManager::UnsubscribeFromInput(InputEventHandle handle)
{
    i32 index = -1;

    for (u32 i = 0; i < m_InputEvents.size(); ++i)
    {
        if (m_InputEvents[i].Handle == handle)
        {
            index = i;
            break;
        }
    }

    if (index != -1)
    {
        m_InputEvents.erase(m_InputEvents.begin() + index);
    }
}

InputManager::InputManager()
{
    ED_ASSERT(!s_Manager, "Only one input manager can exist at once");
    s_Manager = this;
}

InputManager::~InputManager()
{
    s_Manager = nullptr;
}

bool InputManager::KeysMatch(InputKey expected, InputKey actual)
{
    return expected == actual || expected == InputKey::AnyKey;
}

bool InputManager::ActionsMatch(InputAction expected, InputAction actual)
{
    return static_cast<i32>(expected) & static_cast<i32>(actual);
}

InputEvent::InputEvent(InputKey key, InputAction action, std::function<void(InputKey, InputAction)> response) : Handle(InputHelper::GenerateHandle()), Key(key), Action(action), Response(response)
{

}
