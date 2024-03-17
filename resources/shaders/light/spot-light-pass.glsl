// type vertex

#version 460 core

uniform mat4 u_ProjectionViewMatrix;
uniform mat4 u_ModelMatrix;

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 textureCoordinates;

out vec2 v_TextureCoordinates;

void main() {
    gl_Position = u_ProjectionViewMatrix * u_ModelMatrix * vec4(position, 1.0f);
	v_TextureCoordinates = textureCoordinates;
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

struct Light 
{
    vec3 Color;
    float InnerAngleCos;
    float OuterAngleCos;
    float MaxDistance;
    float Intensity;
    vec3 Position;
    
    vec3 Direction;

    bool IsShadowCasting;
    
    sampler2D ShadowMap;
    vec2 ShadowMapPixelSize;

    mat4 ShadowProjectionViewMatrix;

    sampler2D ShadowSamples;
    vec2 ShadowSamplesPixelSize;
    float ShadowFilterSize;
    float ShadowFilterRadius;
};

uniform vec3 u_ViewPosition;
uniform vec2 u_PixelSize;

uniform float u_FarPlane;

uniform sampler2D u_Albedo;
uniform sampler2D u_Position;
uniform sampler2D u_Normal;
uniform sampler2D u_RoughnessMetalic;

uniform Light u_Light;

in vec2 v_TextureCoordinates;

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
        vec4 projected = u_Light.ShadowProjectionViewMatrix * vec4(position, 1.0f);
        projected /= projected.w;

        projected.xyz = projected.xyz * 0.5f + 0.5f;

        float visible = 0.0f;

        int r = int(u_Light.ShadowFilterSize / 2) + 1;
        int l = -r + (int(u_Light.ShadowFilterSize) % 2 == 0 ? 1 : 0);

        for (int i = l; i < r; ++i)
        {
            for (int j = l; j < r; ++j)
            {
                vec2 samplePosition = texture2D(u_Light.ShadowSamples, vec2(projected.x + i * u_Light.ShadowSamplesPixelSize.x, j * u_Light.ShadowSamplesPixelSize.y)).xy;
                float depth = texture2D(u_Light.ShadowMap, projected.xy + samplePosition * u_Light.ShadowFilterRadius * u_Light.ShadowMapPixelSize).r;
                visible += depth + 0.001f >= projected.z ? 1.0f : 0.0f;
            }
        }

        return visible / (u_Light.ShadowFilterSize * u_Light.ShadowFilterSize);
    }
    else
    {
        return 1.0f;
    }
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
    vec3 l = normalize(position - u_Light.Position);
    float angle = dot(l, u_Light.Direction);

    vec3 pointLightVector = u_Light.Position - position;
    float distanceSqr = dot(pointLightVector, pointLightVector);

    if (u_Light.OuterAngleCos > angle || distanceSqr >= u_Light.MaxDistance * u_Light.MaxDistance)
    {
        return 0.0f;
    }

    float a = 1.0f / (u_Light.InnerAngleCos - u_Light.OuterAngleCos);
    float b = -u_Light.OuterAngleCos * a;
    float softness = clamp((a * angle + b) * (a * angle + b), 0.0f, 1.0f);

    float visibility = GetVisibility(pos, position, light, normal);
    return visibility * softness / (distanceSqr + 1);
}

void main()
{    
    vec2 pos = gl_FragCoord.xy * u_PixelSize;

    vec3 position = texture(u_Position, pos).xyz;
    vec3 normal = texture(u_Normal, pos).xyz;
    
    vec3 view = normalize(u_ViewPosition - position);
    vec3 light = normalize(u_Light.Position - position);

    LightIntensity intensity = GetIntensity(pos, normal, view, light);
    float attenuation = GetAttenuation(pos, position, light, normal);

    diffuse = vec4(intensity.diffuse * attenuation, 0.0f);
    specular = vec4(intensity.specular * attenuation, 0.0f);
    combined = diffuse + specular;
}
