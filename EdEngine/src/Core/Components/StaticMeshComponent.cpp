#include "StaticMeshComponent.h"
#include "Core/Assets/StaticMesh.h"

BOOST_CLASS_EXPORT(StaticMeshComponent)

StaticMeshComponent::StaticMeshComponent(): Component("StaticMesh"), m_StaticMesh(nullptr) {}

StaticMeshComponent::StaticMeshComponent(const StaticMeshComponent& StaticMesh): Component("StaticMesh"), m_StaticMesh(StaticMesh.m_StaticMesh) {}

StaticMeshComponent::StaticMeshComponent(const std::shared_ptr<StaticMesh>& mesh): Component("StaticMesh")
{
    SetStaticMesh(mesh);
}

void StaticMeshComponent::SetStaticMesh(const std::shared_ptr<StaticMesh>& mesh)
{
    m_Name = mesh ? mesh->GetAssetName() : "";
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

int32_t StaticMeshComponent::GetStaticMeshAssetId() const
{
    int32_t id = -1;
    if (m_StaticMesh)
    {
        if (std::shared_ptr<AssetDescriptor> descriptor = m_StaticMesh->GetDescriptor())
        {
            id = descriptor->AssetId;
        }
    }
    return id;
}