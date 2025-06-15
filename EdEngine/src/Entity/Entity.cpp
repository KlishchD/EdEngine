#include "EdEngine.h"
#include "Entity.h"

void Entity::Serialize(Archive<>& archive)
{
    archive & Name;
    archive & Id;
    archive & BoolField;
    archive & PositionalFeaturesDependenciesCount;
}
