// type vertex

#version 460 core

layout(location = 0) in vec2 position;

void main() {
    gl_Position = vec4(position, 0.0f, 1.0f);
}

// type fragment

#version 460 core

uniform vec2 u_ScreenSize;

uniform sampler2D u_SSDO;

layout(location = 0) out vec4 diffuse;
layout(location = 2) out vec4 combined;

void main() {
    vec2 pos = gl_FragCoord.xy / u_ScreenSize;
    
    vec3 indirect = texture(u_SSDO, pos).xyz;
    
    diffuse = vec4(indirect, 1.0f);
    combined = diffuse;
}
