// type vertex

#version 460 core

uniform mat4 u_ModelMatrix;
uniform mat4 u_ProjectionViewMatrix;

layout(location = 0) in vec3 vertex;

void main() {
    gl_Position = u_ProjectionViewMatrix * u_ModelMatrix * vec4(vertex, 1.0f);
}
