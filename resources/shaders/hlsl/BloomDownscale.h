#include "Bloom.h"

Texture2D<float4> Input : register(t0);
Texture2D<float3> BloomRead : register(t0, space1);
RWTexture2D<float3> BloomWrite[BLOOM_MIPS_COUNT] : register(u3);

void SampleInput(float2 position, float2 offset, float weight, inout float3 totalColor, inout float totalWeight)
{
	int2 sample = position + offset;
    
	int2 size = max(1, ScreenSize >> BLOOM_MIP_LEVEL);
	bool2 inside = and(sample >= 0, sample < size);

	[branch]
	if (all(inside))
	{
        float2 uv = (sample + 0.5f) / size;
        
		float3 color;

	#if BLOOM_MIP_LEVEL == 0
		color = Input.Sample(BilinearClampSampler, uv).rgb;
	#else
		color = BloomRead.SampleLevel(BilinearClampSampler, uv, BLOOM_MIP_LEVEL - 1);
	#endif

		totalColor += color * weight;
		totalWeight += weight;
	}
}

[numthreads(BLOOM_TILE_SIZE, BLOOM_TILE_SIZE, 1)]
void CSMain(int2 output : SV_DispatchThreadID)
{
	float2 outputSize = max(1.0f, (ScreenSize >> (BLOOM_MIP_LEVEL + 1)));
	bool2 inside = output < outputSize;

	if (all(inside))
	{
        // a - b - c
        // - d - e -
        // f - g - h
        // - i - j -
        // k - l - m

		float2 input = output * 2.0f;

		float3 color = 0.0f;
		float weight = 0.0f;

		SampleInput(input, float2(-2.0f,  2.0f), 0.125f, color, weight); // a
		SampleInput(input, float2( 0.0f,  2.0f), 0.250f, color, weight); // b
		SampleInput(input, float2( 2.0f,  2.0f), 0.125f, color, weight); // c

		SampleInput(input, float2(-1.0f,  1.0f), 0.500f, color, weight); // d
		SampleInput(input, float2( 1.0f,  1.0f), 0.500f, color, weight); // e

		SampleInput(input, float2(-2.0f,  0.0f), 0.250f, color, weight); // f
		SampleInput(input, float2( 0.0f,  0.0f), 0.500f, color, weight); // g
		SampleInput(input, float2( 2.0f,  0.0f), 0.250f, color, weight); // h

		SampleInput(input, float2(-1.0f, -1.0f), 0.500f, color, weight); // i
		SampleInput(input, float2( 1.0f, -1.0f), 0.500f, color, weight); // j

		SampleInput(input, float2(-2.0f, -2.0f), 0.125f, color, weight); // k
		SampleInput(input, float2( 0.0f, -2.0f), 0.250f, color, weight); // l
		SampleInput(input, float2( 2.0f, -2.0f), 0.125f, color, weight); // m

		BloomWrite[BLOOM_MIP_LEVEL][output] = color / max(weight, 0.0001f);
	}
}