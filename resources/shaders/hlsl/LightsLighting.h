#include "Lighting.h"

#define POINT_LIGHT_TYPE 1
#define SPOT_LIGHT_TYPE 2

#define SHADOW_SAMPLES_BLOCK_DIMENSTIONS 10
#define SHADOW_SAMPLES_BLOCK_COUNT 10

#ifndef LIGHT_TYPE
    #error Light type was not provided
#endif

Texture2D<float4> Albedo : register(t0);
Texture2D<float4> Normals : register(t1);
Texture2D<float4> Material : register(t2);

Texture2D<float2> ShadowNoise : register(t0, space1);
Texture2D<float> ShadowMap : register(t0, space2);
Texture2D<float> Depth : register(t0, space3);

#if LIGHT_TYPE == SPOT_LIGHT_TYPE
    cbuffer LigtsData : register(b1)
    {
        float4x4 WorldTransform;
        float4x4 ShadowProjectionViewTransform;

        float3 Position;
        float InnerAngle;

        float3 Forward;
        float OuterAngle;

        float3 Color;
        float MaxDistance;

        int2 ShadowMapStart;
        uint2 ShadowMapSize;

        float3 ViewPosition;

        float Intensity;

        float ShadowFilterRadius;
        int ShadowFilterSize;
    };
#elif LIGHT_TYPE == POINT_LIGHT_TYPE
    cbuffer LigtsData : register(b1)
    {
        float3 Position;
        float Radius;

        float3 Color;
        float Intensity;

        int2 ShadowMapStart;
        uint2 ShadowMapSize;

        float3 ViewPosition;

        float ShadowViewDepthConversion0;
        float ShadowViewDepthConversion1;
        float ShadowFilterRadius;
        int ShadowFilterSize;
    };
#endif

float4 VSMain(float3 position : Position) : SV_Position
{
    #if LIGHT_TYPE == SPOT_LIGHT_TYPE
        float4 world = mul(WorldTransform, float4(position, 1.0f));
    #elif LIGHT_TYPE == POINT_LIGHT_TYPE
        float4 world = float4(position * Radius + Position, 1.0f);
    #endif

    return mul(ProjectionViewTransform, world);
}

float SampleLocalShadowMap(float3 position, float2 start, float3 uv, float bias)
{
    float visible = 0.0f;

    float2 offset = (position.yz * ScreenSize) * (1.0f / SHADOW_SAMPLES_BLOCK_COUNT);

    float filterSize = ShadowFilterSize * 2 + 1;
    float invFilertSize = rcp(filterSize);

    [unroll(10)]
    for (int i = -ShadowFilterSize; i <= ShadowFilterSize; ++i)
    {
        [unroll(10)]
        for (int j = -ShadowFilterSize; j <= ShadowFilterSize; ++j)
        {
            float2 filter = float2(i, j) * invFilertSize;
            float2 noise = ShadowNoise.Sample(PointWrapSampler, filter + offset) * ShadowFilterRadius;

            float2 local = uv.xy * ShadowMapSize + noise;
            uint2 sample = start + clamp(local, 0.0f, ShadowMapSize);

            float depth = ShadowMap[sample];

            visible += depth - bias <= uv.z ? 1.0f : 0.0f;
        }
    }

    return visible / (filterSize * filterSize);
}

#if LIGHT_TYPE == SPOT_LIGHT_TYPE
    float GetVisibility(float2 screen, float3 position, float3 light, float3 normal)
    {
        [branch]
        if (ShadowMapStart.x != -1)
        {
            float4 shadowClip = mul(ShadowProjectionViewTransform, float4(position, 1.0f));
            float3 shadowUV = ClipToScreen(shadowClip);

            float NdotL = max(dot(normal, light), 0.0f);
            float bias = 0.0f; //max(5.0f * (1.0f - NdotL), 1.0f);

            return SampleLocalShadowMap(position, ShadowMapStart, shadowUV, bias);
        }
        else
        {
            return 1.0f;
        }
    }

    float GetAttenuation(float3 world, float3 light)
    {
        float angle = dot(-light, Forward);

        float3 pointLight = Position - world;
        float distanceSqr = dot(pointLight, pointLight);

        [branch]
        if (OuterAngle > angle || distanceSqr >= MaxDistance * MaxDistance)
        {
            return 0.0f;
        }

        float a = 1.0f / (InnerAngle - OuterAngle);
        float b = -OuterAngle * a;
        float softness = clamp((a * angle + b) * (a * angle + b), 0.0f, 1.0f);

        return softness / (distanceSqr + 1.0f);
    }
#elif LIGHT_TYPE == POINT_LIGHT_TYPE
    void GetShadowMapSamplePosition(in float3 direction, out float map, out float3 uv)
    {
        float3 directionAbs = abs(direction);
        float cardinal = max(directionAbs.x, max(directionAbs.y, directionAbs.z));
        float3 normalized = direction / (cardinal + 0.0001);

        [branch]
        if (normalized.x >= 0.95f)
        {
            map = 4;
            uv.xy = float2(-normalized.z, -normalized.y) * 0.5f + 0.5f;
            return;
        }

        [branch]
        if (normalized.x <= -0.95f)
        {
            map = 5;
            uv.xy = float2(normalized.z, normalized.y) * 0.5f + 0.5f;
            return;
        }

        [branch]
        if (normalized.y >= 0.95f)
        {
            map = 3;
            uv.xy = float2(-normalized.z, -normalized.x) * 0.5f + 0.5f;
            return;
        }

        [branch]
        if (normalized.y <= -0.95f)
        {
            map = 2;
            uv.xy = float2(normalized.z, -normalized.x) * 0.5f + 0.5f;
            return;
        }

        [branch]
        if (normalized.z >= 0.95f)
        {
            map = 0;
            uv.xy = float2(-normalized.x, -normalized.y) * 0.5f + 0.5f;
            return;
        }

        [branch]
        if (normalized.z <= -0.95f)
        {
            map = 1;
            uv.xy = float2(normalized.x, -normalized.y) * 0.5f + 0.5f;
            return;
        }

        map = 1000.0f;
        uv = 100.0f;
    }

    float GetVisibility(float2 screen, float3 position, float3 light, float3 normal)
    {
        [branch]
        if (ShadowMapStart.x != -1)
        {
            float shadowMap;
            float3 shadowUV;

            float shadowViewDepth = length(position - Position);
            GetShadowMapSamplePosition(-light, shadowMap, shadowUV);
            shadowUV.z = (shadowViewDepth * ShadowViewDepthConversion0 + ShadowViewDepthConversion1) / shadowViewDepth;

            float2 offset = float2(shadowMap * ShadowMapSize.x, 0.0f);

            float NdotL = max(dot(normal, light), 0.0f);
            float bias = max(5.0f * (1.0f - NdotL), 1.0f);

            return SampleLocalShadowMap(position, ShadowMapStart + offset, shadowUV, bias);
        }
        else
        {
            return 1.0f;
        }
    }

    float GetAttenuation(float3 position, float3 light)
    {
        float3 lightVector = Position - position;
        float distanceSqr = dot(lightVector, lightVector);
        float radiusSqr = Radius * Radius;

        float A = distanceSqr / radiusSqr;
        float B = clamp(1 - A * A, 0.0f, 1.0f);
        float numerator = B * B;

        float denominator = distanceSqr + 1.0f;

        return numerator / denominator;
    }
#endif

float4 PSMain(float4 position : SV_Position) : SV_Target0
{
    float2 screen = position.xy / ScreenSize;

    float3 normal = Normals.Sample(BilinearClampSampler, screen).xyz;

    float depth = Depth.Sample(PointClampSampler, screen);
    float3 world = ScreenToWorld(screen, depth);

    float3 view = normalize(ViewPosition - world);
    float3 light = normalize(Position - world);

    float3 albedo = Albedo.Sample(BilinearClampSampler, screen).xyz;
    float2 material = Material.Sample(BilinearClampSampler, screen).xy;

    float visibility = GetVisibility(screen, world, light, normal);
    float attenuation = GetAttenuation(world, light);

    float3 brdf = BRDF(albedo, material, normal, view, light) * Intensity * Color;

    return float4(brdf * visibility * attenuation, 1.0f);
}