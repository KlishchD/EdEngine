#pragma once

#include "Component.h"
#include "Core/Ed.h"
#include "Core/Assets/StaticMesh.h"

class StaticMeshComponent : public Component
{
    friend class boost::serialization::access;

    template <class Archive>
    void save(Archive& ar, const uint32_t version) const
    {
        ar & boost::serialization::base_object<Component>(*this);
        
        ar & GetStaticMeshAssetId();
    }

    template <class Archive>
    void load(Archive& ar, const uint32_t version)
    {
        ar & boost::serialization::base_object<Component>(*this);
        
        UUID staticMeshId;
        ar & staticMeshId;

        m_StaticMesh = AssetUtils::CreateStaticMesh(staticMeshId);
    }
    
    BOOST_SERIALIZATION_SPLIT_MEMBER()
public:
    StaticMeshComponent();
    StaticMeshComponent(const StaticMeshComponent& submesh);
    StaticMeshComponent(std::shared_ptr<StaticMesh> mesh);
    
    void SetStaticMesh(std::shared_ptr<StaticMesh> mesh);
    std::shared_ptr<StaticMesh> GetStaticMesh() const;
    
   
    virtual ComponentType GetType() const override;
private:
    std::shared_ptr<StaticMesh> m_StaticMesh;

    UUID GetStaticMeshAssetId() const;
};

BOOST_CLASS_VERSION(StaticMeshComponent, 1)