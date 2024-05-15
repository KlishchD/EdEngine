#pragma once

#include "Core/Rendering/Textures/Texture2D.h"

ED_CLASS(OpenGLTexture2D) : public Texture2D 
{
	ED_CLASS_BODY(OpenGLTexture2D, Texture2D)
public:
	OpenGLTexture2D(const std::string& name = "Empty");

	virtual void Initialize() override;

	virtual ~OpenGLTexture2D() override;
protected:
	virtual void RefreshData() override;
	virtual void GenerateMipMaps() override;
	virtual void DeleteMipMaps() override;
	virtual void RefreshParameters() override;
};