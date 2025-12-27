#pragma once

struct StaticFeature;
struct MovableFeature;

enum EntityStates
{
    ES_IsDirty = (1 << 0),

    ES_HasPosition = (1 << 1),
    ES_IsStatic = (1 << 2),
    ES_IsMovalbe = (1 << 3),

    ES_IsEditorPrefab = (1 << 4), // Editor only flag for to signal that it is temporary entity for imported scene and should not be rendered and/or processed.
    ES_TemporaryPrefabMarker = (1 << 5), // Marker for active feature prefab feature selection.
    ES_IsScenePrefab = (1 << 6), // Marker to signal that this is prefab that is placed on a scene and needs to be rendered and/or processed.

    ES_MakeStatic = (ES_HasPosition | ES_IsStatic | ES_IsDirty),
    ES_MakeMovalbe = (ES_HasPosition | ES_IsMovalbe | ES_IsDirty),
};

struct Entity
{
    std::string Name;
    u64 Id;

    union {
        StaticFeature* StaticPositionFeature;
        MovableFeature* MovablePositionFeature;
        StaticFeature* PositionFeature;
    };

    u64 BoolField; // To store permanent flag, that are serialized.
    u32 RuntimeBoolField; // To store runtime flags, that are not serialized.

    // To Prevent detaching if positional feature is needed for other features.
    u32 PositionalFeaturesDependenciesCount;

    inline void MakeDirty() { RuntimeBoolField |= ES_IsDirty; }
    inline bool IsDirty() const { return RuntimeBoolField & ES_IsDirty; }

    inline bool HasPosition() const { return BoolField & (1 << 0); }

    inline bool IsStaticEntity() const { return (BoolField & ES_IsStatic) == ES_IsStatic; }
    inline bool IsMovableEntity() const { return (BoolField & ES_IsMovalbe) == ES_IsMovalbe; }

    inline void MakeStatic() { BoolField = (BoolField | ES_MakeStatic) & (~ES_IsMovalbe); }
    inline void MakeDynamic() { BoolField = (BoolField | ES_MakeMovalbe) & (~ES_IsStatic); }

    inline void AddPositionalFeature(StaticFeature* feature) { StaticPositionFeature = feature; MakeStatic(); }
    inline void AddPositionalFeature(MovableFeature* feature) { MovablePositionFeature = feature; MakeDynamic(); }

    inline void MakeEditorPrefab() { RuntimeBoolField = (RuntimeBoolField | ES_IsEditorPrefab) & ~(ES_TemporaryPrefabMarker | ES_IsScenePrefab); }
    inline bool IsEditorPrefab() const { return RuntimeBoolField & ES_IsEditorPrefab; }

    inline void MakeScenePrefab() { RuntimeBoolField = (RuntimeBoolField | ES_IsScenePrefab) & ~(ES_TemporaryPrefabMarker | ES_IsEditorPrefab); }
    inline bool IsScenePrefab() const { return RuntimeBoolField & ES_IsScenePrefab; }

    inline bool IsRenderable() const { return !(RuntimeBoolField & ES_IsEditorPrefab); }

    inline void AddTemporaryPrefabMarker() { RuntimeBoolField |= ES_TemporaryPrefabMarker; }
    inline void RemoveTemporaryPrefabMarker() { RuntimeBoolField &= ~ES_TemporaryPrefabMarker; }
    inline bool HasTemporaryPrefabMarker() const { return RuntimeBoolField & ES_TemporaryPrefabMarker; }

    void Serialize(Archive<>& archive);
};