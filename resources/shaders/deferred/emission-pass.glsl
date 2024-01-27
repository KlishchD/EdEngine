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

layout(location = 0) out vec4 diffuse;
layout(location = 1) out vec4 specular;
layout(location = 2) out vec4 combined;

void main() {
    vec2 pos = gl_FragCoord.xy / u_ScreenSize;
    
    vec3 albedo = texture(u_Albedo, pos).xyz;
    
    vec4 roughnessMetalic = texture(u_RoughnessMetalic, pos);
    float emission = roughnessMetalic.z;
    
    diffuse = vec4(albedo * emission, 1.0f);
    specular = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    combined = diffuse;
}
