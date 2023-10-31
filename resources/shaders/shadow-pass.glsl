// type vertex

#version 460 core

uniform mat4 u_ModelMatrix;

layout(location = 0) in vec3 vertex;

void main() {
    gl_Position = u_ModelMatrix * vec4(vertex, 1.0f);
}

// type geometry

#version 460 core

uniform mat4 u_ViewProjection[6];

out vec3 v_Position;

layout(triangles) in;
layout(triangle_strip) out;
layout(max_vertices = 18) out;

void main() {
    for (int i = 0; i < 6; ++i) {
        gl_Layer = i;
        for (int j = 0; j < 3; ++j) {
            gl_Position = u_ViewProjection[i] * gl_in[j].gl_Position;
            v_Position = gl_in[j].gl_Position.xyz;
            EmitVertex();
        }
        EndPrimitive();
    }
}

// type fragment

#version 460 core

uniform vec3 u_ViewPosition;

in vec3 v_Position;
uniform float u_FarPlane;

void main() {
    gl_FragDepth = length(v_Position - u_ViewPosition) / u_FarPlane;
}