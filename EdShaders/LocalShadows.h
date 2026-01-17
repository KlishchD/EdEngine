#include "Core.h"

#define MAX_SHADOWS_COUNT 6

cbuffer MetaData : register(b0, space1)
{
    uint ModelIndex;
    uint ShadowOffset;
    uint ShadowCount;
};

cbuffer Models : register(b1)
{
    float4x4 ModelsWorldTransforms[1000];
};

cbuffer Lights : register(b2)
{
    float4x4 ShadowTransforms[1000];
}

struct GSOutput
{
    float4 Position : SV_Position;
    uint Viewport : SV_ViewportArrayIndex;
};

float4 VSMain(float3 position : Position) : SV_Position
{
    return mul(ModelsWorldTransforms[ModelIndex], float4(position, 1.0f));
}

[maxvertexcount(MAX_SHADOWS_COUNT * 3)]
void GSMain(triangle float4 position[3] : SV_Position, inout TriangleStream<GSOutput> stream)
{
    [unroll(MAX_SHADOWS_COUNT)]
    for (uint i = 0; i < ShadowCount; ++i)
    {
        [unroll(3)]
        for (uint j = 0; j < 3; ++j)
        {
            GSOutput output;
            output.Position = mul(ShadowTransforms[ShadowOffset + i], position[j]);
            output.Viewport = i;

            stream.Append(output);
        }

        stream.RestartStrip();
    }
}

void PSMain()
{

}