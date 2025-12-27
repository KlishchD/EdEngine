#pragma once

namespace EntityHelper
{
    u64 GenerateEntityId();

    u32 GetFeatureTypeSize(u32 type, void* data);
    void SerializeFeature(Archive<>& archive, u32 type, void* feature);
};