#include "EdEngine.h"
#include "EntityHelper.h"

u64 EntityHelper::GenerateEntityId()
{
    static std::random_device rd;
    static std::mt19937_64 eng(rd());
    static std::uniform_int_distribution<u64> distribution;
    return distribution(eng) & ~(1 << 0); // 0 bit is for invalid id's, so don't set it
}

u32 EntityHelper::GetFeatureTypeSize(u32 type, void* data)
{
    switch (type)
    {
        case StaticFeature::Type: return sizeof(StaticFeature);
        case MovableFeature::Type: return sizeof(MovableFeature);
        case StaticMeshFeature::Type: return sizeof(StaticMeshFeature);
        case PointLightFeature::Type: return sizeof(PointLightFeature);
        case SpotLightFeature::Type: return sizeof(SpotLightFeature);
        case DirectionalLightFeature::Type: return sizeof(DirectionalLightFeature);
        case CameraFeature::Type: return sizeof(CameraFeature);
        default: ED_ASSERT(0, "Provided Feature type is not supported");
    }

    return 0;
}

void EntityHelper::SerializeFeature(Archive<>& archive, u32 type, void* feature)
{
    switch (type)
    {
    case StaticFeature::Type: archive & *static_cast<StaticFeature*>(feature); break;
    case MovableFeature::Type: archive & *static_cast<MovableFeature*>(feature); break;
    case StaticMeshFeature::Type: archive & *static_cast<StaticMeshFeature*>(feature); break;
    case PointLightFeature::Type: archive & *static_cast<PointLightFeature*>(feature); break;
    case SpotLightFeature::Type: archive & *static_cast<SpotLightFeature*>(feature); break;
    case DirectionalLightFeature::Type: archive & *static_cast<DirectionalLightFeature*>(feature); break;
    case CameraFeature::Type: archive & *static_cast<CameraFeature*>(feature); break;
    default: ED_ASSERT(0, "Provided Feature type is not supported"); break;
    }
}