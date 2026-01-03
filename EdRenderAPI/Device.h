#pragma once

struct DeviceMemoryInformation
{
    u64 Budget;
    u64 Usage;
    u64 Reservable;
    u64 Reserved;
};

class Device
{
public:
    Device();

    template <typename T>
    __forceinline T* GetNativeHandle() const
    {
        return (T*)m_NativeHandle;
    }

    __forceinline uptr GetNativeHandle() const { return m_NativeHandle; }

    DeviceMemoryInformation QueryDeviceMemory() const;

#if DEBUG_BUILD || DEVELOPMENT_BUILD
    void LogDeviceMemoryInformation() const;
#endif

    ~Device();
protected:
    uptr m_NativeHandle;
};
