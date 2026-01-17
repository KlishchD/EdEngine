#include "Core.h"

struct ObjectMatrix
{
    float4x4 WorldTransform;
    float4x4 PreviousWorldTransform;
};

cbuffer ObjectData : register(b1)
{
    ObjectMatrix Matrices[512];
};

cbuffer ObjectDataRoot : register(b0, space1)
{
    float3 NormalTransform1;
    uint AlbedoTextureIndex;

    float3 NormalTransform2;
    uint NormalTextureIndex;

    float3 NormalTransform3;
    uint RoughnessTextureIndex;

    float Metalic;
    float Roughness;
    float Emission;
    uint MetalicTextureIndex;

    uint Index;
};

struct VSInput
{
    float3 Position : Position;
    float4 BaseColor : Color;
    float3 UVs : UVs;
    float3 Normals : Normals;
    float3 Tangents : Tangents;
    float3 Bitangent : Bitangent;
};

struct VSOuput
{
    float4 Position : SV_Position; 
    float3 CurrentPosition : CurrentPosition;
    float3 PreviousPosition : PreviousPosition;
    float4 BaseColor : BaseColor;
    float4 UV : UV;
    float3 Tangent : Tangent;
    float3 Normal : Normal;
    float3 Bitangent : Bitangent;
};

struct PSOutput
{
    float4 Albedo : SV_Target0;
    float4 Normal : SV_Target1;
    float4 Material : SV_Target2;
    float2 Velocity : SV_Target3;
};

float4x4 GetWorlTransform()
{
    return Matrices[Index].WorldTransform;
}

float4x4 GetPreviousWroldTransform()
{
    return Matrices[Index].PreviousWorldTransform;
}

float3x3 GetNormalTransform()
{
    return float3x3(NormalTransform1, NormalTransform2, NormalTransform3);
}

Texture2D<float4> GetAlbedoTexture()
{
    return GetTextureFloat4(AlbedoTextureIndex);
}

bool PerformNormalMapping()
{
    return NormalTextureIndex != DEFAULT_BINDLESS_INDEX;
}

Texture2D<float4> GetNormalTexture()
{
    return GetTextureFloat4(NormalTextureIndex);
}

Texture2D<float> GetMetalicTexture()
{
    return GetTextureFloat(MetalicTextureIndex);
}

Texture2D<float> GetRoughnessTexture()
{
    return GetTextureFloat(RoughnessTextureIndex);
}

void ResolvePositions(in VSInput input, inout VSOuput output)
{
    float4 currentPosition = mul(GetWorlTransform(), float4(input.Position, 1.0f));
    float4 previousPosition = mul(GetPreviousWroldTransform(), float4(input.Position, 1.0f));

    currentPosition = mul(ProjectionViewTransform, currentPosition);
    previousPosition = mul(ProjectionViewTransform, previousPosition);

    output.Position = currentPosition;
    output.CurrentPosition = currentPosition.xyz * rcp(currentPosition.w);
    output.PreviousPosition = currentPosition.xyz * rcp(previousPosition.w);
}

void ResolveNormal(in VSInput input, inout VSOuput output)
{
    float3x3 transform = GetNormalTransform();
    output.Normal = normalize(mul(transform, input.Normals));
    output.Tangent = normalize(mul(transform, input.Tangents));
    output.Bitangent = cross(output.Tangent, output.Normal);
}

VSOuput VSMain(VSInput input)
{
    VSOuput output;

    ResolvePositions(input, output);
    ResolveNormal(input, output);

    output.BaseColor = input.BaseColor;
    output.UV = float4(input.UVs.xy, 1.0f, 1.0f);

    return output;
}

PSOutput PSMain(VSOuput input)
{
    PSOutput output;

    Texture2D<float4> albedoTexture = GetAlbedoTexture();
    Texture2D<float4> normalTexture = GetNormalTexture();
    Texture2D<float> metalicTexture = GetMetalicTexture();
    Texture2D<float> roughnessTexture = GetRoughnessTexture();

    output.Albedo = albedoTexture.Sample(BilinearWrapSampler, input.UV.xy);

    float3 normal = input.Normal;

    [branch]
    if (PerformNormalMapping())
    {
        normal = normalTexture.Sample(PointWrapSampler, input.UV.xy).xyz;
        normal = normalize(normal * 2.0f - 1.0f);
   
        float3x3 TBN = float3x3(input.Tangent, input.Bitangent, input.Normal);
        normal = mul(TBN, normal);
    }

    output.Normal = float4(normal, input.CurrentPosition.z);
    
    float metalic = Metalic * metalicTexture.Sample(PointWrapSampler, input.UV.xy);
    float roughness = Roughness * roughnessTexture.Sample(PointWrapSampler, input.UV.xy);
    output.Material = float4(metalic, roughness, Emission, 1.0f);
    
    output.Velocity = input.PreviousPosition.xy - input.CurrentPosition.xy;
    
    return output;
}