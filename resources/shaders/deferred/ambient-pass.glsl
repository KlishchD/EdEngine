// type vertex

#version 460 core

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 textureCoordinates;

out vec2 v_TextureCoordinates;

void main() {
    gl_Position = vec4(position, 0.0f, 1.0f);
	v_TextureCoordinates = textureCoordinates;
}

// type fragment

#version 460 core

in vec2 v_TextureCoordinates;

uniform sampler2D u_Albedo;
uniform sampler2D u_AmbientOcclusion;

layout(location = 0) out vec4 diffuse;
layout(location = 2) out vec4 combined;

void main() {
    vec3 albedo = texture(u_Albedo, v_TextureCoordinates).xyz;
    float AO = texture2D(u_AmbientOcclusion, v_TextureCoordinates).x;
    
    diffuse = vec4(0.1f * AO * albedo, 1.0f);
    combined = diffuse;
}
