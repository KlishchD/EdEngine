// type fragment

#version 430 core

uniform sampler2D u_Texture;
uniform vec3 u_Color;

in vec2 v_TexCoords;

out vec4 color;

void main() {   
    color = vec4(u_Color, 1.0f) * vec4(1.0f, 1.0f, 1.0f, texture(u_Texture, v_TexCoords).r);
}

// type vertex

#version 430 core

uniform mat4 u_CameraMatrix;

layout(location = 0) in vec3 Point;
layout(location = 1) in vec2 TexCoords;

out vec2 v_TexCoords;

void main() {
    gl_Position = u_CameraMatrix * vec4(Point, 1.0f);
    v_TexCoords = TexCoords;
}