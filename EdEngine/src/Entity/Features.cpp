#include "EdEngine.h"
#include "Features.h"

void StaticFeature::Serialize(Archive<>& archive)
{
    EntityManager::Get().SerializeEntity(archive, &OwnerEntity);

    OwnerEntity->AddPositionalFeature(this);

    EntityManager::Get().SerializeParentPositionalFeature(archive, &ParentPositionFeature);

    archive & RelativeTransform;
    archive & WorldTransformation;
}

void MovableFeature::Serialize(Archive<>& archive)
{
    EntityManager::Get().SerializeEntity(archive, &OwnerEntity);

    OwnerEntity->AddPositionalFeature(this);

    EntityManager::Get().SerializeParentPositionalFeature(archive, &ParentPositionFeature);

    archive & RelativeTransform;
    archive & WorldTransformation;
}

void StaticMeshFeature::Serialize(Archive<>& archive)
{
    archive.Serialize<c8, false>(Name, (u32)strnlen(Name, MaxFeatureNameSize), false);

    EntityManager::Get().SerializeEntity(archive, &OwnerEntity);

    AssetManager::Get().SerializeAsset(archive, &Mesh, false);
    AssetManager::Get().SerializeAsset(archive, &MaterialOverride, false);
}

void StaticMeshFeature::LoadAssets()
{
    if (Mesh)
    {
        AssetManager::Get().LoadAsset(Mesh->BaseAsset);
    }

    if (MaterialOverride)
    {
        AssetManager::Get().LoadAsset(MaterialOverride->BaseAsset);
    }
}

StaticMeshFeature::~StaticMeshFeature()
{
    if (Mesh)
    {
        Mesh->BaseAsset->UnclaimData();
    }

    if (MaterialOverride)
    {
        MaterialOverride->BaseAsset->UnclaimData();
    }
}

void PointLightFeature::Serialize(Archive<>& archive)
{
    EntityManager::Get().SerializeEntity(archive, &OwnerEntity);

    archive.Serialize<c8, false>(Name, (u32)strnlen(Name, MaxFeatureNameSize), false);

    archive & Color;

    archive & Intensity;
    archive & Radius;

    archive & NearPlane;
    archive & FarPlane;

    archive & ShadowFilterRadius;
    archive & ShadowFilterSize;

    archive & IsShadowCasting;
}

void SpotLightFeature::Serialize(Archive<>& archive)
{
    EntityManager::Get().SerializeEntity(archive, &OwnerEntity);

    archive.Serialize<c8, false>(Name, (u32)strnlen(Name, MaxFeatureNameSize), false);

    archive & Color;

    archive & Intensity;

    archive & InnerAngle;
    archive & OuterAngle;
    archive & MaxDistance;

    archive & NearPlane;
    archive & FarPlane;

    archive & ShadowFilterRadius;
    archive & ShadowFilterSize;

    archive & IsShadowCasting;
}

void DirectionalLightFeature::Serialize(Archive<>& archive)
{
    EntityManager::Get().SerializeEntity(archive, &OwnerEntity);

    archive.Serialize<c8, false>(Name, (u32)strnlen(Name, MaxFeatureNameSize), false);

    archive & Color;

    archive & Intensity;

    archive & ShadowMapZMultiplier;
    archive & ShadowFilterRadius;

    archive & CascadesCount;
    archive & ShadowFilterSize;

    archive & IsShadowCasting;
}

void CameraFeature::Serialize(Archive<>& archive)
{
    EntityManager::Get().SerializeEntity(archive, &OwnerEntity);

    archive.Serialize<c8, false>(Name, (u32)strnlen(Name, MaxFeatureNameSize), false);
    archive & CameraFov;
    archive & CameraNear;
    archive & CameraFar;
}
