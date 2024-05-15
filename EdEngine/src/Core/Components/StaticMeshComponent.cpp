#include "StaticMeshComponent.h"
#include "Core/Assets/StaticMesh.h"

StaticMeshComponent::StaticMeshComponent(): Super("StaticMesh"), m_StaticMesh(nullptr) {}

StaticMeshComponent::StaticMeshComponent(const StaticMeshComponent& StaticMesh): Super("StaticMesh"), m_StaticMesh(StaticMesh.m_StaticMesh) {}

StaticMeshComponent::StaticMeshComponent(std::shared_ptr<StaticMesh> mesh): Super("StaticMesh")
{
    SetStaticMesh(mesh);
}

void StaticMeshComponent::SetStaticMesh(std::shared_ptr<StaticMesh> mesh)
{
    m_Name = mesh ? mesh->GetName() : "";
    m_StaticMesh = mesh;
}

std::shared_ptr<StaticMesh> StaticMeshComponent::GetStaticMesh() const
{
    return m_StaticMesh;
}

ComponentType StaticMeshComponent::GetType() const
{
    return ComponentType::StaticMesh;
}

void StaticMeshComponent::Serialize(Archive& archive)
{
    Super::Serialize(archive);

    m_StaticMesh = SerializationHelper::SerializeAsset(archive, m_StaticMesh);
}