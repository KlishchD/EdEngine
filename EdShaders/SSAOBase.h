#include "SSAO.h"

#define GET_SSAO_SAMPLE(index) float3(1.0f, 1.0f, 1.0f)

[numthreads(SSAO_TILE_SIZE, SSAO_TILE_SIZE, 1)]
void CSMain(uint2 screenPosition : SV_DispatchThreadID)
{
    if (all(screenPosition < ScreenSize))
    {
        Texture2D<float> depthTarget = GetTextureFloat(DepthTextureIndex);
        Texture2D<float4> normalTarget = GetTextureFloat4(NormalTextureIndex);
        Texture2D<float2> noiseTarget = GetTextureFloat2(NoiseTextureIndex);

        float2 uv = screenPosition * InverseScreenSize * 2.0f;

        float depth = depthTarget.Sample(BilinearWrapSampler, uv);
        float4 clipPosition = float4(uv * 2.0f - 1.0f, depth, 1.0f);

        float3 world = ScreenToWorld(uv, depth);

        float3 normal = normalTarget.Sample(BilinearWrapSampler, uv).xyz;

        float3 noise = float3(noiseTarget.Sample(PointWrapSampler, uv * NoiseScale), 0.0f);
        float3 tangent = normalize(noise - dot(normal, noise) * normal); // Math basis?
        float3 bitangent = cross(tangent, normal);
        float3x3 TBN = float3x3(tangent, bitangent, normal);

        float sampleContribution = rcp(SampleCount);
        float AO = 0.0f;

        [unroll(SSAO_MAX_SAMPLES_COUNT)]
        for (uint i = 0; i < SampleCount; ++i)
        {
            float3 sampleDirectionTangent = Samples[i].xyz;
            float3 sampleDirectionWorld = mul(TBN, sampleDirectionTangent);
            sampleDirectionWorld = normalize(sampleDirectionWorld);

            float4 samplePositionWorld = float4(world + sampleDirectionWorld * 0.1, 1.0f);

            float4 samplePositionClip = mul(ProjectionViewTransform, samplePositionWorld);
            samplePositionClip.xyz /= samplePositionClip.w;
            samplePositionClip.xy = samplePositionClip.xy * 0.5f + 0.5f;

            float sampleDepth = samplePositionClip.z;
            float sceneDepth = depthTarget.Sample(BilinearWrapSampler, samplePositionClip.xy);

            float rangeCheck = smoothstep(0.0f, 1.0f, Radius / abs(sampleDepth - sceneDepth));

            [branch]
            if (sampleDepth > sceneDepth + Bias)
            {
                AO += sampleContribution * rangeCheck;
            }

            //CurrentAO[screenPosition] = float4(samplePositionWorld.xyz, 1.0f);// float4(sceneDepth, 0.0f, 0.0f, 0.0f);//sampleDepth;//sceneDepth;
            //break;
        }
        CurrentAO[screenPosition] = AO;// float4(sceneDepth, 0.0f, 0.0f, 0.0f);//sampleDepth;//sceneDepth;
    }
}