// type fragment

#version 430 core

uniform float u_Time;
uniform vec3 u_Resolution;

out vec4 color;

void main() {
    color = vec4(1, 1, 1, 1);
}

// type vertex

#version 430 core

uniform mat4 u_Transformation;
uniform mat4 u_CameraMatrix;

layout(location = 0) in vec3 Point;

void main() {
    gl_Position = u_CameraMatrix * u_Transformation * vec4(Point, 1.0f);
}