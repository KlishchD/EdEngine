// type vertex

#version 460 core

layout(location = 0) in vec2 position;

void main() {
    gl_Position = vec4(position, 0.0f, 1.0f);
}

// type fragment

#version 460 core

#define M_PI 3.1415926535897932384626433832795
#define MAX_POINT_LIGHTS_COUNT 100

uniform vec2 u_ScreenSize;

uniform sampler2D u_Albedo;
uniform sampler2D u_RoughnessMetalic;
uniform sampler2D u_Light;
uniform sampler2D u_AmbientOcclusion;

layout(location = 0) out vec4 result;

vec3 ACESFilm(vec3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}

void main() {
    vec2 pos = gl_FragCoord.xy / u_ScreenSize;
    
    vec3 albedo = texture(u_Albedo, pos).xyz;
    vec4 roughnessMetalic = texture(u_RoughnessMetalic, pos);
    
    vec3 color;

    if (roughnessMetalic.a == 1.0f)
    {
        float ambientOcclusion = texture2D(u_AmbientOcclusion, pos).x;
        color = 0.1f * albedo * ambientOcclusion + texture(u_Light, pos).xyz;
    }
    else
    {
        color = albedo;
    }
    
    result = vec4(color, 1.0f);
}
