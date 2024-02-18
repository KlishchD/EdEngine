﻿// type vertex

#version 460 core

uniform mat4 u_ProjectionViewMatrix;
uniform mat4 u_ModelMatrix;

layout(location = 0) in vec3 position;

void main() {
    gl_Position = u_ProjectionViewMatrix * u_ModelMatrix * vec4(position, 1.0f);
}

// type fragment

#version 460 core

#define M_PI 3.1415926535897932384626433832795
#define MAX_POINT_LIGHTS_COUNT 100
#define MAX_SAMPLES_COUNT 64

struct LightIntensity 
{
    vec3 diffuse;
    vec3 specular;
};

struct PointLight 
{
    vec3 Position;
    float Radius;
    vec3 Color;
    float Intensity;
    
    samplerCube ShadowMap;
    bool UseShadowMap;
};

uniform vec2 u_ScreenSize;

uniform vec3 u_ViewPosition;
uniform float u_FarPlane;

uniform sampler2D u_Albedo;
uniform float u_Albedo_ActiveTexturePercentage;

uniform sampler2D u_Position;
uniform float u_Position_ActiveTexturePercentage;

uniform sampler2D u_Normal;
uniform float u_Normal_ActiveTexturePercentage;

uniform sampler2D u_RoughnessMetalic;
uniform float u_RoughnessMetalic_ActiveTexturePercentage;

uniform sampler2D u_RandomSamples;
uniform float u_FilterSize;
uniform float u_ShadowMapPixelSize;

uniform float u_Radius;
uniform float u_SampleCount;
uniform vec3 u_Samples[MAX_SAMPLES_COUNT];

uniform float u_DriectLightStrength = 1.0f;

uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjectionViewMatrix;

uniform PointLight u_PointLight;

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);   

layout(location = 0) out vec4 diffuse;
layout(location = 1) out vec4 specular;
layout(location = 2) out vec4 combined;

float GX(float dot, float r) {
    float k = (1.0f + r) * (1.0f + r) / 8.0f;
    return dot / (dot * (1 - k) + k);
}

float GetVisibility(vec2 pos, vec3 position, vec3 light, vec3 normal)
{
    if (u_PointLight.UseShadowMap)
    {
        float NdotL = max(dot(normal, light), 0.0f);

        float distance = length(u_PointLight.Position - position); // TODO: use sqr

        float bias = max(5.0f * (1.0f - NdotL), 1.0f);
        float shadowIntensity = 0;

        float offset = 0.01f;
        float delta = (2.0f * offset) / u_FilterSize;
    
        for (float i = -offset; i < offset; i += delta)
        {
            for (float j = -offset; j < offset; j += delta)
            {
                for (float k = -offset; k < offset; k += delta)
                {
                    float nearest = texture(u_PointLight.ShadowMap, -light.xyz + vec3(i, j, k)).r * u_FarPlane;
                    if (nearest + bias < distance)
                    {
                        shadowIntensity += 1;
                    }
                }
            }
        }
    
        shadowIntensity /= u_FilterSize * u_FilterSize * u_FilterSize;

        return 1.0f - shadowIntensity;
    }
    else
    {
        return 1.0f;
    }
}

LightIntensity GetIntensity(vec2 pos, vec3 normal, vec3 view, vec3 light)
{
    vec3 albedo = texture(u_Albedo, pos * u_Albedo_ActiveTexturePercentage).xyz;

    vec4 roughnessMetalic = texture(u_RoughnessMetalic, pos * u_RoughnessMetalic_ActiveTexturePercentage);
    float roughness = roughnessMetalic.x;
    float metalic = roughnessMetalic.y;

    vec3 h = normalize(light + view);

    float NdotV = max(dot(normal, view), 0.0f);
    float HdotV = max(dot(h, view), 0.0f);
    float NdotH = max(dot(normal, h), 0.0f);
    float NdotL = max(dot(normal, light), 0.0f);
    float LdotH = max(dot(light, h), 0.0f);
    
    float NdotH2 = NdotH * NdotH;
    float r2 = roughness * roughness;

    vec3 F0 = mix(vec3(0.04f), albedo, metalic);

    vec3 F = F0 + (vec3(1.0f) - F0) * pow(clamp((1.0f - HdotV), 0.0f, 1.0f), 5.0f);
    
    float denominator = (NdotH2 * (r2 * r2 - 1.0f) + 1.0f);
    float D = r2 * r2 / (M_PI * denominator * denominator + 0.0001f); // r2 * r2 is Epic stuff ;)
    
    float G = GX(NdotV, roughness) * GX(NdotL, roughness);
    
    vec3 diffuseIntensity = (vec3(1.0f) - F) * albedo / M_PI;
    vec3 specularIntensity = F * G * D / (4.0f * NdotV * NdotL + 0.0001f);

    vec3 baseIntensity = u_PointLight.Intensity * u_PointLight.Color * NdotL;

    LightIntensity intensity;
    intensity.diffuse = baseIntensity * diffuseIntensity; 
    intensity.specular = baseIntensity * specularIntensity; 

    return intensity;
}

float GetAttenuation(vec2 pos, vec3 position, vec3 light, vec3 normal)
{
    float visibility = GetVisibility(pos, position, light, normal);

    vec3 pointLightVector = u_PointLight.Position - position;
    float distanceSqr = dot(pointLightVector, pointLightVector);
    float radiusSqr = u_PointLight.Radius * u_PointLight.Radius;

    float A = distanceSqr / radiusSqr;
    float B = clamp(1 - A * A, 0.0f, 1.0f);
    float numerator = B * B;

    float denominator = distanceSqr + 1.0f;

    return visibility * numerator / denominator;
}

void main()
{
    vec2 pos = gl_FragCoord.xy / u_ScreenSize;
    
    vec3 position = texture(u_Position, pos * u_Position_ActiveTexturePercentage).xyz;
    vec3 normal = texture(u_Normal, pos * u_Normal_ActiveTexturePercentage).xyz;
    
    vec3 view = normalize(u_ViewPosition - position);
    vec3 light = normalize(u_PointLight.Position - position);

    LightIntensity intensity = GetIntensity(pos, normal, view, light);
    float attenuation = GetAttenuation(pos, position, light, normal);

    vec3 direct = vec3(0.0f);
    /* TODO: Check it again later :)
	for (int i = 0; i < u_SampleCount && i < MAX_SAMPLES_COUNT; ++i)
	{
	    vec3 samplePosition = position + u_Samples[i] * u_Radius;

	    vec4 screen = u_ProjectionViewMatrix * vec4(samplePosition, 1.0f);
	    screen.xyz /= screen.w;
	    screen = screen * 0.5f + 0.5f;

        if (screen.x <= 1.0f && screen.y <= 1.0f && screen.x >= 0.0f && screen.y >= 0.0f)
        {
	        vec3 screenPosition = texture2D(u_Position, screen.xy).xyz;

            float Depth = (u_ViewMatrix * vec4(position, 1.0f)).z;
            float screenDepth = (u_ViewMatrix * vec4(screenPosition, 1.0f)).z;
            float sampleDepth = (u_ViewMatrix * vec4(samplePosition, 1.0f)).z;

	        float visibility = screenDepth >= sampleDepth ? 1.0f : 0.0f;

	        vec3 transmissionDirection = normalize(samplePosition - position);

	        float recieverTerm = clamp(dot(transmissionDirection, normal), 0.0f, 1.0f);

            float rangeCheck = smoothstep(0.0f, 1.0f, u_Radius / abs(Depth - screenDepth));

            LightIntensity intensity = GetIntensity(screen.xy, normal, view, transmissionDirection);

	        direct += vec3(visibility * intensity.diffuse / (u_PointLight.Intensity * u_SampleCount));
        }
    }
    */

    diffuse = vec4(intensity.diffuse * attenuation + direct * u_DriectLightStrength, 0.0f);
    specular = vec4(intensity.specular * attenuation, 0.0f);
    combined = diffuse + specular;
}