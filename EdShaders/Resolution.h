#include "Lighting.h"

cbuffer Parameters : register(b0, space1)
{
    float Gamma;
    float BloomStrength;
    float BloomIntensity;
};

Texture2D<float4> Lighting : register(t0);
Texture2D<float3> Bloom : register(t0, space1);

float4 VSMain(float2 position : Position) : SV_Position
{
    return float4(position, 0.0f, 1.0f);
}

float4 PSMain(float4 position : SV_Position) : SV_Target0
{
    float2 screen = position.xy / ScreenSize;

    float3 color = Lighting.Sample(BilinearWrapSampler, screen).rgb;

    [branch]
    if (BloomIntensity > 0.01f)
    {
        float3 bloom = Bloom.Sample(BilinearWrapSampler, screen);
        color = lerp(color, bloom * BloomIntensity, BloomStrength);
    }

    color = ACESFilm(color);
    color = pow(color, 1.0f / Gamma);

    color.r = 0.2f;
    color.g = 0.3f;

    return float4(color, 1.0f);
}