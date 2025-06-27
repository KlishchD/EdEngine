#include "Core.h"

cbuffer ObjectDataRoot : register(b0, space1)
{
    float4x4 WorldTransform;
};

float4 VSMain(float3 position : Position) : SV_POSITION
{
    return mul(ProjectionViewTransform, mul(WorldTransform, float4(position, 1.0f)));
}

void PSMain()
{
}