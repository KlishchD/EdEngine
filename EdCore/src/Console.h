#pragma once

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

    struct Variable
    {
        enum Types
        {
            Int,
            UInt,
            Float,
            Double,
            String,
            Unknown
        };

        ccstr8 Name;
        void* Value;
        u32 Size;
    };

    // If no Set or ToString provided one shall be provided based on type if it is possible.
    struct VariableExtension
    {
        Variable* Owner;
        SetFunction Set;
        ToStringFunction ToString;
        Variable::Types Type;
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
    Variable* RegisterVariable(ccstr8 name, u32 size, Variable::Types type);
    Variable* RegisterVariable(ccstr8 name, u32 size, SetFunction set, ToStringFunction toString, Variable::Types type);
    
    Command* FindCommand(ccstr8 name);
    Variable* FindVariable(ccstr8 name);
    VariableExtension* FindExtension(ccstr8 name);
    VariableExtension* FindExtension(Variable* owner);

    SetFunction FindSetFunction(Variable::Types type);
    ToStringFunction FindToStringFunction(Variable::Types type);

    void ExecuteCommand(ccstr8 name, ccstr8 input);
    void SetVariable(ccstr8 name, ccstr8 input);
    void SetVariable(ccstr8 name, const void* value);

    void ExecuteCommand(Command* command, ccstr8 input);
    void SetVariable(Variable* variable, ccstr8 input);
    void SetVariable(Variable* variable, const void* value);

    ccstr8 ToString(Variable* variable);

    template <typename T> requires(std::is_arithmetic_v<T>)
    ccstr8 ToStringTemplated(Variable* variable)
    {
        std::string str = std::to_string(*reinterpret_cast<T*>(variable->Value));
        return Strings::RequestString(str.c_str(), str.size(), true);
    }

    template <typename T> requires(std::is_arithmetic_v<T>)
    void SetTemplated(Variable* variable, ccstr8 str)
    {
        if (Strings::IsNumeric(str))
        {
            T& value = *reinterpret_cast<T*>(variable->Value);
            value = Strings::ParseNumeric<T>(str);
        }
    }

    template <typename T>
    class BasicVariable
    {
    public:
        constexpr static Variable::Types GetType()
        {
            if constexpr (std::is_same_v<i32, T>)
            {
                return Console::Variable::Int;
            }
            if constexpr (std::is_same_v<u32, T>)
            {
                return Console::Variable::UInt;
            }
            if constexpr (std::is_same_v<f32, T>)
            {
                return Console::Variable::Float;
            }
            if constexpr (std::is_same_v<f64, T>)
            {
                return Console::Variable::Double;
            }

            ED_ASSERT(0, "Can not parse vairable type.");

            return Variable::Types::Unknown;
        }

        BasicVariable(ccstr8 name, const T& value)
        {
            m_Variable = Console::RegisterVariable(name, sizeof(T), GetType());
            SetVariable(m_Variable, reinterpret_cast<const void*>(&value));
        }
    protected:
        Variable* m_Variable;
    };
};

#define CONSOLE_VARIABLE(name, type, defaultValue) \
    struct name : Console::BasicVariable<type> \
    { \
        name() : Console::BasicVariable<type>(#name, defaultValue) \
        { \
        } \
        \
        operator type& () \
        { \
            return *reinterpret_cast<type*>(m_Variable->Value); \
        } \
        operator const type& () const \
        { \
            return *reinterpret_cast<type*>(m_Variable->Value); \
        } \
        void operator()(const type& value) \
        { \
            Console::SetVariable(m_Variable, reinterpret_cast<const void*>(&value)); \
        } \
    \
    protected: \
    type* m_Value; \
    \
    } name;
