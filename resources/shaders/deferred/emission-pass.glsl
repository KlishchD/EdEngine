// type vertex

#version 460 core

uniform mat4 u_ProjectionViewMatrix;
uniform mat4 u_ModelMatrix;

layout(location = 0) in vec2 position;

void main() {
    gl_Position = vec4(position, 0.0f, 1.0f);
}

// type fragment

#version 460 core

uniform vec2 u_ScreenSize;

uniform sampler2D u_Albedo;
uniform sampler2D u_RoughnessMetalic;

layout(location = 0) out vec4 color;

void main() {
    vec2 pos = gl_FragCoord.xy / u_ScreenSize;
    
    vec3 albedo = texture(u_Albedo, pos).xyz;
    
    vec4 roughnessMetalic = texture(u_RoughnessMetalic, pos);
    float emission = roughnessMetalic.z;
    
    color = vec4(albedo * emission + albedo * 0.1f, 1.0f);
}
