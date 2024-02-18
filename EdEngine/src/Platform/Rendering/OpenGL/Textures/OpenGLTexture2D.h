#pragma once

#include "Core/Rendering/Textures/Texture2D.h"
#include "Core/Rendering/Textures/Texture.h"

class OpenGLTexture2D : public Texture2D {
public:
	OpenGLTexture2D(std::shared_ptr<Texture2DDescriptor> descriptor);

	virtual void Resize(uint32_t width, uint32_t height) override;

	virtual TextureType GetType() const override;

	virtual void SetData(const Texture2DData& inData) override;
	virtual void SetData(Texture2DData&& inData) override;

	virtual ~OpenGLTexture2D() override;
protected:
	virtual void Initialize() override;
};