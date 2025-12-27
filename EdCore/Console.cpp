#include "EdCore.h"
#include "Console.h"

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
    variable->Type = Types::Unknown;
    return variable;
}

Console::Variable* Console::RegisterVariable(ccstr8 name, u32 size, Types type)
{
    Registry& registry = GetRegisty();

    Variable* variable = registry.Variables.Allocate();
    variable->Name = name;
    variable->Value = Memory::Get().RequestStaticMemory(size, 8, name);
    variable->Size = size;
    variable->Type = type;

    VariableExtension* extension = registry.Extensitons.Allocate();
    extension->Owner = variable;
    extension->Set = nullptr;
    extension->ToString = nullptr;

    ED_ASSERT(extension->Set, "Set function can not be nullptr.");
    ED_ASSERT(extension->ToString, "ToString function can not be nullptr.");

    return variable;
}

Console::Variable* Console::RegisterVariable(ccstr8 name, u32 size, SetFunction set, ToStringFunction toString, Console::Types type)
{
    Registry& registry = GetRegisty();
    
    Variable* variable = registry.Variables.Allocate();
    variable->Name = name;
    variable->Value = Memory::Get().RequestStaticMemory(size, 8, name);
    variable->Size = size;
    variable->Type = type;

    ED_ASSERT(set, "Set function can not be nullptr.");
    ED_ASSERT(toString, "ToString function can not be nullptr.");

    VariableExtension* extension = registry.Extensitons.Allocate();
    extension->Owner = variable;
    extension->Set = set;
    extension->ToString = toString;
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