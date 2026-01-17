#include "Constants.h"

cbuffer GlobalData : register(b0)
{
    float4x4 ViewTransform;
    float4x4 ProjectionTransform;
    float4x4 ProjectionViewTransform;
    float4x4 InverseProjectionViewTransform;

    uint2 ScreenSize;
    float2 InverseScreenSize;
};

sampler TrilinearWrapSampler : register(s0, space0);
sampler BilinearWrapSampler : register(s1, space0);
sampler PointWrapSampler : register(s2, space0);
sampler AnisoWrapSampler : register(s3, space0);

sampler TrilinearClampSampler : register(s0, space1);
sampler BilinearClampSampler : register(s1, space1);
sampler PointClampSampler : register(s2, space1);
sampler AnisoClampSampler : register(s3, space1);

sampler TrilinearBorderSampler : register(s0, space2);
sampler BilinearBorderSampler : register(s1, space2);
sampler PointBorderSampler : register(s2, space2);
sampler AnisoBorderSampler : register(s3, space3);

sampler TrilinearMirrorSampler : register(s0, space3);
sampler BilinearMirrorSampler : register(s1, space3);
sampler PointMirrorSampler : register(s2, space3);
sampler AnisoMirrorSampler : register(s3, space3);

sampler TrilinearMirrorOnceSampler : register(s0, space4);
sampler BilinearMirrorOnceSampler : register(s1, space4);
sampler PointMirrorOnceSampler : register(s2, space4);
sampler AnisoMirrorOnceSampler : register(s3, space4);

#ifndef NO_BINDLESS

Texture2D<float>  Textures[1024]  : register(t0, space0);
Texture2D<float2> Textures2[1024] : register(t0, space1);
Texture2D<float3> Textures3[1024] : register(t0, space2);
Texture2D<float4> Textures4[1024] : register(t0, space3);

Texture2D<float> GetTextureFloat(uint index) { return Textures[index]; }
Texture2D<float2> GetTextureFloat2(uint index) { return Textures2[index]; }
Texture2D<float3> GetTextureFloat3(uint index) { return Textures3[index]; }
Texture2D<float4> GetTextureFloat4(uint index) { return Textures4[index]; }

#define DEFAULT_BINDLESS_INDEX 2

#endif

float3 ScreenToWorld(float2 screen, float depth)
{
    float4 clip = float4(screen.x * 2.0f - 1.0f, 1.0f - screen.y * 2.0f, depth, 1.0f);
    clip = mul(InverseProjectionViewTransform, clip);
    return clip.xyz / clip.w;
}

float3 ClipToScreen(float4 clip)
{
    float3 uv = clip.xyz / clip.w;
    return float3(uv.x * 0.5f + 0.5f, 0.5f - uv.y * 0.5f, uv.z);
}

float GX(float dot, float r) {
    float k = (1.0f + r) * (1.0f + r) / 8.0f;
    return dot / (dot * (1 - k) + k);
}

float3 ACESFilm(float3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}
