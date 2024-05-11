#pragma once

#include "Component.h"
#include "Core/Ed.h"
#include "Core/Assets/StaticMesh.h"

class StaticMeshComponent : public Component
{
public:
    StaticMeshComponent();
    StaticMeshComponent(const StaticMeshComponent& submesh);
    StaticMeshComponent(std::shared_ptr<StaticMesh> mesh);
    
    void SetStaticMesh(std::shared_ptr<StaticMesh> mesh);
    std::shared_ptr<StaticMesh> GetStaticMesh() const;
   
    virtual ComponentType GetType() const override;

    virtual void Serialize(Archive& archive) override;
private:
    std::shared_ptr<StaticMesh> m_StaticMesh;

    UUID GetStaticMeshAssetId() const;
};
