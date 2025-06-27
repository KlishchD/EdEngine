#include "Lighting.h"

#define MAX_SHADOWS_COUNT 6

Texture2D<float4> Albedo : register(t0);
Texture2D<float4> Normals : register(t1);
Texture2D<float4> Material : register(t2);

float4 VSMain(float2 position : Position) : SV_Position
{
    return float4(position, 0.0f, 1.0f);
}

float4 PSMain(float4 position : SV_Position) : SV_Target0
{
    float2 screen = position.xy / ScreenSize;

    float3 albedo = Albedo.Sample(BilinearWrapSampler, screen).rgb;
    float3 material = Material.Sample(BilinearWrapSampler, screen).rgb;
    
    const float ambient = 0.1f;
    const float emissive = material.b;
    float3 lighting = (ambient + emissive) * albedo;
    
    return float4(lighting, 1.0f);
}