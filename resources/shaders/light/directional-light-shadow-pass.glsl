// type vertex

#version 460 core

uniform mat4 u_ModelMatrix;

layout(location = 0) in vec3 vertex;

void main() {
    gl_Position = u_ModelMatrix * vec4(vertex, 1.0f);
}

// type geometry

#version 460 core

#define MAX_CASCADES_COUNT 4

uniform float u_Cascades;
uniform mat4 u_ProjectionViewMatries[MAX_CASCADES_COUNT];

layout(triangles) in;
layout(triangle_strip) out;
layout(max_vertices = 12) out;

void main()
{
    for (int i = 0; i < u_Cascades && i <MAX_CASCADES_COUNT; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            gl_Position = u_ProjectionViewMatries[i] * gl_in[j].gl_Position;
            gl_Layer = i;

            EmitVertex();
        }
        
        EndPrimitive();
    }
}