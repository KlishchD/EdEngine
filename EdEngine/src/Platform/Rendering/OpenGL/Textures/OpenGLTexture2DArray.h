#pragma once

#include "Core/Rendering/Textures/Texture2DArray.h"

class OpenGLTexture2DArray : public Texture2DArray
{
public:
	OpenGLTexture2DArray(const std::string& name);
	
	virtual void Initialize() override;

	virtual ~OpenGLTexture2DArray() override;
protected:
	virtual void RefreshData() override;
	virtual void RefreshParameters() override;
};