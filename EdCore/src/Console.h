#pragma once

// Current system is fun but it is not ready because it is difficult to debug.
// Debugging solution implemented or system reinvented.
namespace Console
{
    struct Command;
    struct Variable;

    using ExecuteFunction = void(*)(Command*, ccstr8);
    using SetFunction = void(*)(Variable*, ccstr8);
    using ToStringFunction = ccstr8(*)(Variable*);

    template <typename T>
    void GenericSetFunction(T* value, ccstr8 input)
    {
        ED_ASSERT(0, "GenericSetFunction no implementation provided.");
    }

    struct Command
    {
        ccstr8 Name;
        ExecuteFunction Execute;
    };

    enum class Types
    {
        Int,
        UInt,
        Float,
        Double,
        String,
        Unknown
    };

    struct Variable
    {
        ccstr8 Name;
        void* Value;
        u32 Size;
        Types Type;
    };

    // If no Set or ToString provided one shall be provided based on type if it is possible.
    struct VariableExtension
    {
        Variable* Owner;
        SetFunction Set;
        ToStringFunction ToString;
    };

    struct Registry
    {
        Registry() : Commands("ConsoleCommands"), Variables("ConsoleVariables"), Extensitons("Extension")
        {
        }

        PoolAllocator<Command, 256> Commands;
        PoolAllocator<Variable, 256> Variables;
        PoolAllocator<VariableExtension, 256> Extensitons;
    };

    FreeListAllocator& GetAllocator();
    Registry& GetRegisty();

    Command* RegisterCommand(ccstr8 name, ExecuteFunction execute);
    Variable* RegisterVariable(ccstr8 name, u32 size);
    Variable* RegisterVariable(ccstr8 name, u32 size, Types type);
    Variable* RegisterVariable(ccstr8 name, u32 size, SetFunction set, ToStringFunction toString, Types type);
    
    Command* FindCommand(ccstr8 name);
    Variable* FindVariable(ccstr8 name);
    VariableExtension* FindExtension(ccstr8 name);
    VariableExtension* FindExtension(Variable* owner);

    void ExecuteCommand(ccstr8 name, ccstr8 input);
    void SetVariable(ccstr8 name, ccstr8 input);
    void SetVariable(ccstr8 name, const void* value);

    void ExecuteCommand(Command* command, ccstr8 input);
    void SetVariable(Variable* variable, ccstr8 input);
    void SetVariable(Variable* variable, const void* value);

    template <typename T>
    constexpr Types GetType()
    {
        if constexpr (std::is_same_v<i32, T>)
        {
            return Types::Int;
        }
        if constexpr (std::is_same_v<u32, T>)
        {
            return Types::UInt;
        }
        if constexpr (std::is_same_v<f32, T>)
        {
            return Types::Float;
        }
        if constexpr (std::is_same_v<f64, T>)
        {
            return Types::Double;
        }

        ED_ASSERT(0, "Can not parse vairable type.");

        return Types::Unknown;
    }

    template <typename T>
    struct ArithmeticVariableValue
    {
        T Value;
        T Min;
        T Max;
    };

    template <typename T> requires(std::is_arithmetic_v<T>)
    Variable* RegisterVariable(ccstr8 name, T default, T min, T max)
    {
        constexpr u32 size = sizeof(T) * 3;
        constexpr Types type = GetType<T>();

        Variable* variable = RegisterVariable(name, size, SetValue<T>, ToString<T>, type);
        
        ArithmeticVariableValue<T>* destination = reinterpret_cast<ArithmeticVariableValue<T>*>(variable->Value);
        destination->Value = default;
        destination->Min = min;
        destination->Max = max;

        return variable;
    }
    
    template <typename T> requires(std::is_arithmetic_v<T>)
    void SetValue(Variable* variable, T value)
    {
        constexpr Types type = GetType<T>();
        
        ED_ASSERT(type == variable->Type, "Can not set value of one type to a variable of different type.");

        ArithmeticVariableValue<T>* destination = reinterpret_cast<ArithmeticVariableValue<T>*>(variable->Value);
        destination->Value = value;
    }
    
    template <typename T> requires(std::is_arithmetic_v<T>)
    void SetValue(Variable* variable, ccstr8 input)
    {
        constexpr Types type = GetType<T>();
        
        ED_ASSERT(type == variable->Type, "Can not set value of one type to a variable of different type.");

        ArithmeticVariableValue<T>* destination = reinterpret_cast<ArithmeticVariableValue<T>*>(variable->Value);
        destination->Value = Strings::ParseNumeric<T>(input);
    }

    template <typename T> requires(std::is_arithmetic_v<T>)
    void SetMin(Variable* variable, T min)
    {
        constexpr Types type = GetType<T>();

        ED_ASSERT(type == variable->Type, "Can not set min of one type to a variable of different type.");

        ArithmeticVariableValue<T>* destination = reinterpret_cast<ArithmeticVariableValue<T>*>(variable->Value);
        destination->Min = min;
    }

    template <typename T> requires(std::is_arithmetic_v<T>)
    void SetMax(Variable* variable, T max)
    {
        constexpr Types type = GetType<T>();

        ED_ASSERT(type == variable->Type, "Can not set max of one type to a variable of different type.");

        ArithmeticVariableValue<T>* destination = reinterpret_cast<ArithmeticVariableValue<T>*>(variable->Value);
        destination->Max = max;
    }

    template <typename T> requires(std::is_arithmetic_v<T>)
    T GetValue(Variable* variable)
    {
        constexpr Types type = GetType<T>();

        ED_ASSERT(type == variable->Type, "Can not get value of one type to a variable of different type.");

        ArithmeticVariableValue<T>* source = reinterpret_cast<ArithmeticVariableValue<T>*>(variable->Value);
        return source->Value;
    }
    
    template <typename T> requires(std::is_arithmetic_v<T>)
    T GetMin(Variable* variable)
    {
        constexpr Types type = GetType<T>();

        ED_ASSERT(type == variable->Type, "Can not get min of one type to a variable of different type.");

        ArithmeticVariableValue<T>* source = reinterpret_cast<ArithmeticVariableValue<T>*>(variable->Value);
        return source->Min;
    }
    
    template <typename T> requires(std::is_arithmetic_v<T>)
    T GetMax(Variable* variable)
    {
        constexpr Types type = GetType<T>();

        ED_ASSERT(type == variable->Type, "Can not get max of one type to a variable of different type.");

        ArithmeticVariableValue<T>* source = reinterpret_cast<ArithmeticVariableValue<T>*>(variable->Value);
        return source->Max;
    }

    template <typename T> requires(std::is_arithmetic_v<T>)
    ccstr8 ToString(Variable* variable)
    {
        ArithmeticVariableValue<T>* destination = reinterpret_cast<ArithmeticVariableValue<T>*>(variable->Value);
        std::string str = std::to_string(destination->Value);
        return Strings::RequestString(str.c_str(), str.size(), true);
    }

    ccstr8 ToString(Variable* variable);
};

#define DEFINE_ARIPHMETIC_VARIABLE(name, default, min, max) inline Console::Variable* name = Console::RegisterVariable<decltype(min)>(#name, default, min, max);
