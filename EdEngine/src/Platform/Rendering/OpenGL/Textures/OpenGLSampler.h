#pragma once

#include "Core/Rendering/Textures/Sampler.h"

class OpenGLSampler : public Sampler
{
public:
	OpenGLSampler();

	virtual void EnbaleAnisotrophicFiltering(float samples);

	virtual ~OpenGLSampler() override;
};