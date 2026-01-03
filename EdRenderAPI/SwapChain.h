#pragma once

// TODO: Add vsync option.
// TODO: Add fullscreen option.

class Window;
class Resource;

class SwapChain
{
public:
    SwapChain(CommandQueue* queue, Window* window, PixelFormat format, u32 width, u32 height);

    const Array<Resource*>& GetTargets() const { return m_Targets; }

    void Present();

    u32 GetActiveBackBufferIndex() const;

    template <typename T>
    __forceinline T* GetNativeHandle() const
    {
        return (T*)m_NativeHandle;
    }

    __forceinline uptr GetNativeHandle() const { return m_NativeHandle; }

    ~SwapChain();
protected:
    uptr m_NativeHandle;

    Array<Resource*> m_Targets;
};
