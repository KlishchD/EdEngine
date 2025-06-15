#include "EdCore.h"
#include "Console.h"

template <>
void Console::GenericSetFunction<u32>(u32* value, ccstr8 input)
{
    reinterpret_cast<u32&>(*value) = atoi(input);
}

template <>
void Console::GenericSetFunction<i32>(i32* value, ccstr8 input)
{
    reinterpret_cast<i32&>(*value) = atoi(input);
}

FreeListAllocator& Console::GetAllocator()
{
    static FreeListAllocator allocator = FreeListAllocator(Memory::BytesToMB, "Console");
    return allocator;
}

Console::Registry& Console::GetRegisty()
{
    static Registry registry;
    return registry;
}

Console::Command* Console::RegisterCommand(ccstr8 name, ExecuteFunction execute)
{
    Registry& registry = GetRegisty();
    Command* command = registry.Commands.Allocate();
    command->Name = name;
    command->Execute = execute;
    return command;
}

Console::Variable* Console::RegisterVariable(ccstr8 name, u32 size)
{
    Registry& registry = GetRegisty();
    Variable* variable = registry.Variables.Allocate();
    variable->Name = name;
    variable->Value = Memory::Get().RequestStaticMemory(size, 8, name);
    variable->Size = size;
    return variable;
}

Console::Variable* Console::RegisterVariable(ccstr8 name, u32 size, Variable::Types type)
{
    Registry& registry = GetRegisty();

    Variable* variable = registry.Variables.Allocate();
    variable->Name = name;
    variable->Value = Memory::Get().RequestStaticMemory(size, 8, name);
    variable->Size = size;

    VariableExtension* extension = registry.Extensitons.Allocate();
    extension->Owner = variable;
    extension->Set = FindSetFunction(type);
    extension->ToString = FindToStringFunction(type);
    extension->Type = type;

    ED_ASSERT(extension->Set, "Set function can not be nullptr.");
    ED_ASSERT(extension->ToString, "ToString function can not be nullptr.");

    return variable;
}

Console::Variable* Console::RegisterVariable(ccstr8 name, u32 size, SetFunction set, ToStringFunction toString, Console::Variable::Types type)
{
    Registry& registry = GetRegisty();
    
    Variable* variable = registry.Variables.Allocate();
    variable->Name = name;
    variable->Value = Memory::Get().RequestStaticMemory(size, 8, name);
    variable->Size = size;

    if (!set)
    {
        set = FindSetFunction(type);
    }

    if (!toString)
    {
        toString = FindToStringFunction(type);
    }

    ED_ASSERT(set, "Set function can not be nullptr.");
    ED_ASSERT(toString, "ToString function can not be nullptr.");

    VariableExtension* extension = registry.Extensitons.Allocate();
    extension->Owner = variable;
    extension->Set = set;
    extension->ToString = toString;
    extension->Type = type;
    return variable;
}

Console::Command* Console::FindCommand(ccstr8 name)
{
    Registry& registry = GetRegisty();

    for (Command* current : registry.Commands)
    {
        if (Strings::Equal(name, current->Name))
        {
            return current;
        }
    }

    return nullptr;
}

Console::Variable* Console::FindVariable(ccstr8 name)
{
    Registry& registry = GetRegisty();

    for (Variable* current : registry.Variables)
    {
        if (Strings::Equal(name, current->Name))
        {
            return current;
        }
    }

    return nullptr;
}

Console::VariableExtension* Console::FindExtension(ccstr8 name)
{
    Registry& registry = GetRegisty();

    for (VariableExtension* current : registry.Extensitons)
    {
        if (Strings::Equal(name, current->Owner->Name))
        {
            return current;
        }
    }

    return nullptr;
}

Console::VariableExtension* Console::FindExtension(Variable* owner)
{
    Registry& registry = GetRegisty();

    for (VariableExtension* current : registry.Extensitons)
    {
        if (current->Owner == owner)
        {
            return current;
        }
    }

    return nullptr;
}


Console::SetFunction Console::FindSetFunction(Variable::Types type)
{
    switch (type)
    {
    case Console::Variable::Int: return &Console::SetTemplated<i32>;
    case Console::Variable::UInt: return &Console::SetTemplated<u32>;
    case Console::Variable::Float: return &Console::SetTemplated<f32>;
    case Console::Variable::Double: return &Console::SetTemplated<f64>;
    case Console::Variable::String: return nullptr;
    case Console::Variable::Unknown:
    default:
        ED_LOG(Console, err, "No common setter was found");
        break;
    }

    return nullptr;
}

Console::ToStringFunction Console::FindToStringFunction(Variable::Types type)
{
    switch (type)
    {
    case Console::Variable::Int: return &Console::ToStringTemplated<i32>;
    case Console::Variable::UInt: return &Console::ToStringTemplated<u32>;
    case Console::Variable::Float: return &Console::ToStringTemplated<f32>;
    case Console::Variable::Double: return &Console::ToStringTemplated<f64>;
    case Console::Variable::String: return nullptr;
    case Console::Variable::Unknown:
    default:
        ED_LOG(Console, err, "No common setter was found");
        break;
    }

    return nullptr;
}

void Console::ExecuteCommand(ccstr8 name, ccstr8 input)
{
    if (Command* command = FindCommand(name))
    {
        ExecuteCommand(command, input);
    }
}

void Console::SetVariable(ccstr8 name, ccstr8 input)
{
    if (Variable* variable = FindVariable(name))
    {
        SetVariable(variable, input);
    }
}

void Console::SetVariable(ccstr8 name, const void* value)
{
    if (Variable* variable = FindVariable(name))
    {
        SetVariable(variable, value);
    }
}

void Console::ExecuteCommand(Command* command, ccstr8 input)
{
    ED_ASSERT(command, "Can not exectue nullptr command.");
    command->Execute(command, input);
}

void Console::SetVariable(Variable* variable, const void* value)
{
    ED_ASSERT(variable, "Can not set nullptr variable");

    // This is dangerous part of way of doing it, user must ensure that sizes are proper.
    memcpy(variable->Value, variable, variable->Size);
}

void Console::SetVariable(Variable* variable, ccstr8 input)
{
    ED_ASSERT(variable, "Can not set nullptr variable");

    if (VariableExtension* extension = FindExtension(variable))
    {
        extension->Set(variable, input);
        return;
    }

    ED_LOG(Console, err, "Variable doesn't have extensions to work with strings.");
}

ccstr8 Console::ToString(Variable* variable)
{
    if (VariableExtension* extension = FindExtension(variable))
    {
        return extension->ToString(variable);
    }

    ED_LOG(Console, err, "Was not able to find extension for variable.");

    return nullptr;
}