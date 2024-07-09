#pragma once

#include "Core/Rendering/Textures/CubeTexture.h"
#include "Helpers/RenderingHelper.h"

ED_CLASS2(OpenGLCubeTexture, RenderingHelper::CreateCubeTexture) : public CubeTexture
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