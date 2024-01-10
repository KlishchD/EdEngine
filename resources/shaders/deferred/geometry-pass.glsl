// type vertex

#version 460 core

uniform mat4 u_ProjectionMatrix;

uniform mat4 u_PreviousViewMatrix;
uniform mat4 u_ViewMatrix;

uniform mat4 u_InvProjectionViewMatrix;

uniform vec3 u_ViewPosition;

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

// type fragment

#version 460 core
#extension GL_ARB_bindless_texture : require

struct Material {
    vec3 BaseColor;

    float Roughness;
    float Metalic;
    float Emission;

    sampler2D BaseColorTexture;
    sampler2D NormalTexture;

    bool HasNormalTexture;

    sampler2D RoughnessTexture;
    sampler2D MetalicTexture;
};

uniform Material u_Material;

in vec4 v_CurrentPosition;
in vec4 v_PreviousPosition;

in vec3 v_Position;
in vec3 v_Normal;
in vec4 v_BaseColor;
in vec3 v_TextureCoordinates;
in mat3 v_TBN;

layout(location = 0) out vec4 albedo;
layout(location = 1) out vec4 position;
layout(location = 2) out vec4 normal;
layout(location = 3) out vec4 roughnessMetalic;
layout(location = 4) out vec2 velocity;

void main()
{
    albedo = vec4(u_Material.BaseColor, 1.0f) * v_BaseColor * texture2D(u_Material.BaseColorTexture, v_TextureCoordinates.xy);
    position = vec4(v_Position, 1.0f);

    if (u_Material.HasNormalTexture)
    {
        normal = vec4(v_TBN * (2.0f * texture2D(u_Material.NormalTexture, v_TextureCoordinates.xy).xyz - 1.0f), 1.0f);
    }
    else
    {
        normal = vec4(v_Normal, 1.0f);
    }

    float roughness = u_Material.Roughness * texture2D(u_Material.RoughnessTexture, v_TextureCoordinates.xy).r;
    float metalic = u_Material.Metalic * texture2D(u_Material.MetalicTexture, v_TextureCoordinates.xy).r;
    roughnessMetalic = vec4(roughness, metalic, u_Material.Emission, 1.0f);

    vec2 current = (v_CurrentPosition.xy / v_CurrentPosition.w) * 0.5f + 0.5f;
    vec2 previous = (v_PreviousPosition.xy / v_PreviousPosition.w) * 0.5f + 0.5f;
    velocity = current - previous; // Not subtracting jitter because used the same projection matrix with the same jitter.
}