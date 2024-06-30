#version 450

uniform mat4 u_ProjectionMatrix;

uniform mat4 u_PreviousViewMatrix;
uniform mat4 u_ViewMatrix;
 
uniform mat4 u_PreviousModelMatrix;
uniform mat4 u_ModelMatrix;
uniform mat3 u_NormalMatrix;

uniform bool u_PerformNormalMapping;

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec4 BaseColor;
layout(location = 2) in vec3 textureCoordinates;
layout(location = 3) in vec3 normal;
layout(location = 4) in vec3 tangent;
layout(location = 5) in vec3 bitangent;

out vec4 v_CurrentPosition;
out vec4 v_PreviousPosition;

out vec3 v_Position;
out vec3 v_Normal;
out vec4 v_BaseColor;
out vec3 v_TextureCoordinates;
out mat3 v_TBN;

void main()
{
    v_CurrentPosition = u_ProjectionMatrix * u_ViewMatrix * u_ModelMatrix * vec4(vertex, 1.0f);
    v_PreviousPosition = u_ProjectionMatrix * u_PreviousViewMatrix * u_PreviousModelMatrix * vec4(vertex, 1.0f);

    v_Position = (u_ModelMatrix * vec4(vertex, 1.0f)).xyz;
    v_Normal = normalize(u_NormalMatrix * normal);
    v_BaseColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    v_TextureCoordinates = textureCoordinates;

    if (u_PerformNormalMapping) {
        vec3 T = normalize(u_NormalMatrix * tangent);
        vec3 N = normalize(u_NormalMatrix * normal);
        vec3 B = normalize(cross(T, N)); // TODO: maybe add switch or smth ;)

        v_TBN = mat3(T, B, N);
    }
    else
    {
        v_TBN = mat3(1);
    }

    gl_Position = v_CurrentPosition;
}