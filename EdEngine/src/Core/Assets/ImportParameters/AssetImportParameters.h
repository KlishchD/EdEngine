#pragma once

#include <string>
#include "Utils/Serializable.h"

struct AssetImportParameters : public Serializable
{
	std::string Path;

	virtual void Serialize(Archive& archive) override;
};
