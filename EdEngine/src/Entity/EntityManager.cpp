#include "EdEngine.h"
#include "EntityManager.h"
#include "Helpers/EntityHelper.h"
#include "Helpers/RenderingHelper.h"
#include "Helpers/FilesHelper.h"
#include "RenderScene.h"

EntityManager::EntityManager() : m_LoadedScenePath(Files::GetDefaultScenePath()), m_Entites("Entities"), m_StaticFeatures("StaticFeatures"), m_MovableFeatures("MovableFeatures"), m_StaticMeshFeatures("StaticMeshFeatures"),
    m_PointLightFeatures("PointLightFeatures"), m_SpotLightsFeatures("SpotLightFeatures"), m_DirectionalLightsFeatures("DirectionalLightsFeature"), m_CamerasFeatures("CameraFeatures"), m_Prefabs("Prefabs")
{
    ED_ASSERT(!s_Manager, "There could only be one entity manager."); // TODO: could be use full to make multiple of the for some kind of level division or some kind of world parition system.
    s_Manager = this;
}

void EntityManager::Initialize()
{
    // Ensure empty scene always exists.
    SaveScene();
}

void EntityManager::Deinitialize()
{
    
}

void EntityManager::Update(f32 DeltaSeconds)
{
    UpdateMovableFeatures();
}

template <typename T, uint32_t Count>
void SerializeItems(Archive<>& archive, PoolAllocator<T, Count>& items)
{
    if (archive.GetMode() == SerializationMode::Write)
    {
        // TODO: This seems to be over kill, so it would be better to move prefabs to different pool.
        u32 count = 0;

        for (T* object : items)
        {
            if constexpr (std::is_base_of<Entity, T>())
            {
                if (!object->IsEditorPrefab())
                {
                    ++count;
                }
            }
            else
            {
                if (!object->OwnerEntity->IsEditorPrefab())
                {
                    ++count;
                }
            }
        }

        archive & count;

        for (T* object : items)
        {
            if constexpr (std::is_base_of<Entity, T>())
            {
                if (!object->IsEditorPrefab())
                {
                    archive & (*object);
                }
            }
            else
            {
                if (!object->OwnerEntity->IsEditorPrefab())
                {
                    archive & (*object);
                }
            }
        }
    }
    else
    {
        u32 size;
        archive & size;

        for (u32 i = 0; i < size; ++i)
        {
            T* object = items.Allocate();
            archive & (*object);
        }
    }
}

void EntityManager::Serialize(Archive<>& archive)
{
    SerializeItems(archive, m_Entites);
    SerializeItems(archive, m_StaticFeatures);
    SerializeItems(archive, m_MovableFeatures);
    SerializeItems(archive, m_StaticMeshFeatures);
    SerializeItems(archive, m_PointLightFeatures);
    SerializeItems(archive, m_SpotLightsFeatures);
    SerializeItems(archive, m_DirectionalLightsFeatures);
    SerializeItems(archive, m_CamerasFeatures);

    ED_ITERATE_ALL_FEATURES(ResolveAssetDependenciesOnLoad)

    if (archive.GetMode() == SerializationMode::Read)
    {
        ResolveDependencies();
    }
}

void EntityManager::ResolveDependencies()
{
    for (StaticFeature* feature : m_StaticFeatures)
    {
        if (feature->ParentPositionFeature)
        {
            feature->ParentPositionFeature = reinterpret_cast<Entity*>(feature->ParentPositionFeature)->PositionFeature;
        }
    }

    for (MovableFeature* feature : m_MovableFeatures)
    {
        if (feature->ParentPositionFeature)
        {
            feature->ParentPositionFeature = reinterpret_cast<Entity*>(feature->ParentPositionFeature)->PositionFeature;
        }
    }
}

void EntityManager::ResolveDependencies(Prefab* prefab)
{
    for (const Feature& feature : prefab->Features)
    {
        if (feature.Type == StaticFeature::Type || feature.Type == MovableFeature::Type)
        {
            StaticFeature* staticFeature = static_cast<StaticFeature*>(feature.Data);
            staticFeature->ForcedUpdate();

            if (staticFeature->ParentPositionFeature)
            {
                staticFeature->ParentPositionFeature = reinterpret_cast<Entity*>(staticFeature->ParentPositionFeature)->PositionFeature;
            }
        }
    }
}

void EntityManager::ResolveAssetDependenciesOnLoad(void* feature, u32 type)
{
    if (!feature) return;

    switch (type)
    {
    case StaticMeshFeature::Type:
    {
        StaticMeshFeature* mesh = static_cast<StaticMeshFeature*>(feature);
        mesh->LoadAssets();
    }
    break;
    default:
        break;
    }
}

void EntityManager::UnloadScene()
{
    m_Entites.Clear();
    m_StaticFeatures.Clear();
    m_MovableFeatures.Clear();
    m_StaticMeshFeatures.Clear();
    m_PointLightFeatures.Clear();
    m_SpotLightsFeatures.Clear();
    m_DirectionalLightsFeatures.Clear();
    m_CamerasFeatures.Clear();

    m_SceneLoaded = false;
}

void EntityManager::SaveScene(const ContentPath& path)
{
    Archive archive(path, SerializationMode::Write);
    
    Serialize(archive);

    m_LoadedScenePath = path;
}

void EntityManager::SaveScene()
{
    SaveScene(m_LoadedScenePath);
}

void EntityManager::LoadScene(const ContentPath& path)
{
    if (m_SceneLoaded)
    {
        UnloadScene();
    }

    Archive archive(path, SerializationMode::Read);
    Serialize(archive);

    m_LoadedScenePath = path;
    m_SceneLoaded = true;
}

void EntityManager::LoadScene()
{
    LoadScene(m_LoadedScenePath);
}

template <typename T>
void AddFeatureToPrefab(T* feature, u32 type, Prefab* prefab)
{
    prefab->Features.Add({ type, (void*)feature });
}

Prefab* EntityManager::CreatePrefab(ccstr8 name, const Array<Entity*>& entities)
{
    Prefab* prefab = m_Prefabs.Allocate();

    u32 size = strnlen(name, MaxFeatureNameSize);
    strncpy(prefab->Name, name, size);
    prefab->Entities = entities;

    ED_ITERATE_ALL_FEATURES_PARAMS(AddFeatureToPrefab, prefab);

    return prefab;
}

Prefab* EntityManager::CreatePrefab(PrefabAsset* asset)
{
    Prefab* prefab = m_Prefabs.Allocate();

    const std::string& name = asset->BaseAsset->Name;
    strncpy(prefab->Name, name.c_str(), std::min<u32>(name.size(), MaxFeatureNameSize));

    Archive archive(asset->Buffer, SerializationMode::Read);

    for (u32 i = 0; i < asset->EntitiesCount; ++i)
    {
        Entity* entity = m_Entites.Allocate();
        archive & (*entity);

        prefab->Entities.Add(entity);
    }

    for (u32 i = 0; i < asset->FeaturesCount; ++i)
    {
        u32 type;
        archive & type;

        void* feature = AllocateFeature(type);
        EntityHelper::SerializeFeature(archive, type, feature);

        prefab->Features.Add({ type, feature });

        ResolveAssetDependenciesOnLoad(feature, type);
    }

    ResolveDependencies(prefab);

    return prefab;
}

Prefab* EntityManager::CreatePrefab(ccstr8 name)
{
    Prefab* prefab = m_Prefabs.Allocate();
    strncpy(prefab->Name, name, strnlen(name, MaxFeatureNameSize));
    return prefab;
}

Prefab* EntityManager::CreatePrefab(ccstr8 name, PrefabAsset* asset)
{
    Prefab* prefab = CreatePrefab(asset);
    strncpy(prefab->Name, name, strnlen(name, MaxFeatureNameSize));

    for (Entity* entity : prefab->Entities)
    {
        entity->Name.append(".");
        entity->Name.append(name);
    }

    return prefab;
}

void EntityManager::UpdateMovableFeatures()
{
    u32 UpdatedCount = 0;

    TemporaryArray<StaticFeature*> statics;
    for (StaticFeature* feature : m_StaticFeatures)
    {
        statics.Clear();
        
        bool update = false;

        while (feature)
        {
            update |= feature->ForcedUpdate() | feature->IsUpdated();
            
            if (feature->IsUpdated())
            {
                break;
            }

            statics.Add(feature);
            feature = feature->ParentPositionFeature;
        }

        while (statics.GetSize() && update)
        {
            StaticFeature* current = statics.GetAndRemoveLast();
            if (StaticFeature* parent = current->ParentPositionFeature)
            {
                current->UpdateWorldPosition(parent->WorldTransformation);
            }
            else
            {
                current->MarkUpdated();
            }
        }
    }

    for (StaticFeature* feature : m_StaticFeatures)
    {
        feature->ClearState();
    }

    TemporaryArray<MovableFeature*> movables;
    for (MovableFeature* feature : m_MovableFeatures)
    {
        movables.Clear();

        while (!feature->IsParentStatic && feature->ParentPositionFeature)
        {
            feature = feature->MovableParentPositionFeature;
            if (!feature->WasUpdated())
            {
                movables.Add(feature);
            }
        }

        while (movables.GetSize())
        {
            MovableFeature* current = movables.GetAndRemoveLast();
            if (StaticFeature* parent = current->ParentPositionFeature)
            {
                current->UpdateWorldPosition(parent->WorldTransformation);
            }
        }
    }

    for (MovableFeature* feature : m_MovableFeatures)
    {
        feature->ClearState();
    }
}

Entity* EntityManager::CreateEntity(ccstr8 name)
{
    Entity* entity = m_Entites.Allocate();
    entity->Name = name;
    entity->Id = EntityHelper::GenerateEntityId();
    return entity;
}

Entity* EntityManager::CreatePrefabEntity(ccstr8 name)
{
    Entity* entity = CreateEntity(name);
    entity->MakeEditorPrefab();
    return entity;
}

void EntityManager::AttachStaticFeature(Entity* entity, StaticFeature* parentFeature, const Transform& relativeTransform)
{
    ED_ASSERT(entity, "Can not attach feature to nullptr entity->");
    ED_ASSERT(!entity->PositionFeature, "Can not attach entity that allready has position feature.");

    StaticFeature* feature = m_StaticFeatures.Allocate();
    feature->OwnerEntity = entity;
    feature->ParentPositionFeature = parentFeature;
    feature->RelativeTransform = relativeTransform;

    if (parentFeature)
    {
        feature->WorldTransformation = parentFeature->WorldTransformation + feature->RelativeTransform;
    }

    entity->AddPositionalFeature(feature);
}

void EntityManager::AttachMovableFeature(Entity* entity, StaticFeature* parentFeature, const Transform& relativeTransform)
{
    ED_ASSERT(entity, "Can not attach feature to nullptr entity->");
    ED_ASSERT(!entity->PositionFeature, "Can not attach entity that allready has position feature.");

    MovableFeature* feature = m_MovableFeatures.Allocate();
    feature->OwnerEntity = entity;
    feature->ParentPositionFeature = parentFeature;
    feature->IsParentStatic = true;
    feature->RelativeTransform = relativeTransform;

    if (parentFeature)
    {
        feature->WorldTransformation = parentFeature->WorldTransformation + feature->RelativeTransform;
    }

    entity->AddPositionalFeature(feature);
}

void EntityManager::AttachMovableFeature(Entity* entity, MovableFeature* parentFeature, const Transform& relativeTransform)
{
    ED_ASSERT(entity, "Can not attach feature to nullptr entity->");
    ED_ASSERT(!entity->PositionFeature, "Can not attach entity that allready has position feature.");

    MovableFeature* feature = m_MovableFeatures.Allocate();
    feature->OwnerEntity = entity;
    feature->MovableParentPositionFeature = parentFeature;
    feature->IsParentStatic = false;
    feature->RelativeTransform = relativeTransform;

    if (parentFeature)
    {
        feature->WorldTransformation = parentFeature->WorldTransformation + feature->RelativeTransform;
    }

    entity->AddPositionalFeature(feature);
}

void EntityManager::ReattachPositionFeature(MovableFeature* feature, MovableFeature* parentFeature)
{
    ED_ASSERT(parentFeature, "Use a dedicated function to detach feature.");

    feature->MovableParentPositionFeature = parentFeature;
    feature->IsParentStatic = false;
    feature->WorldTransformation = parentFeature->WorldTransformation + feature->RelativeTransform;
}

void EntityManager::ReattachPositionFeature(MovableFeature* feature, StaticFeature* parentFeature)
{
    ED_ASSERT(parentFeature, "Use a dedicated function to detach feature.");

    feature->ParentPositionFeature = parentFeature;
    feature->IsParentStatic = true;
    feature->WorldTransformation = parentFeature->WorldTransformation + feature->RelativeTransform;
}

void EntityManager::ReattachPositionFeature(MovableFeature* feature, MovableFeature* parentFeature, const Transform& newRelativeTransform)
{
    ED_ASSERT(parentFeature, "Use a dedicated function to detach feature.");

    feature->MovableParentPositionFeature = parentFeature;
    feature->IsParentStatic = false;
    feature->RelativeTransform = newRelativeTransform;
    feature->WorldTransformation = parentFeature->WorldTransformation + feature->RelativeTransform;
}

void EntityManager::ReattachPositionFeature(MovableFeature* feature, StaticFeature* parentFeature, const Transform& newRelativeTransform)
{
    ED_ASSERT(parentFeature, "Use a dedicated function to detach feature.");

    feature->ParentPositionFeature = parentFeature;
    feature->IsParentStatic = true;
    feature->RelativeTransform = newRelativeTransform;
    feature->WorldTransformation = parentFeature->WorldTransformation + feature->RelativeTransform;
}

void EntityManager::ReattachPositionFeature(StaticFeature* feature, StaticFeature* parentFeature)
{
    ED_ASSERT(parentFeature, "Use a dedicated function to detach feature.");

    feature->ParentPositionFeature = parentFeature;
    feature->WorldTransformation = parentFeature->WorldTransformation + feature->RelativeTransform;
}

void EntityManager::ReattachPositionFeature(StaticFeature* feature, StaticFeature* parentFeature, const Transform& newRelativeTransform)
{
    ED_ASSERT(parentFeature, "Use a dedicated function to detach feature.");

    feature->ParentPositionFeature = parentFeature;
    feature->RelativeTransform = newRelativeTransform;
    feature->WorldTransformation = parentFeature->WorldTransformation + feature->RelativeTransform;
}

void EntityManager::DetachPositionFeature(MovableFeature* feature, bool useRelativeTransformAsWorld)
{
    ED_ASSERT(feature, "Feature can not be null.");

    feature->ParentPositionFeature = nullptr;
    feature->IsParentStatic = false;

    if (useRelativeTransformAsWorld)
    {
        feature->WorldTransformation = feature->RelativeTransform;
    }
}

void EntityManager::DetachPositionFeature(StaticFeature* feature, bool useRelativeTransformAsWorld)
{
    ED_ASSERT(feature, "Feature can not be null.");

    feature->ParentPositionFeature = nullptr;

    if (useRelativeTransformAsWorld)
    {
        feature->WorldTransformation = feature->RelativeTransform;
    }
}

StaticMeshFeature* EntityManager::AttachStaticMeshFeature(Entity* entity, StaticMeshAsset* mesh, MaterialAsset* materialOverride)
{
    ED_ASSERT(entity && entity->PositionFeature, "Can not attach this feature to an entity without position feature, pls attach position feature first");

    StaticMeshFeature* feature = m_StaticMeshFeatures.Allocate();
    if (mesh)
    {
        std::string name = mesh->BaseAsset->Name;
        strncpy(feature->Name, name.c_str(), std::min<u32>(name.size() , MaxPrefabCount));
        mesh->BaseAsset->ClaimData();
    }

    if (materialOverride)
    {
        materialOverride->BaseAsset->ClaimData();
    }

    feature->OwnerEntity = entity;
    feature->Mesh = mesh;
    feature->MaterialOverride = materialOverride;
    return feature;
}

PointLightFeature* EntityManager::AttachPointLightFeature(Entity* entity)
{
    ED_ASSERT(entity && entity->PositionFeature, "Can not attach this feature to an entity without position feature, pls attach position feature first");

    PointLightFeature* feature = m_PointLightFeatures.Allocate();
    feature->OwnerEntity = entity;

    return feature;
}

SpotLightFeature* EntityManager::AttachSpotLightFeature(Entity* entity)
{
    ED_ASSERT(entity && entity->PositionFeature, "Can not attach this feature to an entity without position feature, pls attach position feature first");

    SpotLightFeature* feature = m_SpotLightsFeatures.Allocate();
    feature->OwnerEntity = entity;

    return feature;
}

DirectionalLightFeature* EntityManager::AttachDirectionalLightFeature(Entity* entity)
{
    DirectionalLightFeature* feature = m_DirectionalLightsFeatures.Allocate();
    feature->OwnerEntity = entity;

    return feature;
}

CameraFeature* EntityManager::AttachCameraFeature(Entity* entity)
{
    ED_ASSERT(entity && entity->PositionFeature, "Can not attach this feature to an entity without position feature, pls attach position feature first");

    CameraFeature* feature = m_CamerasFeatures.Allocate();
    feature->OwnerEntity = entity;

    return feature;
}

RenderScene EntityManager::CollectRenderScene()
{
    // TODO: This could be optimized a bit by saving this overriding previous frames object as scene should not change drastically and it would allow not to reallocate that data once more
    RenderScene scene;

#if DEBUG_BUILD == 1 || DEVELOPMENT_BUILD == 1
    scene.Name = "Debug;)";
#endif

    {

       if (m_CamerasFeatures.GetAllocatedObjectsCount())
       {
           // TODO: Add support for camera selection
           CameraFeature* camera = *m_CamerasFeatures.begin();
           StaticFeature* position = camera->OwnerEntity->PositionFeature;

           scene.Camera.Position = position->WorldTransformation.GetTranslation();
           scene.Camera.Rotation = position->WorldTransformation.GetEulerRotation();
           scene.Camera.Fov = camera->CameraFov;
           scene.Camera.Near = camera->CameraNear;
           scene.Camera.Far = camera->CameraFar;
       }
#ifdef ED_EDITOR
       else
       {
           scene.Camera.Position = glm::vec3(50.0f, 0.0f, 0.0f);
           scene.Camera.Rotation = glm::vec3(0.0f, 0.0f, 0.0f);
           scene.Camera.Fov = 90.0f;
           scene.Camera.Near = 1.0f;
           scene.Camera.Far = 100000.0f;
       }
#else
       ED_ASSERT(m_CamerasFeatures.GetAllocatedObjectsCount(), "Must have at least one camera feature.");
#endif
    }

    {
        // TODO: this could be a good idea to add a field for direct position feature access to avoid two level indirection
        scene.StaticMeshes.Reserve(m_StaticMeshFeatures.GetAllocatedObjectsCount());
        for (StaticMeshFeature* mesh : m_StaticMeshFeatures)
        {
            if (!mesh->OwnerEntity->IsRenderable() || !mesh->Mesh)
            {
                continue;
            }

            MaterialAsset* material = mesh->MaterialOverride ? mesh->MaterialOverride : mesh->Mesh->Material;
            const bool hasMeshData = mesh->Mesh->BaseAsset->HasData;
            const bool hasMaterialData = material->BaseAsset->HasData;

            if (!hasMeshData || !hasMaterialData)
            {
                continue;
            }

            StaticMeshElement& element = scene.StaticMeshes.Add();

#if DEBUG_BUILD == 1 || DEVELOPMENT_BUILD == 1
            element.Name = mesh->Name;
#endif

            if (mesh->OwnerEntity->IsStaticEntity())
            {
                StaticFeature* position = mesh->OwnerEntity->StaticPositionFeature;
                element.WorldTransform = &position->WorldTransformation;
                element.PreviousWorldTransform = &position->WorldTransformation;
            }
            else
            {
                MovableFeature* position = mesh->OwnerEntity->MovablePositionFeature;
                element.WorldTransform = &position->WorldTransformation;
                element.PreviousWorldTransform = &position->PreviousWorldTransformation;
            }

            element.VertexBufferView = mesh->Mesh->VertexBufferView;
            element.IndexBufferView = mesh->Mesh->IndexBufferView;
            
            // TODO: Investigate idea of putting a material in there, it could be worth it if there will be not so much of debug data in material
            element.AlbedoTextureView = material->AlbedoTexture ? material->AlbedoTexture->TextureView : RenderingHelper::GetWhiteTexture();
            element.NormalTextureView = material->NormalTexture ? material->NormalTexture->TextureView : RenderingHelper::GetWhiteTexture();
            element.RoughnessTextureView = material->RoughnessTexture ? material->RoughnessTexture->TextureView : RenderingHelper::GetWhiteTexture();
            element.MetalicTextureView = material->MetalicTexture ? material->MetalicTexture->TextureView : RenderingHelper::GetWhiteTexture();

            element.BaseColor = material->BaseColor;
            element.Metalic = material->Metalic;
            element.Roughness = material->Roughness;
            element.Emission = material->Emission;
        }
    }

    {
        scene.DirectionalLights.Reserve(m_DirectionalLightsFeatures.GetAllocatedObjectsCount());
        for (DirectionalLightFeature* light : m_DirectionalLightsFeatures)
        {
            if (!light->OwnerEntity->IsRenderable()) continue;

            DirectionalLightElement& element = scene.DirectionalLights.Add();

#if DEBUG_BUILD == 1 || DEVELOPMENT_BUILD == 1
            element.Name = light->Name;
#endif

            element.Direction = glm::normalize(light->Direction);

            element.Color = light->Color;
            element.Intensity = light->Intensity;
            element.IsShadowCasting = light->IsShadowCasting;
            element.CascadesCount = light->CascadesCount;
            element.ShadowFilterSize = light->ShadowFilterSize;
            element.ShadowMapZMultiplier = light->ShadowMapZMultiplier;
            element.ShadowFilterRadius = light->ShadowFilterRadius;
        }
    }

    {
        scene.SpotLights.Reserve(m_SpotLightsFeatures.GetAllocatedObjectsCount());
        for (SpotLightFeature* light : m_SpotLightsFeatures)
        {
            if (!light->OwnerEntity->IsRenderable()) continue;

            SpotLightElement& element = scene.SpotLights.Add();

#if DEBUG_BUILD == 1 || DEVELOPMENT_BUILD == 1
            element.Name = light->Name;
#endif

            StaticFeature* position = light->OwnerEntity->PositionFeature;

            element.WorldTransform = &position->WorldTransformation;

            element.Color = light->Color;
            element.Intensity = light->Intensity;
            element.IsShadowCasting = light->IsShadowCasting;
            element.DrawWireframe = light->DrawWireframe;
            element.InnerAngle = light->InnerAngle;
            element.OuterAngle = light->OuterAngle;
            element.MaxDistance = light->MaxDistance;
            element.NearPlane = light->NearPlane;
            element.FarPlane = light->FarPlane;
            element.ShadowFilterSize = light->ShadowFilterSize;
            element.ShadowFilterRadius = light->ShadowFilterRadius;
        }
    }

    {
        scene.PointLights.Reserve(m_PointLightFeatures.GetAllocatedObjectsCount());
        for (PointLightFeature* light : m_PointLightFeatures)
        {
            if (!light->OwnerEntity->IsRenderable()) continue;

            PointLightElement& element = scene.PointLights.Add();

#if DEBUG_BUILD == 1 || DEVELOPMENT_BUILD == 1
            element.Name = light->Name;
#endif

            StaticFeature* position = light->OwnerEntity->PositionFeature;

            element.Position = position->WorldTransformation.GetTranslation();

            element.Color = light->Color;
            element.Intensity = light->Intensity;
            element.IsShadowCasting = light->IsShadowCasting;
            element.DrawWireframe = light->DrawWireframe;
            element.Radius = light->Radius;
            element.NearPlane = light->NearPlane;
            element.FarPlane = light->FarPlane;
            element.ShadowFilterRadius = light->ShadowFilterRadius;
            element.ShadowFilterSize = light->ShadowFilterSize;
        }
    }

    return scene;
}

Entity* EntityManager::FindEntity(u64 id)
{
    for (Entity* entity : m_Entites)
    {
        if (entity->Id == id)
        {
            return entity;
        }
    }

    return nullptr;
}

void EntityManager::SerializeEntity(Archive<>& archive, Entity** entity)
{
    if (archive.GetMode() == SerializationMode::Write)
    {
        if (*entity)
        {
            archive & (*entity)->Id;
        }
        else
        {
            archive & 1LLu;
        }
    }
    else
    {
        u64 id;
        archive & id;

        if (id == 1)
        {
            (*entity) = nullptr;
        }
        else
        {
            (*entity) = FindEntity(id);
        }
    }

}

void EntityManager::SerializeParentPositionalFeature(Archive<>& archive, StaticFeature** feature)
{
    if (archive.GetMode() == SerializationMode::Write)
    {
        if (*feature)
        {
            archive & (*feature)->OwnerEntity->Id;
        }
        else
        {
            archive & 1LLu;
        }
    }
    else
    {
        u64 id;
        archive & id;

        if (id == 1)
        {
            (*feature) = nullptr;
        }
        else
        {
            Entity* owner = FindEntity(id);
            (*feature) = reinterpret_cast<StaticFeature*>(owner);
        }
    }
}

void EntityManager::SerializeParentPositionalFeature(Archive<>& archive, MovableFeature** feature)
{
    if (archive.GetMode() == SerializationMode::Write)
    {
        if (feature)
        {
            archive & (*feature)->OwnerEntity->Id;
        }
        else
        {
            archive & 1LLu;
        }
    }
    else
    {
        u64 id;
        archive & id;

        if (id == 1)
        {
            (*feature) = nullptr;
        }
        else
        {
            Entity* owner = FindEntity(id);
            (*feature) = reinterpret_cast<MovableFeature*>(owner);

            ED_COND_LOG(!owner, EntityManager, warn, "Positional feature parent exists and have witten a valid owner but now when reading can not find it, some connections could have broken up during serialization!");
        }
    }
}

void* EntityManager::AllocateFeature(u32 type)
{
    switch (type)
    {
        case StaticFeature::Type: return reinterpret_cast<void*>(m_StaticFeatures.Allocate());
        case MovableFeature::Type: return reinterpret_cast<void*>(m_MovableFeatures.Allocate());
        case StaticMeshFeature::Type: return reinterpret_cast<void*>(m_StaticMeshFeatures.Allocate());
        case PointLightFeature::Type: return reinterpret_cast<void*>(m_PointLightFeatures.Allocate());
        case SpotLightFeature::Type: return reinterpret_cast<void*>(m_SpotLightsFeatures.Allocate());
        case DirectionalLightFeature::Type: return reinterpret_cast<void*>(m_DirectionalLightsFeatures.Allocate());
        case CameraFeature::Type: return reinterpret_cast<void*>(m_CamerasFeatures.Allocate());
        default: ED_ASSERT(0, "Feature type is not supported");
    }

    return nullptr;
}
