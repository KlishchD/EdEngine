#version 450

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
    albedo = vec4(u_Material.BaseColor, 1.0f) * v_BaseColor * texture(u_Material.BaseColorTexture, v_TextureCoordinates.xy);
    position = vec4(v_Position, 1.0f);

    if (u_Material.HasNormalTexture)
    {
        normal = vec4(v_TBN * (2.0f * texture(u_Material.NormalTexture, v_TextureCoordinates.xy).xyz - 1.0f), 1.0f);
    }
    else
    {
        normal = vec4(v_Normal, 1.0f);
    }

    float roughness = u_Material.Roughness * texture(u_Material.RoughnessTexture, v_TextureCoordinates.xy).r;
    float metalic = u_Material.Metalic * texture(u_Material.MetalicTexture, v_TextureCoordinates.xy).r;
    roughnessMetalic = vec4(roughness, metalic, u_Material.Emission, 1.0f);

    vec2 current = (v_CurrentPosition.xy / v_CurrentPosition.w) * 0.5f + 0.5f;
    vec2 previous = (v_PreviousPosition.xy / v_PreviousPosition.w) * 0.5f + 0.5f;
    velocity = current - previous; // Not subtracting jitter because used the same projection matrix with the same jitter.
}