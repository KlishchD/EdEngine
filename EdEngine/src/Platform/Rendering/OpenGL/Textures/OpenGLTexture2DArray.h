#pragma once

#include "Core/Rendering/Textures/Texture2DArray.h"

class OpenGLTexture2DArray : public Texture2DArray
{
public:
	OpenGLTexture2DArray(std::shared_ptr<Texture2DArrayDescriptor> descpriptor);

	virtual void Resize(uint32_t width, uint32_t height, uint32_t depth);
protected:
	virtual void Initialize() override;
};