#include "Core.h"

cbuffer SSAOParameters : register(b1)
{
    uint DepthTextureIndex;
    uint NormalTextureIndex;
    uint NoiseTextureIndex;

    float NoiseScale;
    float Radius;
    float Bias;

    uint SampleCount;
    float3 Samples[SSAO_MAX_SAMPLES_COUNT];
};

RWTexture2D<float4> CurrentAO : register(u0);
RWTexture2D<float4> HistoryAO : register(u1);
