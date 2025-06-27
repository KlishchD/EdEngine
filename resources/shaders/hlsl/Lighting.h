#define NO_BINDLESS 1

#include "Core.h"

float3 BRDF(float3 albedo, float2 material, float3 normal, float3 view, float3 light)
{
    float roughness = material.x;
    float metalic = material.y;

    float3 half = normalize(light + view);

    float NdotV = max(dot(normal, view), 0.0f);
    float HdotV = max(dot(half, view), 0.0f);
    float NdotH = max(dot(normal, half), 0.0f);
    float NdotL = max(dot(normal, light), 0.0f);
    float LdotH = max(dot(light, half), 0.0f);

    float NdotH2 = NdotH * NdotH;
    float r2 = roughness * roughness;

    float3 F0 = lerp(0.04f, albedo, metalic);
    float3 F = F0 + (1.0f - F0) * pow(clamp((1.0f - HdotV), 0.0f, 1.0f), 5.0f);

    float denominator = (NdotH2 * (r2 * r2 - 1.0f) + 1.0f);
    float D = r2 * r2 / (M_PI * denominator * denominator + 0.0001f); // r2 * r2 is Epic stuff ;)

    float G = GX(NdotV, roughness) * GX(NdotL, roughness);

    float3 diffuse = (1.0f - F) * albedo / M_PI;
    float3 specular = F * G * D / (4.0f * NdotV * NdotL + 0.0001f);

    return (specular + diffuse) * NdotL;
}