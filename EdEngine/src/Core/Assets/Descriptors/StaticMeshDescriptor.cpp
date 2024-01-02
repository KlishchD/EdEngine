#include "StaticMeshDescriptor.h"

BOOST_CLASS_EXPORT_IMPLEMENT(StaticMeshDescriptor)

StaticSubmeshData::StaticSubmeshData(StaticSubmeshData&& data) noexcept
{
	Name = std::move(data.Name);
	Vertices = std::move(data.Vertices);
	Indices = std::move(data.Indices);

	MaterialID = data.MaterialID;
}

StaticSubmeshData::StaticSubmeshData()
{
}

bool StaticMeshDescriptor::HasData() const
{
	return MeshData.size();
}
