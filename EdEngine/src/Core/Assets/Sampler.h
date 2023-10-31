#pragma once

#include "Core/Ed.h"

class Sampler {
public:
	Sampler();

	uint32_t GetID() const;

	void Bind(uint32_t slot);
	void Unbind(uint32_t slot);

	void EnbaleAnisotrophicFiltering(const float samples);

	~Sampler();
private:
	uint32_t m_Id;
};