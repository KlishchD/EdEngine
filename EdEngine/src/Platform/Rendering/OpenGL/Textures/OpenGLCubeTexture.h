#pragma once

#include "Core/Rendering/Textures/CubeTexture.h"

ED_CLASS(OpenGLCubeTexture) : public CubeTexture
{
	ED_CLASS_BODY(OpenGLCubeTexture, CubeTexture)
public:
	OpenGLCubeTexture(const std::string& name = "Empty");

	virtual void Initialize() override;

	virtual ~OpenGLCubeTexture() override;
protected:
	virtual void RefreshData() override;
	virtual void RefreshParameters() override;
};