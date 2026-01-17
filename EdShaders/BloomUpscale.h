#include "Bloom.h"

cbuffer Parameters : register(b0, space1)
{
    float BloomUpscaleStrength;
};

Texture2D<float3> BloomRead : register(t0, space1);
RWTexture2D<float3> BloomWrite[BLOOM_MIPS_COUNT] : register(u3);

void SampleInput(in float2 position, in float2 offset, in float weight, inout float3 totalColor, inout float totalWeight)
{
    float2 size = max(1, ScreenSize >> (BLOOM_MIP_LEVEL + 2));
    float2 sample = position + offset;

    bool2 inside = and (sample >= 0, sample < size);

    [branch]
    if (all(inside))
    {
        float2 uv = (sample + 0.5f) / size;

        float3 color = BloomRead.SampleLevel(BilinearBorderSampler, uv, BLOOM_MIP_LEVEL + 1);

        totalColor += color * weight;
        totalWeight += weight;
    }
}

[numthreads(BLOOM_TILE_SIZE, BLOOM_TILE_SIZE, 1)]
void CSMain(int2 output : SV_DispatchThreadID)
{
    float2 outputSize = max(1, (ScreenSize >> (BLOOM_MIP_LEVEL + 1)));
    bool2 inside = output < outputSize;

    [branch]
    if (all(inside))
    {
        // 1 2 1
        // 2 4 2
        // 1 2 1

        // a b c
        // d e f
        // g h i

        float2 input = output * 0.5f;

        float3 color = 0.0f;
        float weight = 0.0f;

        SampleInput(input, float2(-1.0f,  1.0f), 1.0f, color, weight); // a
        SampleInput(input, float2( 0.0f,  1.0f), 2.0f, color, weight); // b
        SampleInput(input, float2( 1.0f,  1.0f), 1.0f, color, weight); // c

        SampleInput(input, float2(-1.0f,  0.0f), 2.0f, color, weight); // d
        SampleInput(input, float2( 0.0f,  0.0f), 4.0f, color, weight); // e
        SampleInput(input, float2( 1.0f,  0.0f), 2.0f, color, weight); // f

        SampleInput(input, float2(-1.0f, -1.0f), 1.0f, color, weight); // g
        SampleInput(input, float2( 0.0f, -1.0f), 2.0f, color, weight); // h
        SampleInput(input, float2( 1.0f, -1.0f), 1.0f, color, weight); // i

        float3 average = color / (weight + 0.0001f);

        float3 upscaled = BloomWrite[BLOOM_MIP_LEVEL][output];
        BloomWrite[BLOOM_MIP_LEVEL][output] = lerp(0.0f, upscaled + average, BloomUpscaleStrength);
    }
}