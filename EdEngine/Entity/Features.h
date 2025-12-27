#pragma once

// Only movable entities can be moved during "Play".
// If static entity's transformation will be changed during "Play" only it will be changed and no children will be updated.
// If there is a need to move entity during "Play" please make it movable.
// Parenting rules: Movable entity can have both a static and a movable entity as a parent but static entity can have only a static entity
// as a parent because only movable entities' transforms are being processed and updated every frame, so a static entity whose parent
// is a movable entity will not get any updates from parents movement.

struct Entity;
struct StaticMeshAsset;
struct MaterialAsset;

constexpr inline u32 MaxFeatureNameSize = 256;

// Do not reorder common parameters from static and movable features !!!
struct StaticFeature
{
    static inline constexpr i32 Type = 1;

    Entity* OwnerEntity;
    StaticFeature* ParentPositionFeature;
    Transform RelativeTransform;
    Transform WorldTransformation;

    u32 State;

    inline void UpdateWorldPosition(const Transform& world)
    {
        WorldTransformation = world + RelativeTransform;
        MarkUpdated();
    }

    inline bool ForcedUpdate() const { return State & 1; }
    inline void ForceUpdate() { State |= 1; }
    inline void ClearForcedUpdate() { State |= ~1; }

    inline bool IsUpdated() const { return State & 2; }
    inline void MarkUpdated() { State |= 2; }

    inline void ClearState() { State = 0; }

    void Serialize(Archive<>& archive);
};

struct MovableFeature
{
    static inline constexpr i32 Type = 2;

    Entity* OwnerEntity;

    union {
        StaticFeature* StaticParentPositionFeature;
        MovableFeature* MovableParentPositionFeature;
        StaticFeature* ParentPositionFeature;
    };

    Transform RelativeTransform;
    Transform WorldTransformation;

    u32 State;
    u32 IsParentStatic;

    Transform PreviousWorldTransformation;

    Transform NextTransformation;

    // TODO: Add some kind of ability to control the movement

    inline void UpdateWorldPosition(const Transform& world)
    {
        PreviousWorldTransformation = WorldTransformation;
        WorldTransformation = world + RelativeTransform;
        State |= 1;
    }

    inline bool WasUpdated() const { return State & 1; }
    inline void ClearState() { State = 0; }

    void Serialize(Archive<>& archive);
};

struct StaticMeshFeature
{
    static inline constexpr i32 Type = 3;

    cstr8 Name = Strings::RequestString("DefaultStaticMeshFeature", MaxFeatureNameSize, false);
    
    Entity* OwnerEntity;
    StaticMeshAsset* Mesh;
    MaterialAsset* MaterialOverride;

    void Serialize(Archive<>& archive);
    void LoadAssets();

    ~StaticMeshFeature();
};

struct PointLightFeature
{
    static inline constexpr i32 Type = 4;

    cstr8 Name = Strings::RequestString("DefaultPointLightFeature", MaxFeatureNameSize, false);
    
    Entity* OwnerEntity;

    glm::vec3 Color = glm::vec3(1.0f);

    f32 Intensity = glm::radians(1.0f);
    f32 Radius = 1.0f;

    f32 NearPlane = 0.1f;
    f32 FarPlane = 1000.0f;

    f32 ShadowFilterRadius = 3.0f;
    u16 ShadowFilterSize = 3;

    bool IsShadowCasting = true;
    bool DrawWireframe = false;
  
    void Serialize(Archive<>& archive);
};

struct SpotLightFeature
{
    static inline constexpr i32 Type = 5;

    cstr8 Name = Strings::RequestString("DefaultSpotLightFeature", MaxFeatureNameSize, false);

    Entity* OwnerEntity;

    glm::vec3 Color = glm::vec3(1.0f);

    f32 Intensity = glm::radians(1.0f);

    f32 InnerAngle = 1.0f;
    f32 OuterAngle = 1.0f;
    f32 MaxDistance = 1.0f;

    f32 NearPlane = 0.1f;
    f32 FarPlane = 1000.0f;

    f32 ShadowFilterRadius = 3.0f;
    u16 ShadowFilterSize = 4;

    bool IsShadowCasting = true;
    bool DrawWireframe = false;
  
    void Serialize(Archive<>& archive);
};

struct DirectionalLightFeature
{
    static inline constexpr i32 Type = 6;

    cstr8 Name = Strings::RequestString("DefaultDirectionalLightFeature", MaxFeatureNameSize, false);
    
    Entity* OwnerEntity;

    glm::vec3 Direction = glm::vec3(0.0f, -1.0f, 0.0f);

    glm::vec3 Color = glm::vec3(1.0f);

    f32 Intensity = glm::radians(1.0f);

    f32 ShadowMapZMultiplier = 10.0f;
    f32 ShadowFilterRadius = 3.0f;

    u8 CascadesCount = 4;
    u8 ShadowFilterSize = 3;

    bool IsShadowCasting = true;
  
    void Serialize(Archive<>& archive);
};

struct CameraFeature
{
    static inline constexpr i32 Type = 7;

    cstr8 Name = Strings::RequestString("DefaultCameraFeature", MaxFeatureNameSize, false);
    
    Entity* OwnerEntity;
    f32 CameraFov;
    f32 CameraNear;
    f32 CameraFar;
  
    void Serialize(Archive<>& archive);
};

struct Feature
{
    u32 Type;
    void* Data;
};

struct Prefab
{
    cstr8 Name = Strings::RequestString("DefautltPrefab", MaxFeatureNameSize, false);

    Array<Entity*> Entities;
    Array<Feature> Features;
};