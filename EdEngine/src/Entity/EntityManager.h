#pragma once

#include "Entity.h"
#include "Features.h"

struct RenderScene;

#define ED_ITERATE_FEATURE_TYPE_BASE(function, type) \
    for (type* feature : EntityManager::Get().Get ## type ## s()) \
    { \
        function(feature, type::Type); \
    }

#define ED_ITERATE_FEATURE_TYPE_BASE_PARAMS(function, type, ...) \
    for (type* feature : EntityManager::Get().Get ## type ## s()) \
    { \
        function(feature, type::Type, __VA_ARGS__); \
    }

#define ED_ITERATE_FEATURE_TYPE(function, type) ED_ITERATE_FEATURE_TYPE_BASE(function, type)
#define ED_ITERATE_FEATURE_TYPE_NAMED(function, type) ED_ITERATE_FEATURE_TYPE_BASE_PARAMS(function, type, #type)

#define ED_ITERATE_FEATURE_TYPE_PARAMS(function, type, ...)  ED_ITERATE_FEATURE_TYPE_BASE_PARAMS(function, type, __VA_ARGS__)
#define ED_ITERATE_FEATURE_TYPE_NAMED_PARAMS(function, type, ...)  ED_ITERATE_FEATURE_TYPE_BASE_PARAMS(function, type, #type, __VA_ARGS__)

#define ED_ITERATE_POSITIONAL_FEATURES(function) \
    ED_ITERATE_FEATURE_TYPE(function, StaticFeature) \
    ED_ITERATE_FEATURE_TYPE(function, MovableFeature)

#define ED_ITERATE_POSITIONAL_FEATURES_NAMED(function) \
    ED_ITERATE_FEATURE_TYPE_NAMED(function, StaticFeature) \
    ED_ITERATE_FEATURE_TYPE_NAMED(function, MovableFeature)

#define ED_ITERATE_POSITIONAL_FEATURES_PARAMS(function, ...) \
    ED_ITERATE_FEATURE_TYPE_PARAMS(function, StaticFeature, __VA_ARGS__) \
    ED_ITERATE_FEATURE_TYPE_PARAMS(function, MovableFeature, __VA_ARGS__)

#define ED_ITERATE_POSITIONAL_FEATURES_NAMED_PARAMS(function, ...) \
    ED_ITERATE_FEATURE_TYPE_NAMED_PARAMS(function, StaticFeature, __VA_ARGS__) \
    ED_ITERATE_FEATURE_TYPE_NAMED_PARAMS(function, MovableFeature, __VA_ARGS__)

#define ED_ITERATE_FEATURES(function) \
    ED_ITERATE_FEATURE_TYPE(function, StaticMeshFeature) \
    ED_ITERATE_FEATURE_TYPE(function, PointLightFeature) \
    ED_ITERATE_FEATURE_TYPE(function, SpotLightFeature) \
    ED_ITERATE_FEATURE_TYPE(function, DirectionalLightFeature) \
    ED_ITERATE_FEATURE_TYPE(function, CameraFeature)

#define ED_ITERATE_FEATURES_NAMED(function) \
    ED_ITERATE_FEATURE_TYPE_NAMED(function, StaticMeshFeature) \
    ED_ITERATE_FEATURE_TYPE_NAMED(function, PointLightFeature) \
    ED_ITERATE_FEATURE_TYPE_NAMED(function, SpotLightFeature) \
    ED_ITERATE_FEATURE_TYPE_NAMED(function, DirectionalLightFeature) \
    ED_ITERATE_FEATURE_TYPE_NAMED(function, CameraFeature)

#define ED_ITERATE_FEATURES_PARAMS(function, ...) \
    ED_ITERATE_FEATURE_TYPE_PARAMS(function, StaticMeshFeature, __VA_ARGS__) \
    ED_ITERATE_FEATURE_TYPE_PARAMS(function, PointLightFeature, __VA_ARGS__) \
    ED_ITERATE_FEATURE_TYPE_PARAMS(function, SpotLightFeature, __VA_ARGS__) \
    ED_ITERATE_FEATURE_TYPE_PARAMS(function, DirectionalLightFeature, __VA_ARGS__) \
    ED_ITERATE_FEATURE_TYPE_PARAMS(function, CameraFeature, __VA_ARGS__)

#define ED_ITERATE_FEATURES_NAMED_PARAMS(function, ...) \
    ED_ITERATE_FEATURE_TYPE_NAMED_PARAMS(function, StaticMeshFeature, __VA_ARGS__) \
    ED_ITERATE_FEATURE_TYPE_NAMED_PARAMS(function, PointLightFeature, __VA_ARGS__) \
    ED_ITERATE_FEATURE_TYPE_NAMED_PARAMS(function, SpotLightFeature, __VA_ARGS__) \
    ED_ITERATE_FEATURE_TYPE_NAMED_PARAMS(function, DirectionalLightFeature, __VA_ARGS__) \
    ED_ITERATE_FEATURE_TYPE_NAMED_PARAMS(function, CameraFeature, __VA_ARGS__)

#define ED_ITERATE_ALL_FEATURES(function) \
        ED_ITERATE_POSITIONAL_FEATURES(function) \
        ED_ITERATE_FEATURES(function)

#define ED_ITERATE_ALL_FEATURES_NAMED(function) \
        ED_ITERATE_POSITIONAL_FEATURES_NAMED(function) \
        ED_ITERATE_FEATURES_NAMED(function)

#define ED_ITERATE_ALL_FEATURES_PARAMS(function, ...) \
        ED_ITERATE_POSITIONAL_FEATURES_PARAMS(function, __VA_ARGS__) \
        ED_ITERATE_FEATURES_PARAMS(function, __VA_ARGS__)

#define ED_ITERATE_ALL_FEATURES_NAMED_PARAMS(function, ...) \
        ED_ITERATE_POSITIONAL_FEATURES_NAMED_PARAMS(function, __VA_ARGS__) \
        ED_ITERATE_FEATURES_NAMED_PARAMS(function, __VA_ARGS__)

// TODO: Add serialization as this now plays scene role of the previous engine iteration
class EntityManager
{
public:
    static constexpr u32 MaxEntitiesCount = 1024;

    static constexpr u32 MaxStaticEntitiesCount = 1024;
    static constexpr u32 MaxMovableEntitiesCount = 128;

    static constexpr u32 MaxStaticMeshesCount = 1024;

    static constexpr u32 MaxPointLightsCount = 10;
    static constexpr u32 MaxSpotLigthsCount = 10;
    static constexpr u32 MaxDirectionalLigthsCount = 2;

    static constexpr u32 MaxCamerasCount = 10;

    static constexpr u32 MaxPrefabCount = 100;

    static constexpr u32 MaxTransformationAttachmentDepth = 5;
public:
    static inline EntityManager& Get()
    {
        ED_ASSERT(s_Manager, "Entity manager was not yet initialized");
        return *s_Manager;
    }

    void Initialize();
    void Deinitialize();

    void Update(f32 DeltaSeconds);

    void SaveScene();
    void SaveScene(const ContentPath& path);
    
    void LoadScene();
    void LoadScene(const ContentPath& path);

    const ContentPath& GetLoadedScenePath() const { return m_LoadedScenePath; }

    // TODO: In future check if there is a need to create a variation of these method for multiple entities at once

    // TODO: Add debug loop attachment protection
    Entity* CreateEntity(ccstr8 name);
    Entity* CreatePrefabEntity(ccstr8 name);
    Prefab* CreatePrefab(ccstr8 name);

    void AttachStaticFeature(Entity* entity, StaticFeature* parentFeature, const Transform& relativeTransform = Transform::Unit);
    void AttachMovableFeature(Entity* entity, StaticFeature* parentFeature, const Transform& relativeTransform = Transform::Unit);
    void AttachMovableFeature(Entity* entity, MovableFeature* parentFeature, const Transform& relativeTransform = Transform::Unit);

    void ReattachPositionFeature(MovableFeature* feature, MovableFeature* parentFeature);
    void ReattachPositionFeature(MovableFeature* feature, StaticFeature* parentFeature);

    void ReattachPositionFeature(MovableFeature* feature, MovableFeature* parentFeature, const Transform& newRelativeTransform);
    void ReattachPositionFeature(MovableFeature* feature, StaticFeature* parentFeature, const Transform& newRelativeTransform);

    void ReattachPositionFeature(StaticFeature* feature, StaticFeature* parentFeature);
    void ReattachPositionFeature(StaticFeature* feature, StaticFeature* parentFeature, const Transform& newRelativeTransform);

    void DetachPositionFeature(MovableFeature* feature, bool useRelativeTransformAsWorld);
    void DetachPositionFeature(StaticFeature* feature, bool useRelativeTransformAsWorld);

    StaticMeshFeature* AttachStaticMeshFeature(Entity* entity, StaticMeshAsset* mesh, MaterialAsset* materialOverride = nullptr);
    PointLightFeature* AttachPointLightFeature(Entity* entity);
    SpotLightFeature* AttachSpotLightFeature(Entity* entity);
    DirectionalLightFeature* AttachDirectionalLightFeature(Entity* entity);
    CameraFeature* AttachCameraFeature(Entity* entity);

    RenderScene CollectRenderScene();

    Entity* FindEntity(u64 id);

    Prefab* CreatePrefab(ccstr8 name, const Array<Entity*>& entities);
    Prefab* CreatePrefab(PrefabAsset* asset);
    Prefab* CreatePrefab(ccstr8 name, PrefabAsset* asset);

    void SerializeEntity(Archive<>& archive, Entity** entity);
    void SerializeParentPositionalFeature(Archive<>& archive, StaticFeature** feature);
    void SerializeParentPositionalFeature(Archive<>& archive, MovableFeature** feature);

    inline const PoolAllocator<Entity, MaxEntitiesCount>& GetEntites() const { return m_Entites; }
    inline const PoolAllocator<StaticFeature, MaxStaticEntitiesCount>& GetStaticFeatures() const { return m_StaticFeatures; }
    inline const PoolAllocator<MovableFeature, MaxMovableEntitiesCount>& GetMovableFeatures() const { return m_MovableFeatures; }
    inline const PoolAllocator<StaticMeshFeature, MaxStaticMeshesCount>& GetStaticMeshFeatures() const { return m_StaticMeshFeatures; }
    inline const PoolAllocator<PointLightFeature, MaxPointLightsCount>& GetPointLightFeatures() const { return m_PointLightFeatures; }
    inline const PoolAllocator<SpotLightFeature, MaxSpotLigthsCount>& GetSpotLightFeatures() const { return m_SpotLightsFeatures; }
    inline const PoolAllocator<DirectionalLightFeature, MaxDirectionalLigthsCount>& GetDirectionalLightFeatures() const { return m_DirectionalLightsFeatures; }
    inline const PoolAllocator<CameraFeature, MaxCamerasCount>& GetCameraFeatures() const { return m_CamerasFeatures; }

protected:
    void* AllocateFeature(u32 type);

protected:
    void UpdateMovableFeatures();
    
    void Serialize(Archive<>& archive);
    void ResolveDependencies();
    void ResolveDependencies(Prefab* prefab);

    void ResolveAssetDependenciesOnLoad(void* feature, u32 type);

    void UnloadScene();

    static inline EntityManager* s_Manager;

    EntityManager();
    friend class Engine;
protected:
    ContentPath m_LoadedScenePath;
    bool m_SceneLoaded;

    PoolAllocator<Entity, MaxEntitiesCount> m_Entites;
    PoolAllocator<StaticFeature, MaxStaticEntitiesCount> m_StaticFeatures;
    PoolAllocator<MovableFeature, MaxMovableEntitiesCount> m_MovableFeatures;
    PoolAllocator<StaticMeshFeature, MaxStaticMeshesCount> m_StaticMeshFeatures;
    PoolAllocator<PointLightFeature, MaxPointLightsCount> m_PointLightFeatures;
    PoolAllocator<SpotLightFeature, MaxSpotLigthsCount> m_SpotLightsFeatures;
    PoolAllocator<DirectionalLightFeature, MaxDirectionalLigthsCount> m_DirectionalLightsFeatures;
    PoolAllocator<CameraFeature, MaxCamerasCount> m_CamerasFeatures;
    PoolAllocator<Prefab, MaxPrefabCount> m_Prefabs;
};