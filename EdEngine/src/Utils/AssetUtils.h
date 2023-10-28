#pragma once
#include <memory>

struct StaticMeshDescriptor;
struct StaticSubmeshData;

class StaticSubmesh;
class StaticMesh;

class AssetUtils
{
public:
    static void SetUseFullDescriptor(bool state);
    static bool UseFullDescriptor();
    
    static std::shared_ptr<StaticMesh> CreateStaticMesh(const std::shared_ptr<StaticMeshDescriptor>& descriptor);
    static std::shared_ptr<StaticMesh> CreateStaticMesh(int32_t staticMeshId);

    static std::shared_ptr<StaticSubmesh> CreateStaticSubmesh(const StaticSubmeshData& data);
private:
    inline static bool m_UseFullDescriptor = false;
};
