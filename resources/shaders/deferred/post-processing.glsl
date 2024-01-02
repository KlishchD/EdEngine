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

uniform sampler2D u_Color;

uniform sampler2D u_Bloom;
uniform float u_BloomStrength;
uniform float u_BloomIntensity;

uniform float u_Gamma;

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
    
    vec3 color = texture(u_Color, pos).xyz;

    vec3 bloom = texture(u_Bloom, pos).xyz * u_BloomIntensity;
    if (u_BloomStrength == 1.0f)
    {
        color += bloom;
    }
    else
    {
        color = mix(color, bloom, u_BloomStrength); // vec3(1.0f, 0.0f, 0.0f); // 
    }

    // HDR tone-mapping
    //totalColor = totalColor / (totalColor + vec3(1.0f));
    color = ACESFilm(color);
    
    // gamma correction
    color = pow(color, vec3(1.0f / u_Gamma));
    
    result = vec4(color, 1.0f);
}
