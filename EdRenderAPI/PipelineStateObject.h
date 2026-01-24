#pragma once

#include "PipelineStateObjectBuilder.h"

class PipelineStateObject
{
public:
    enum Type : u8
    {
        Graphics,
        Compute
    };

    PipelineStateObject(ccstr8 name, const GraphicsPipelineStateObjectBuilder& builder);
    PipelineStateObject(ccstr8 name, const ComputePipelineStateObjectBuilder& builder);

    void Recreate(const GraphicsPipelineStateObjectBuilder& builder);
    void Recreate(const ComputePipelineStateObjectBuilder& builder);

    void SetDebugName(ccstr8 name, i32 size = -1);
    void GetDebugName(ccstr8& name, i32& size);

    Type GetType() const { return m_Type; }

    void SetVertexStride(u32 stride) { m_VertexStride = stride; }
    u32 GetVertexStride() const { return m_VertexStride; }

    template <typename T>
    __forceinline T* GetNativeHandle() const
    {
        return (T*)m_NativeHandle;
    }

    __forceinline uptr GetNativeHandle() const { return m_NativeHandle; }

    ~PipelineStateObject();
protected:
    uptr m_NativeHandle;
    ccstr8 m_Name;
    Type m_Type;
    u32 m_VertexStride;
};

