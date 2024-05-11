// type vertex

#version 460 core

layout(location = 0) in vec3 position;

void main() {
    gl_Position = vec4(position, 0.0f);
}

// type fragment

#version 460 core

#define M_PI 3.1415926535897932384626433832795
#define MAX_POINT_LIGHTS_COUNT 100
#define MAX_SAMPLES_COUNT 64
#define MAX_CASCADES_COUNT 4

struct LightIntensity 
{
    vec3 diffuse;
    vec3 specular;
};

struct Light 
{
    vec3 Position;
    vec3 Color;
    float Intensity;

    vec3 Direction;

    bool IsShadowCasting;

    sampler2DArray ShadowMap;
    float ShadowMapPixelSize;

    float ShadowFilterSize;
    float ShadowFilterRadius;

    float Cascades;
    mat4 ShadowProjectionViewMatries[MAX_CASCADES_COUNT];
};

uniform vec2 u_PixelSize;

uniform vec3 u_ViewPosition;
uniform float u_FarPlane;

uniform sampler2D u_Albedo;
uniform sampler2D u_Position;
uniform sampler2D u_Normal;
uniform sampler2D u_RoughnessMetalic;

uniform mat4 u_ViewMatrix;
uniform mat4 u_ProjectionViewMatrix;

uniform Light u_Light;

layout(location = 0) out vec4 diffuse;
layout(location = 1) out vec4 specular;
layout(location = 2) out vec4 combined;

float GX(float dot, float r) {
    float k = (1.0f + r) * (1.0f + r) / 8.0f;
    return dot / (dot * (1 - k) + k);
}

float GetVisibility(vec2 pos, vec3 position, vec3 light, vec3 normal)
{
    if (u_Light.IsShadowCasting)
    {
        for (int layer = 0; layer < u_Light.Cascades && layer < MAX_CASCADES_COUNT; ++layer)
        {
            vec4 samplePosition = u_Light.ShadowProjectionViewMatries[layer] * vec4(position, 1.0f);
            samplePosition /= samplePosition.w;
            samplePosition = samplePosition * 0.5f + 0.5f;

            if (samplePosition.x >= 0.0f && samplePosition.x <= 1 && samplePosition.y >= 0.0f && samplePosition.y <= 1.0f)
            {
                float visibility = 0.0f;

                float bias = max(0.0025f * (1.0f - dot(normal, light)), 0.0001f);
                
                int r = int(u_Light.ShadowFilterSize / 2) + 1;
                int l = -r + (int(u_Light.ShadowFilterSize) % 2 == 0 ? 1 : 0);

                for (int i = l; i < r; ++i)
                {
                    for (int j = l; j < r; ++j)
                    {
                        float depth = texture(u_Light.ShadowMap, vec3(samplePosition.xy + u_Light.ShadowFilterRadius * u_Light.ShadowMapPixelSize * vec2(i, j) / (r - 1), layer)).r;
        
                        visibility += depth + bias >= samplePosition.z ? 1.0f : 0.0f;
                    }
                }

                return visibility / (u_Light.ShadowFilterSize * u_Light.ShadowFilterSize);
            }
        }

        return 0.0f;
    }

    return 1.0f;
}

LightIntensity GetIntensity(vec2 pos, vec3 normal, vec3 view, vec3 light)
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
    
    vec3 diffuseIntensity = (vec3(1.0f) - F) * albedo / M_PI;
    vec3 specularIntensity = F * G * D / (4.0f * NdotV * NdotL + 0.0001f);

    vec3 baseIntensity = u_Light.Intensity * u_Light.Color * NdotL;

    LightIntensity intensity;
    intensity.diffuse = baseIntensity * diffuseIntensity; 
    intensity.specular = baseIntensity * specularIntensity; 

    return intensity;
}

float GetAttenuation(vec2 pos, vec3 position, vec3 light, vec3 normal)
{
    return GetVisibility(pos, position, light, normal);
}

void main()
{
    vec2 pos = gl_FragCoord.xy * u_PixelSize;
    
    vec3 position = texture(u_Position, pos).xyz;
    vec3 normal = texture(u_Normal, pos).xyz;
    
    vec3 view = normalize(u_ViewPosition - position);
    vec3 light = -u_Light.Direction;

    LightIntensity intensity = GetIntensity(pos, normal, view, light);
    float attenuation = GetAttenuation(pos, position, light, normal);

    diffuse = vec4(intensity.diffuse * attenuation, 0.0f);
    specular = vec4(intensity.specular * attenuation, 0.0f);
    combined = diffuse + specular;
}
