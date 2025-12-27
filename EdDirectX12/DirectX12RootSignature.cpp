#include "EdDirectX12.h"

RootSignature::RootSignature(const RootSignatureBuilder& builder)
{
    D3D12_ROOT_SIGNATURE_DESC description;
    description.NumParameters = builder.m_Tables.GetSize() + builder.m_Constants.GetSize() + builder.m_Descriptors.GetSize();
    description.pParameters = Memory::Get().RequestDynamicMemory<D3D12_ROOT_PARAMETER>(description.NumParameters, 1, "D3D12RootSignatureParameters");

    for (u32 i = 0; i < builder.m_Tables.GetSize(); ++i)
    {
        const RootSignatureBuilder::DescriptorTableParameter& source = builder.m_Tables[i];

        D3D12_ROOT_PARAMETER& parameter = const_cast<D3D12_ROOT_PARAMETER&>(description.pParameters[source.Index]);
        parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        parameter.DescriptorTable.NumDescriptorRanges = 1;

        D3D12_DESCRIPTOR_RANGE* range = Memory::Get().RequestDynamicMemory<D3D12_DESCRIPTOR_RANGE>(1, 1, "D3D12DescriptorRange");
        range->RangeType = DirectX12Types::ConvertRangeType(source.Type);
        range->NumDescriptors = source.Count;
        range->BaseShaderRegister = source.Register;
        range->RegisterSpace = source.Space;
        range->OffsetInDescriptorsFromTableStart = source.Offset;

        parameter.DescriptorTable.pDescriptorRanges = range;
        parameter.ShaderVisibility = DirectX12Types::ConvertShaderVisibility(source.Visibility);
    }

    for (u32 i = 0; i < builder.m_Constants.GetSize(); ++i)
    {
        const RootSignatureBuilder::ConstantParameter& source = builder.m_Constants[i];

        D3D12_ROOT_PARAMETER& parameter = const_cast<D3D12_ROOT_PARAMETER&>(description.pParameters[source.Index]);
        parameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
        parameter.Constants.ShaderRegister = source.Register;
        parameter.Constants.RegisterSpace = source.Space;
        parameter.Constants.Num32BitValues = source.Count;
        parameter.ShaderVisibility = DirectX12Types::ConvertShaderVisibility(source.Visibility);
    }

    for (u32 i = 0; i < builder.m_Descriptors.GetSize(); ++i)
    {
        const RootSignatureBuilder::DescriptorParameter& source = builder.m_Descriptors[i];

        D3D12_ROOT_PARAMETER& parameter = const_cast<D3D12_ROOT_PARAMETER&>(description.pParameters[source.Index]);
        parameter.ParameterType = DirectX12Types::ConvertRooParmaterType(source.Type);
        parameter.Descriptor.ShaderRegister = source.Register;
        parameter.Descriptor.RegisterSpace = source.Space;
        parameter.ShaderVisibility = DirectX12Types::ConvertShaderVisibility(source.Visibility);
    }

    description.NumStaticSamplers = builder.m_Samplers.GetSize();
    description.pStaticSamplers = Memory::Get().RequestDynamicMemory<D3D12_STATIC_SAMPLER_DESC>(description.NumStaticSamplers, 1, "D3D12StaticSamplers");

    for (u32 i = 0; i < description.NumStaticSamplers; ++i)
    {
        const RootSignatureBuilder::StaticSampler& source = builder.m_Samplers[i];

        D3D12_STATIC_SAMPLER_DESC& sampler = const_cast<D3D12_STATIC_SAMPLER_DESC&>(description.pStaticSamplers[i]);

        sampler.Filter = DirectX12Types::ConvertFilteringMode(source.Filtering);
        sampler.AddressU = DirectX12Types::ConvertWrapMode(source.WrapU);
        sampler.AddressV = DirectX12Types::ConvertWrapMode(source.WrapV);
        sampler.AddressW = DirectX12Types::ConvertWrapMode(source.WrapW);
        sampler.MipLODBias = 0;
        sampler.MaxAnisotropy = source.Anisotrophy;
        sampler.ComparisonFunc = DirectX12Types::ConvertComparisonFunction(source.Comparison);
        sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        sampler.MinLOD = 0;
        sampler.MaxLOD = 100;
        sampler.ShaderRegister = source.Register;
        sampler.RegisterSpace = source.Space;
        sampler.ShaderVisibility = DirectX12Types::ConvertShaderVisibility(source.Visibility);
    }

    description.Flags = DirectX12Types::ConvertRootSignatureFlags(builder.m_Flags);

    Microsoft::WRL::ComPtr<ID3DBlob> output, errors;
    D3D::Check(D3D12SerializeRootSignature(&description, D3D_ROOT_SIGNATURE_VERSION_1, &output, &errors));

    if (errors)
    {
        ccstr8 message = reinterpret_cast<ccstr8>(errors->GetBufferPointer());
        ED_ASSERT(0, "Failed to create root signature {}", message);
    }

    D3D::Check(g_Device->CreateRootSignature(0, output->GetBufferPointer(), output->GetBufferSize(), __uuidof(ID3D12RootSignature), reinterpret_cast<void**>(&m_NativeHandle)));
}


NAME_METHODS_TEMPLATE(RootSignature, ID3D12RootSignature)

RootSignature::~RootSignature()
{
    GetNativeHandle<ID3D12RootSignature>()->Release();
}
