#pragma once

class Buffer;
struct Asset;
struct MaterialAsset;

struct Vertex
{
    glm::vec3 Position = glm::vec3(0.0f);
    glm::vec4 Color = glm::vec4(1.0f);
    glm::vec3 TextureCoordinates = glm::vec3(0.0f);
    glm::vec3 Normal = glm::vec3(0.0);
    glm::vec3 Tangent = glm::vec3(0.0f);
    glm::vec3 Bitangent = glm::vec3(0.0f);

    void Serialize(AssetArchive& archive);
};

struct StaticMeshAsset
{
    static inline constexpr i32 Type = 1;

    Asset* BaseAsset;

    ResourceView VertexBufferView;
    ResourceView IndexBufferView;

    Vertex* Vertices;
    u32* Indices;

    u32 VertexCount;
    u32 IndexCount;

    MaterialAsset* Material;

    void SetMaterial(MaterialAsset* material);

    void Serialize(AssetArchive& archive);

    bool ShouldSerializeData(AssetArchive& archive) const;
    void SerializeData(AssetArchive& archive);
    bool InitializeRenderBuffers();

    void LoadDependencies();

    void FreeData();

    ~StaticMeshAsset();
};