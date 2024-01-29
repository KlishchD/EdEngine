// type vertex

#version 460 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 textureCoordinates;

out vec2 v_TextureCoordinates;

void main() 
{
	gl_Position = vec4(position.x, position.y, 0.0f, 1.0f);
	v_TextureCoordinates = textureCoordinates;
}

// type fragment

#version 460 core

#define MAX_SAMPLES_COUNT 64
#define PI 3.1415926535897932384626433832795

in vec2 v_TextureCoordinates;

uniform sampler2D u_Albedo;
uniform sampler2D u_RoughnessMetalic;
uniform sampler2D u_Diffuse;
uniform sampler2D u_Position;
uniform sampler2D u_Normal;

uniform float u_IndirectStrength = 1.0f;

uniform float u_SampleCount;
uniform vec3 u_Samples[MAX_SAMPLES_COUNT];

uniform mat4 u_ViewMatrix;
uniform mat4 u_NormalMatrix;
uniform mat4 u_ProjectionMatrix;
uniform mat4 u_ProjectionViewMatrix;

uniform vec3 u_ViewPosition;

uniform float u_Radius;

out vec4 color;

struct LightIntensity 
{
    vec3 diffuse;
    vec3 specular;
};

vec3 ToView(vec3 vec)
{
	return (u_ViewMatrix * vec4(vec, 1.0f)).xyz;
}

vec3 ToViewNormal(vec3 vec)
{
	return normalize((u_NormalMatrix * vec4(vec, 1.0f)).xyz);
}

float GX(float dot, float r) {
    float k = (1.0f + r) * (1.0f + r) / 8.0f;
    return dot / (dot * (1 - k) + k);
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
    float D = r2 * r2 / (PI * denominator * denominator + 0.0001f);
    
    float G = GX(NdotV, roughness) * GX(NdotL, roughness);
    
    vec3 diffuseIntensity = (vec3(1.0f) - F) * albedo / PI;
    vec3 specularIntensity = F * G * D / (4.0f * NdotV * NdotL + 0.0001f);

    LightIntensity intensity;
    intensity.diffuse = diffuseIntensity; 
    intensity.specular = specularIntensity; 

    return intensity;
}

void main() 
{
	vec3 position = ToView(texture2D(u_Position, v_TextureCoordinates).xyz);
	vec3 normal = ToViewNormal(texture2D(u_Normal, v_TextureCoordinates).xyz);

    vec3 view = ToView(u_ViewPosition) - position;

	vec3 direct = vec3(0.0f);
	vec3 indirect = vec3(0.0f);

	float A = PI * u_Radius * u_Radius / u_SampleCount;

	for (int i = 0; i < u_SampleCount && i < MAX_SAMPLES_COUNT; ++i) 
	{
		vec3 samplePosition = position + u_Samples[i] * u_Radius;

		vec4 screen = u_ProjectionMatrix * vec4(samplePosition, 1.0f);
		screen.xyz /= screen.w;
		screen = screen * 0.5f + 0.5f;

        if (screen.x <= 1.0f && screen.y <= 1.0f && screen.x >= 0.0f && screen.y >= 0.0f)
        {
            vec3 screenAlbedo = texture2D(u_Albedo, screen.xy).rgb;
		    vec3 screenDiffuse = texture2D(u_Diffuse, screen.xy).rgb;
		    vec3 screenPosition = ToView(texture2D(u_Position, screen.xy).xyz);
		    vec3 screenNormal = ToViewNormal(texture2D(u_Normal, screen.xy).xyz);

		    float visibility = screenPosition.z >= samplePosition.z ? 1.0f : 0.0f;

		    vec3 transmissionVector = position - screenPosition;
		    vec3 transmissionDirection = normalize(transmissionVector);

		    float recieverTerm = clamp(dot(transmissionDirection, -normal), 0.0f, 1.0f);
		    float senderTerm = clamp(dot(transmissionDirection, screenNormal), 0.0f, 1.0f);

		    float F = A * senderTerm * recieverTerm;
		    float attenuation = clamp(1.0f / dot(transmissionVector, transmissionVector), 0.0f, 1.0f);

            LightIntensity intensity = GetIntensity(v_TextureCoordinates, normal, view, -transmissionDirection);

		    direct += visibility * screenAlbedo * (intensity.specular + intensity.diffuse) * recieverTerm * recieverTerm;
		    indirect += (1 - visibility) * screenDiffuse * (intensity.specular + intensity.diffuse) * attenuation * F;
        }
    }

	indirect *= 2.0f * PI;

	color = vec4(u_IndirectStrength * indirect, 1.0f);
}
