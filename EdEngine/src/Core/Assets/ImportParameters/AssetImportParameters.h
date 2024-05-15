#pragma once

#include <string>
#include "Core/Objects/GameObject.h"

ED_CLASS(AssetImportParameters) : public GameObject
{
	ED_CLASS_BODY(AssetImportParameters, GameObject)
public:
	std::string Path;

	virtual void Serialize(Archive& archive) override;
};
