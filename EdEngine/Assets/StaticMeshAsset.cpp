#include "EdEngine.h"
#include "StaticMeshAsset.h"
#include "RenderingContext.h"

void Vertex::Serialize(AssetArchive& archive)
{
    archive & Position;
    archive & Color;
    archive & TextureCoordinates;
    archive & Normal;
    archive & Tangent;
    archive & Bitangent;
}

void StaticMeshAsset::SetMaterial(MaterialAsset* material)
{
    if (Material)
    {
        Material->BaseAsset->UnclaimData();
    }

    Material = material;

    if (Material)
    {
        Material->BaseAsset->ClaimData();
    }
}

void StaticMeshAsset::Serialize(AssetArchive& archive)
{
    archive & VertexCount;
    archive & IndexCount;

    AssetManager::Get().SerializeAsset(archive, &Material, false);
}

bool StaticMeshAsset::ShouldSerializeData(AssetArchive& archive) const
{
    return BaseAsset->ShouldSerializeData(archive);
}

void StaticMeshAsset::SerializeData(AssetArchive& archive)
{
    archive.Serialize<Vertex, false>(&Vertices, VertexCount);
    archive.Serialize<u32, false>(&Indices, IndexCount);

    if (archive.GetMode() == SerializationMode::Read)
    {
        LoadDependencies();
        InitializeRenderBuffers();
    }

    BaseAsset->HasData = true;
}

bool StaticMeshAsset::InitializeRenderBuffers()
{
    if (!Vertices && !Indices) return false;
    
    RenderingContext::Get().UploadModelDeferred(Vertices, sizeof(Vertex), VertexCount, VertexBufferView, Indices, IndexCount, IndexBufferView);

    // 2 Frames of delay just in case we have called this function after we have uploaded render data this frame
    // but before we have conducted deallocations this frame.
    Memory::Get().AddDeallocation<Vertex>(&AssetDataAllocator(), &Vertices, VertexCount, MakeResourceUploadedPredicate(&VertexBufferView));
    Memory::Get().AddDeallocation<u32>(&AssetDataAllocator(), &Indices, IndexCount, MakeResourceUploadedPredicate(&IndexBufferView));

    return true;
}

void StaticMeshAsset::FreeData()
{
    ED_ASSERT(BaseAsset->HasData, "Called free data on an asset without data.");
    ED_ASSERT(!BaseAsset->IsDataClaimed(), "Called free data on an asset with claimed data.");

    RenderingContext::Get().FreeModel(VertexBufferView, IndexBufferView);

    if (Material)
    {
        Material->BaseAsset->UnclaimData();
    }

    BaseAsset->HasData = false;
}

StaticMeshAsset::~StaticMeshAsset()
{
    if (BaseAsset->HasData)
    {
        FreeData();
    }
}

void StaticMeshAsset::LoadDependencies()
{
    if (Material)
    {
        Material->LoadDependencies();
        Material->BaseAsset->ClaimData();
    }
}
