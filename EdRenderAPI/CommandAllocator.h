#pragma once

class CommandAllocator {
public:
    CommandAllocator(CommandListType type);

    void SetDebugName(ccstr8 name);
    void GetDebugName(ccstr8& name);

    void Reset();

    template <typename T>
    __forceinline T* GetNativeHandle() const
    {
        return (T*)m_NativeHandle;
    }

    __forceinline uptr GetNativeHandle() const { return m_NativeHandle; }

    ~CommandAllocator();
public:
    uptr m_NativeHandle;
};
