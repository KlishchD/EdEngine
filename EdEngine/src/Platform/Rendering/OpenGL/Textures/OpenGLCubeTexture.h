#pragma once

#include "Core/Rendering/Textures/CubeTexture.h"

class OpenGLCubeTexture : public CubeTexture
{
public:
	OpenGLCubeTexture(std::shared_ptr<CubeTextureDescriptor> descriptor);

	virtual void Resize(uint32_t size) override;
	virtual TextureType GetType() const override;

	virtual ~OpenGLCubeTexture() override;
protected:
	virtual void Initialize(TextureImportParameters* inParameters, TextureData* inData) override;
};