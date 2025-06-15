#include "EdEngine.h"
#include "AssetHelper.h"
#include "Helpers/FilesHelper.h"
#include "Helpers/EntityHelper.h"

u32 AssetHelper::GetAssetTypeFromExtension(ccstr8 extension)
{
	if (strcmp(extension, Files::FullMeshAssetExtension) == 0) return StaticMeshAsset::Type;
    if (strcmp(extension, Files::FullMaterialAssetExtension) == 0) return MaterialAsset::Type;
	if (strcmp(extension, Files::FullTextureAssetExtension) == 0) return TextureAsset::Type;
    if (strcmp(extension, Files::FullPrefabAssetExtension) == 0) return PrefabAsset::Type;
    return InvalidAssetTypeId;
}

bool AssetHelper::IsAssetExtension(ccstr8 extension)
{
	return GetAssetTypeFromExtension(extension) != InvalidAssetTypeId;
}

ccstr8 AssetHelper::GetSaveExtensions(u32 type)
{
    switch (type)
    {
    case TextureAsset::Type:    return Files::FullTextureAssetExtension;
    case MaterialAsset::Type:   return Files::FullMaterialAssetExtension;
    case StaticMeshAsset::Type: return Files::FullMeshAssetExtension;
    case PrefabAsset::Type:     return Files::FullPrefabAssetExtension;
    }

    return "";
}

ContentPath AssetHelper::GetSavePath(const ContentPath& save, u32 type, ccstr8 filename)
{
    ContentPath path(save);

    if (filename)
    {
        constexpr u32 arbitraryCutSize = 64;
        if (arbitraryCutSize == strnlen(filename, arbitraryCutSize))
        {
            filename = Strings::RequestString(filename, arbitraryCutSize, true);
        }

        path.ReplaceFile(filename);
    }

    path.ReplaceExtension(GetSaveExtensions(type));

    return path;
}

ccstr8 AssetHelper::GetAssetNameLable(const Asset& asset)
{
	return (asset.Name + "##" + std::to_string((uptr)&asset)).c_str();
}

u64 AssetHelper::GenerateAssetID()
{
    static std::random_device rd;
    static std::mt19937_64 eng(rd());
    static std::uniform_int_distribution<u64> distribution;
    return distribution(eng) & (~(1 << 0)); // Zero bit is for nullptr when serializing
}

void ComposePrefabSeparate(aiNode* node, ccstr8 name, StaticFeature* parent, const ImportedScene& scene, Array<Entity*>& entities)
{
    if (!node)
    {
        return;
    }

    aiVector3D scale, rotation, translation;
    node->mTransformation.Decompose(scale, rotation, translation);
    Transform transform = Transform(*(glm::vec3*)&translation, glm::quat(*(glm::vec3*)&rotation), *(glm::vec3*)&scale);

    Entity* entity = EntityManager::Get().CreatePrefabEntity(name ? name : node->mName.C_Str());
    EntityManager::Get().AttachStaticFeature(entity, parent, transform);
    entities.Add(entity);

    for (u32 meshIndex = 0; meshIndex < node->mNumMeshes; ++meshIndex)
    {
        u32 importedMeshIndex = node->mMeshes[meshIndex];
        StaticMeshAsset* mesh = scene.StaticMeshes[importedMeshIndex];
        EntityManager::Get().AttachStaticMeshFeature(entity, mesh, mesh->Material);
    }

    for (u32 childIndex = 0; childIndex < node->mNumChildren; ++childIndex)
    {
        ComposePrefabSeparate(node->mChildren[childIndex], nullptr, entity->PositionFeature, scene, entities);
    }
}

ImportedScene AssetHelper::ImportScene(const Path& origin, const ContentPath& save, ccstr8 prefabName, bool createPrefab, bool optimizeScene)
{
    ImportedScene result;

    Assimp::Importer importer;

    u32 flags = aiProcess_ConvertToLeftHanded | aiProcess_FixInfacingNormals | aiProcess_PreTransformVertices;
    if (optimizeScene)
    {
        flags |= aiProcessPreset_TargetRealtime_MaxQuality;
    }
    else
    {
        flags |= aiProcessPreset_TargetRealtime_Fast;
    }

    if (const aiScene* scene = importer.ReadFile(origin.Get(), flags))
    {

        // This is for embedded textures which are not used
        // result.Textures = std::move(ImportMultipleTextures(origin, save, scene->mTextures, scene->mNumTextures));

        TemporaryArray<TextureSpecification> textures;

        result.Materials = std::move(ImportMultipleMaterials(origin, save, scene->mMaterials, scene->mNumMaterials, textures));
        result.Textures = std::move(ImportMultipleTextures(save, textures));
        result.StaticMeshes = std::move(ImportMultipleStaticMeshes(origin, save, scene->mMeshes, scene->mNumMeshes));

        MatchTextures(result.Materials, result.Textures);
        MatchMaterials(result.StaticMeshes, result.Materials);

        for (TextureAsset* asset : result.Textures)
        {
            AssetManager::Get().SaveAsset(asset->BaseAsset);
        }

        for (MaterialAsset* asset : result.Materials)
        {
            AssetManager::Get().SaveAsset(asset->BaseAsset);
        }

        for (StaticMeshAsset* asset : result.StaticMeshes)
        {
            AssetManager::Get().SaveAsset(asset->BaseAsset);
        }

        LoadTextures(result.Textures);
        LoadMaterails(result.Materials);
        LoadStaticMeshes(result.StaticMeshes);

        if (createPrefab)
        {
            Entity* entity = EntityManager::Get().CreatePrefabEntity(prefabName);
            EntityManager::Get().AttachStaticFeature(entity, nullptr);
            
            Array<Entity*> entities;
            entities.Add(entity);

            ComposePrefabSeparate(scene->mRootNode, prefabName, entity->PositionFeature, result, entities);

            Prefab* prefab = EntityManager::Get().CreatePrefab(prefabName, entities);

            result.Prefab = AssetHelper::CreatePrefabAsset(prefab, save, origin);
        }
        else
        {
            result.Prefab = nullptr;
        }

        importer.FreeScene();
    }
    else
    {
        ED_LOG(AssetHelper, err, importer.GetErrorString());
    }

    return result;
}

TextureAsset* AssetHelper::ImportOrLoadTexture(const Path& origin, const ContentPath& save, TextureAsset::MetaData meta, ccstr8 name, PixelFormat format, u32 mips)
{
    TextureAsset* asset = nullptr;

    // TODO: Add separate Path class and add checks to types of path (e.g. path to directory is directory and so on)
    // TODO: Two "GetAsset" (here and in import, improve it)
    ContentPath assetPath = GetSavePath(save, TextureAsset::Type, name);
    if (Asset* base = AssetManager::Get().GetAsset(assetPath))
    {
        AssetManager::Get().LoadAsset(base);
        asset = base->GetData<TextureAsset>();
    }
    else
    {
        asset = ImportSingleTexture(origin, save, meta, name, format, mips);
        asset->InitializeRenderTexture();

        asset->BaseAsset->ClaimData();

        AssetManager::Get().SaveAsset(asset->BaseAsset);
    }

    return asset;
}

TextureAsset* AssetHelper::ImportSingleTexture(const Path& origin, const ContentPath& save, TextureAsset::MetaData meta, ccstr8 name, PixelFormat format, u32 mips)
{
    ContentPath assetPath = GetSavePath(save, TextureAsset::Type, name);
 
    if (Asset* asset = AssetManager::Get().GetAsset(assetPath))
    {
        ED_LOG(AssetHelper, warn, "This save path {} already exists, assuming that is correct previously imported texture.", assetPath.Get());
        return reinterpret_cast<TextureAsset*>(asset->Data);
    }
    
    TextureAsset* asset = AssetManager::Get().CreateTexture(assetPath.GetFileName(), assetPath, origin);

    // Will need to find someway to pass our ptr to importer to avoid copying
    i32 channalsRead, channalsExpected;

    switch (meta)
    {
    case TextureAsset::Count:
    case TextureAsset::Albedo:
    case TextureAsset::Normal: 
        channalsExpected = 4;
        break;
    case TextureAsset::Material:
        channalsExpected = 1;
        break;
    default:
        break;
    }

    u8* image = stbi_load(origin.Get(), reinterpret_cast<i32*>(&asset->Width), reinterpret_cast<i32*>(&asset->Height), &channalsRead, channalsExpected);
    
    ED_ASSERT(image, "Image data cannot be null.");
    ED_ASSERT(channalsRead <= channalsExpected, "Can not proeply handle this difference.");

    asset->RenderTextureType = TextureType::Texture2D;
    // We would like to override texture format to ensure minimal memory usage.
    asset->Format = RenderTypes::CompressType(format, static_cast<u32>(channalsRead));
    asset->Meta = meta;

    if (image)
    {
        asset->Mips = (asset->Width >> mips) > 0 && (asset->Height >> mips) > 0 ? mips : 1;

        u32 imageSize = asset->GetDataSize();
        asset->Data = AssetDataAllocator().Allocate(imageSize);
        memcpy(asset->Data, image, imageSize);

        stbi_image_free(image);
    }
    else
    {
        static u32 white = -1;

        asset->Mips = 1;
        asset->Data = reinterpret_cast<u8*>(&white);
    }

    asset->BaseAsset->HasData = true;

    return asset;
}

TextureAsset* AssetHelper::ImportSingleTexture(const ContentPath& save, const TextureSpecification& specification)
{
    return ImportSingleTexture(specification.Origin, save, specification.Meta, specification.Name, specification.Format, specification.MipsCount);
}

// 
//     PixelFormat selectedFormat = format;
// 
//     if (channals != RenderTypes::GetChannelNumber(format))
//     {
//         ED_LOG(AssetHelper, warn, "Pixel format was different from expected for {} texture.", name);
// 
//         switch (channals)
//         {
//         case 1: selectedFormat = PixelFormat::R8F; break;
//         case 2: selectedFormat = PixelFormat::RG8F; break;
//         case 3: selectedFormat = PixelFormat::RGB8F; break;
//         case 4: selectedFormat = PixelFormat::RGBA8F; break;
//         }
//     }

Array<StaticMeshAsset*> AssetHelper::ImportMultipleStaticMeshes(const Path& origin, const ContentPath& save, aiMesh** meshes, u32 count, const Transform& transform)
{
    Array<StaticMeshAsset*> assets(count);
    for (u32 i = 0; i < count; ++i)
    {
        aiMesh* mesh = meshes[i];

        ccstr8 name = Path(mesh->mName.C_Str()).GetFileName();
        ContentPath filepath = GetSavePath(save, StaticMeshAsset::Type, name);
        StaticMeshAsset* asset = AssetManager::Get().CreateStaticMesh(mesh->mName.C_Str(), filepath, origin);

        // TODO: Data allocated by "new" must allocated by fixed size allocator
        asset->Material = reinterpret_cast<MaterialAsset*>((u64)mesh->mMaterialIndex);
        asset->Vertices = AssetDataAllocator().Allocate<Vertex>(mesh->mNumVertices);
        asset->VertexCount = mesh->mNumVertices;
        asset->Indices = AssetDataAllocator().Allocate<u32>(mesh->mNumFaces * 3);
        asset->IndexCount = mesh->mNumFaces * 3;

        asset->BaseAsset->HasData = true;

        glm::mat4 world = transform.GetMatrix();
        glm::mat3 normal = glm::mat3(transform.GetInversedTransposedMatrix());

        for (u32 i = 0; i < mesh->mNumVertices; ++i)
        {
            Vertex& vertex = asset->Vertices[i];
            vertex = {};

            aiVector3D& position = mesh->mVertices[i];
            vertex.Position = world * glm::vec4(position.x, position.y, position.z, 1.0f);
            vertex.Position *= 0.01; // cm -> m.

            if (mesh->mColors[0])
            {
                vertex.Color = *(glm::vec4*)&mesh->mColors[0][i];
            }

            if (mesh->mTextureCoords[0])
            {
                vertex.TextureCoordinates.x = mesh->mTextureCoords[0][i].x;
                vertex.TextureCoordinates.y = mesh->mTextureCoords[0][i].y;
                vertex.TextureCoordinates.z = mesh->mTextureCoords[0][i].z;
            }

            if (mesh->mNormals)
            {
                vertex.Normal.x = mesh->mNormals[i].x;
                vertex.Normal.y = mesh->mNormals[i].y;
                vertex.Normal.z = mesh->mNormals[i].z;
             
                vertex.Normal = normal * vertex.Normal;
            }

            if (mesh->mTangents)
            {
                vertex.Tangent.x = mesh->mTangents[i].x;
                vertex.Tangent.y = mesh->mTangents[i].y;
                vertex.Tangent.z = mesh->mTangents[i].z;
                
                vertex.Tangent = normal * vertex.Tangent;
            }

            if (mesh->mBitangents)
            {
                vertex.Bitangent.x = mesh->mBitangents[i].x;
                vertex.Bitangent.y = mesh->mBitangents[i].y;
                vertex.Bitangent.z = mesh->mBitangents[i].z;

                vertex.Bitangent = normal * vertex.Bitangent;
            }
        }

        for (u32 face = 0; face < mesh->mNumFaces; ++face)
        {
            for (i32 index = 0; index < 3; ++index)
            {
                asset->Indices[face * 3 + index] = mesh->mFaces[face].mIndices[index];
            }
        }

        assets.Add(asset);
    }

    return assets;
}

template <u32 TextureName, TextureAsset::MetaData Meta>
inline void ParseTextureParameter(aiMaterial* material, TextureAsset*& texture, const Path& origin, TemporaryArray<AssetHelper::TextureSpecification>& textures)
{
    static aiString filepath;
    if (material->GetTexture(static_cast<aiTextureType>(TextureName), 0, &filepath) == aiReturn_SUCCESS)
    {
        AssetHelper::TextureSpecification& specification = textures.Add();
        specification.Origin = origin;
        specification.Origin.ReplaceFile(filepath.C_Str());
        specification.Name = Strings::RequestString(specification.Origin.GetFileName(), specification.Origin.GetSize(), false);
        specification.Format = PixelFormat::RGBA8F;
        specification.Meta = Meta;
        specification.MipsCount = 5;

        u64 index = textures.GetSize();
        texture = reinterpret_cast<TextureAsset*>(index);
    }
}

#define ParseColor(parameter, ...) \
    if (material->Get<aiColor3D>(__VA_ARGS__, *reinterpret_cast<aiColor3D*>(&parameter)) == aiReturn_FAILURE) \
    { \
        parameter = glm::vec3(1.0f); \
    } \

#define ParseScalarParameter(parameter, ...) \
    if (material->Get(__VA_ARGS__, parameter) == aiReturn_FAILURE) \
    { \
        parameter = 0; \
    } \

Array<MaterialAsset*> AssetHelper::ImportMultipleMaterials(const Path& origin, const ContentPath& save, aiMaterial** materials, u32 count, TemporaryArray<TextureSpecification>& textures)
{
    Array<MaterialAsset*> assets(count);
    for (u32 i = 0; i < count; ++i)
    {
        aiMaterial* material = materials[i];

        ccstr8 name = Path(material->GetName().C_Str()).GetFileName();
        ContentPath filepath = GetSavePath(save, MaterialAsset::Type, name);
        MaterialAsset* asset = AssetManager::Get().CreateMaterial(material->GetName().C_Str(), filepath, origin);

        ParseTextureParameter<aiTextureType_DIFFUSE, TextureAsset::Albedo>(material, asset->AlbedoTexture, origin, textures);
        ParseTextureParameter<aiTextureType_BASE_COLOR, TextureAsset::Albedo>(material, asset->AlbedoTexture, origin, textures);

        ParseTextureParameter<aiTextureType_NORMALS, TextureAsset::Normal>(material, asset->NormalTexture, origin, textures);
        ParseTextureParameter<aiTextureType_HEIGHT, TextureAsset::Normal>(material, asset->NormalTexture, origin, textures);
        
        ParseTextureParameter<aiTextureType_SHININESS, TextureAsset::Material>(material, asset->RoughnessTexture, origin, textures);
        ParseTextureParameter<aiTextureType_DIFFUSE_ROUGHNESS, TextureAsset::Material>(material, asset->RoughnessTexture, origin, textures);
        ParseTextureParameter<aiTextureType_METALNESS, TextureAsset::Material>(material, asset->MetalicTexture, origin, textures);

        ParseColor(asset->BaseColor, AI_MATKEY_BASE_COLOR);

        ParseScalarParameter(asset->Roughness, AI_MATKEY_ROUGHNESS_FACTOR);
        ParseScalarParameter(asset->Metalic, AI_MATKEY_METALLIC_FACTOR);

        assets.Add(asset);
    }

    return assets;
}

// Array<TextureAsset*> AssetHelper::ImportMultipleTextures(const Path& origin, const ContentPath& save, aiTexture** textures, u32 count, bool mips)
// {
//     Array<TextureAsset*> assets(count);
//     for (u32 i = 0; i < count; ++i)
//     {
//         aiTexture* texture = textures[i];
//         u32 expectedChannals = (texture->achFormatHint[4] != 0) + (texture->achFormatHint[5] != 0) + (texture->achFormatHint[6] != 0) + (texture->achFormatHint[7] != 0);
// 
//         PixelFormat format;
// 
//         switch (expectedChannals) // TODO: there is a better way to get information from assimp
//         {
//         case 1: format = PixelFormat::R8F; break;
//         case 2: format = PixelFormat::RG8F; break;
//         case 3: format = PixelFormat::RGB8F; break;
//         case 4: format = PixelFormat::RGBA8F; break;
//         }
// 
//         assets.Add(ImportSingleTexture(origin, save, texture->mFilename.C_Str(), format, mips));
//     }
// 
//     return assets;
// }

Array<TextureAsset*> AssetHelper::ImportMultipleTextures(const ContentPath& save, const TemporaryArray<TextureSpecification>& textures)
{
    Array<TextureAsset*> assets(textures.GetSize());
    for (const TextureSpecification& specification : textures)
    {
        assets.Add(ImportSingleTexture(save, specification));
    }

    return assets;
}

void AssetHelper::LoadTextures(const Array<TextureAsset*>& textures)
{
    for (TextureAsset* asset : textures)
    {
        asset->BaseAsset->HasData = true;
        asset->InitializeRenderTexture();
    }
}

void AssetHelper::LoadMaterails(const Array<MaterialAsset*>& materials)
{
    for (MaterialAsset* asset : materials)
    {
        asset->BaseAsset->HasData = true;
    }
}

void AssetHelper::MatchTextures(const Array<MaterialAsset*>& materials, const Array<TextureAsset*>& textures)
{
    for (MaterialAsset* asset : materials)
    {
        asset->BaseAsset->HasData = true;

        if (asset->AlbedoTexture)
        {
            asset->AlbedoTexture = textures[reinterpret_cast<u64>(asset->AlbedoTexture) - 1];
        }

        if (asset->NormalTexture)
        {
            asset->NormalTexture = textures[reinterpret_cast<u64>(asset->NormalTexture) - 1];
        }

        if (asset->RoughnessTexture)
        {
            asset->RoughnessTexture = textures[reinterpret_cast<u64>(asset->RoughnessTexture) - 1];
        }

        if (asset->MetalicTexture)
        {
            asset->MetalicTexture = textures[reinterpret_cast<u64>(asset->MetalicTexture) - 1];
        }
    }
}

void AssetHelper::MatchMaterials(const Array<StaticMeshAsset*>& meshes, const Array<MaterialAsset*>& materials)
{
    for (StaticMeshAsset* asset : meshes)
    {
        asset->Material = materials[reinterpret_cast<u64>(asset->Material)];
    }
}

void AssetHelper::LoadStaticMeshes(const Array<StaticMeshAsset*>& meshes)
{
    for (StaticMeshAsset* asset : meshes)
    {
        asset->BaseAsset->HasData = true;
        asset->InitializeRenderBuffers();
    }
}

template <typename T>
void EnumerateFeature(T* feature, u32 type, const c8* typeName, TemporaryArray<Feature>& features, u32& size)
{
    if (feature->OwnerEntity->HasTemporaryPrefabMarker())
    {
        features.Add({ type, reinterpret_cast<u8*>(feature) });
        size += sizeof(u32) + sizeof(T); // TODO: This is not the best way to estimate feature size, but it is fine for now, but probably needs to be changed later on.
    }
}

template <bool Static, bool Temporary>
void CopyPrefabDataToAsset(u8* buffer, const Array<Entity*>& entities, const Array<Feature, Static, Temporary>& features)
{
    Archive<> archive(static_cast<void*>(buffer), SerializationMode::Write);

    for (Entity* entitiy : entities)
    {
        archive & *entitiy;
    }

    for (const Feature& feature : features)
    {
        archive & feature.Type;
        EntityHelper::SerializeFeature(archive, feature.Type, feature.Data);
    }
}

PrefabAsset* AssetHelper::CreatePrefabAsset(ccstr8 name, const ContentPath& save, const Array<Entity*>& entities)
{
    // TODO: Add validation rules to prefab creation, so that no incorrect combination that could lead to error could be created

    PrefabAsset* asset = AssetManager::Get().CreatePrefab(name, save, "Engine");

    for (Entity* entity : entities)
    {
        entity->AddTemporaryPrefabMarker();
    }

    TemporaryArray<Feature> features;
    u32 size = sizeof(Entity) * entities.GetSize();
    ED_ITERATE_ALL_FEATURES_NAMED_PARAMS(EnumerateFeature, features, size);

    for (Entity* entity : entities)
    {
        entity->RemoveTemporaryPrefabMarker();
    }

    asset->EntitiesCount = entities.GetSize();
    asset->FeaturesCount = features.GetSize();

    asset->BufferSize = size;

    asset->Buffer = AssetDataAllocator().Allocate(asset->BufferSize, 1);

    CopyPrefabDataToAsset(asset->Buffer, entities, features);

    AssetManager::Get().SaveAsset(asset->BaseAsset);

    return asset;
}

PrefabAsset* AssetHelper::CreatePrefabAsset(Prefab* prefab, const ContentPath& save, const Path& origin)
{
    PrefabAsset* asset = AssetManager::Get().CreatePrefab(prefab->Name, save, origin);

    u32 size = sizeof(Entity) * prefab->Entities.GetSize();

    for (Entity* entity : prefab->Entities)
    {
        size += entity->Name.size();
    }

    for (const Feature& feature : prefab->Features)
    {
        size += EntityHelper::GetFeatureTypeSize(feature.Type, feature.Data);
    }

    asset->EntitiesCount = prefab->Entities.GetSize();
    asset->FeaturesCount = prefab->Features.GetSize();

    asset->BufferSize = size;

    asset->Buffer = AssetDataAllocator().Allocate(asset->BufferSize, 1);

    CopyPrefabDataToAsset(asset->Buffer, prefab->Entities, prefab->Features);

    asset->BaseAsset->HasData = true;

    AssetManager::Get().SaveAsset(asset->BaseAsset);

    return asset;

}
