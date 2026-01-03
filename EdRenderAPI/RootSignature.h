#pragma once

class RootSignatureBuilder
{
    struct DescriptorTableParameter
    {
        u32 Register;
        u32 Space;
        DescriptorRangeType Type;
        u32 Count;
        u32 Offset;
        ShaderVisbilityFlags Visibility;
        u32 Index;
    };

    struct ConstantParameter
    {
        u32 Register;
        u32 Space;
        u32 Count;
        ShaderVisbilityFlags Visibility;
        u32 Index;
    };

    struct DescriptorParameter
    {
        DescriptorHeapType Type;
        u32 Register;
        u32 Space;
        ShaderVisbilityFlags Visibility;
        u32 Index;
    };

    struct StaticSampler
    {
        u32 Register;
        u32 Space;
        FilteringMode Filtering;
        WrapMode WrapU;
        WrapMode WrapV;
        WrapMode WrapW;
        u32 Anisotrophy;
        ComparisonFunction Comparison;
        ShaderVisbilityFlags Visibility;
    };

    friend class RootSignature;
public:
    __forceinline RootSignatureBuilder& AddParameter(u32 reg, u32 space, DescriptorRangeType type, u32 count, u32 offset, ShaderVisbilityFlags visibility)
    {
        DescriptorTableParameter& parameter = m_Tables.Add();
        parameter.Register = reg;
        parameter.Space = space;
        parameter.Type = type;
        parameter.Count = count;
        parameter.Offset = offset;
        parameter.Visibility = visibility;
        parameter.Index = m_ParametersIndex++;

        return *this;
    }

    __forceinline RootSignatureBuilder& AddParameter(u32 reg, u32 space, u32 count, ShaderVisbilityFlags visibility)
    {
        ConstantParameter& parameter = m_Constants.Add();
        parameter.Register = reg;
        parameter.Space = space;
        parameter.Count = count;
        parameter.Visibility = visibility;
        parameter.Index = m_ParametersIndex++;

        return *this;
    }

    __forceinline RootSignatureBuilder& AddParameter(DescriptorHeapType type, u32 reg, u32 space, ShaderVisbilityFlags visibility)
    {
        ED_ASSERT(type == DescriptorHeapType::CBV || type == DescriptorHeapType::UAV || type == DescriptorHeapType::SRV, "Descriptor parameter can only be of CBV, UAV or SRV type.");
        DescriptorParameter& parameter = m_Descriptors.Add();
        parameter.Type = type;
        parameter.Register = reg;
        parameter.Space = space;
        parameter.Visibility = visibility;
        parameter.Index = m_ParametersIndex++;

        return *this;
    }

    __forceinline RootSignatureBuilder& AddStaticSampler(u32 reg, u32 space, FilteringMode filtering, WrapMode wrapU, WrapMode wrapV, WrapMode wrapM, u32 anisotrophy, ComparisonFunction comparison, ShaderVisbilityFlags visibility)
    {
        StaticSampler& sampler = m_Samplers.Add();
        sampler.Register = reg;
        sampler.Space = space;
        sampler.Filtering = filtering;
        sampler.WrapU = wrapU;
        sampler.WrapV = wrapV;
        sampler.WrapW = wrapM;
        sampler.Anisotrophy = anisotrophy;
        sampler.Comparison = comparison;
        sampler.Visibility = visibility;

        return *this;
    }

    RootSignatureBuilder& AddFlags(RootSignatureFlags flags)
    {
        m_Flags = static_cast<RootSignatureFlags>(m_Flags | flags);
        return *this;
    }

    __forceinline RootSignatureBuilder& AddShaderVisibility(ShaderVisbilityFlags visibility)
    {
        m_VisibilityFlags = static_cast<ShaderVisbilityFlags>(m_VisibilityFlags | visibility);
        return *this;
    }

    __forceinline RootSignatureBuilder& SetShaderVisibility(ShaderVisbilityFlags visibility)
    {
        m_VisibilityFlags = visibility;
        return *this;
    }

protected:
    TemporaryArray<DescriptorTableParameter> m_Tables;
    TemporaryArray<ConstantParameter> m_Constants;
    TemporaryArray<DescriptorParameter> m_Descriptors;
    TemporaryArray<StaticSampler> m_Samplers;
    RootSignatureFlags m_Flags = RSF_None;
    ShaderVisbilityFlags m_VisibilityFlags = SVF_All;
    u32 m_ParametersIndex = 0;
};

class RootSignature
{
public:
    RootSignature(const RootSignatureBuilder& builder);

    void SetDebugName(ccstr8 name, i32 size = -1);
    void GetDebugName(ccstr8& name, i32& size);

    template <typename T>
    __forceinline T* GetNativeHandle() const
    {
        return (T*)m_NativeHandle;
    }

    __forceinline uptr GetNativeHandle() const { return m_NativeHandle; }

    ~RootSignature();
protected:
    uptr m_NativeHandle;
};
