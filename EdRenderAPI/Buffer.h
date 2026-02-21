#pragma once

class Buffer final
{
    friend class RenderingContext;
public:
    void SetDebugName(ccstr8 name);
    void GetDebugName(ccstr8& name);

	void SetData(void* data);
	void SetData(void* data, i32 size);

	void SetSubdata(u32 offset, u32 size, void* data);

	__forceinline uptr GetNativeHandle() const { return m_NativeHandle; }

	u32 GetSize() const { return m_Size; }

	void SetType(BufferType type) { m_Type = type; }
	BufferType GetType() const { return m_Type; }

	~Buffer();
protected:
    Buffer(BufferType type, u32 initialSize, BufferUsage usage);
protected:
	uptr m_NativeHandle;

	BufferType m_Type;
    BufferUsage m_Usage;
    u32 m_Size;
};