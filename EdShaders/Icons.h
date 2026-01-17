#include "Core.h"

struct IconParameters
{
    float3 Tint;
    uint IconIndex;

    float3 Location;
    float UniformScale;
};

cbuffer Icons : register(b1)
{
    IconParameters Parameters[100];
};

cbuffer Meta : register(b0, space1)
{
    uint IconIndex;
}

float4 VSMain(uint index : SV_VertexID) : SV_Position
{
    float2 offset = 0.5f * Parameters[IconIndex].UniformScale * float2(index % 2 ? 1.0f : -1.0f, index / 2 ? 1.0f : -1.0f);
    float3 up = float3(ViewTransform[0][1], ViewTransform[1][2], ViewTransform[2][2]);
    float3 right = float3(ViewTransform[0][2], ViewTransform[1][2], ViewTransform[2][2]);
    float3 viewFacing = Parameters[IconIndex].Location + offset.x * right + offset.y * up;
    return mul(ProjectionViewTransform, float4(viewFacing, 1.0f));
}

float4 PSMain(float4 position : SV_Position) : SV_Target0
{
    Texture2D<float4> icon = GetTextureFloat4(Parameters[IconIndex].IconIndex);
    float4 color = icon.Sample(TrilinearWrapSampler, position.xy * InverseScreenSize);
    color.rgb *= Parameters[IconIndex].Tint;
    return color;
}