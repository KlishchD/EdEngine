// type vertex

#version 460 core

uniform mat4 u_ProjectionViewMatrix;
uniform mat4 u_ModelMatrix;

layout(location = 0) in vec3 vertex;

void main() 
{       
    gl_Position = u_ProjectionViewMatrix * u_ModelMatrix * vec4(vertex, 1.0f);
}

// type fragment

#version 460 core

out vec4 color;

void main() 
{
    color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}
