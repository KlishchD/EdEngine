#pragma once

#include "Core/Ed.h"

// TODO: Make an asset and add specifications

class Sampler {
public:
	uint32_t GetID() const;
	
	virtual void EnbaleAnisotrophicFiltering(const float samples) = 0;

	virtual ~Sampler() = default;
protected:
	uint32_t m_Id;
};