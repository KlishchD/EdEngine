// type vertex

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

struct PointLight {
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
uniform sampler2D u_Position;
uniform sampler2D u_Normal;
uniform sampler2D u_RoughnessMetalic;

uniform sampler2D u_RandomSamples;
uniform float u_FilterSize;
uniform float u_ShadowMapPixelSize;

uniform PointLight u_PointLight;

vec3 sampleOffsetDirections[20] = vec3[]
(
   vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1), 
   vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
   vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
   vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
   vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
);   

layout(location = 0) out vec4 color;

float GX(float dot, float r) {
    float k = (1.0f + r) * (1.0f + r) / 8.0f;
    return dot / (dot * (1 - k) + k);
}

float GetVisibility(vec2 pos, vec3 light, vec3 normal, vec3 position)
{
    if (u_PointLight.UseShadowMap)
    {
        float NdotL = max(dot(normal, light), 0.0f);

        float distance = length(u_PointLight.Position - position);

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

vec3 GetIntensity(vec2 pos, vec3 normal, vec3 view, vec3 light)
{
    vec3 albedo = texture(u_Albedo, pos).xyz;

    vec4 roughnessMetalic = texture(u_RoughnessMetalic, pos);
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
    
    vec3 specular = F * G * D / (4.0f * NdotV * NdotL + 0.0001f);
    vec3 diffuse = (vec3(1.0f) - F) * albedo / M_PI;

    return (diffuse + specular) * NdotL;
}

vec3 GetAttenuation(vec2 pos, vec3 position)
{
    float distance = length(u_PointLight.Position - position);
    return u_PointLight.Intensity * u_PointLight.Color / (distance * distance);
}

void main()
{
    vec2 pos = gl_FragCoord.xy / u_ScreenSize;
    
    vec3 position = texture(u_Position, pos).xyz;
    vec3 normal = texture(u_Normal, pos).xyz;
    
    vec3 view = normalize(u_ViewPosition - position);
    vec3 light = normalize(u_PointLight.Position - position);

    float visibility = GetVisibility(pos, light, normal, position);
    vec3 intensity = GetIntensity(pos, normal, view, light);
    vec3 attenuation = GetAttenuation(pos, position);

    color = vec4(visibility * intensity * attenuation, 1.0f);
}
