#pragma once

class Shader final {
    friend class RenderingContext;
public:
    Shader(ShaderType type, const ShaderPath& filepath);

    ShaderType GetType() const { return m_Type; }

    bool Compile(const Array<ccstr8>& defines, bool debug, cstr8 message);
    bool IsCompiled() const;

    u32 GetBytecodeLength() const;
    const void* GetBytecode() const;

    template <typename T>
    __forceinline T* GetNativeHandle() const
    {
        return (T*)m_NativeHandle;
    }

    __forceinline uptr GetNativeHandle() const { return m_NativeHandle; }

    ~Shader();
protected:
    uptr m_NativeHandle;
    ShaderType m_Type;
    ShaderPath m_Path;
};