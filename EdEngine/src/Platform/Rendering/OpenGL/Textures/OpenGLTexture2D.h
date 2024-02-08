#pragma once

#include "Core/Rendering/Textures/Texture2D.h"
#include "Core/Rendering/Textures/Texture.h"

class OpenGLTexture2D : public Texture2D {
public:
	OpenGLTexture2D(std::shared_ptr<TextureDescriptor> descriptor);

	virtual void Resize(uint32_t width, uint32_t height) override;
	virtual TextureType GetType() const override;

	virtual void SetData(const Texture2DData& data) override;
	virtual void SetData(Texture2DData&& data) override;

	virtual ~OpenGLTexture2D() override;
protected:
	virtual void Initialize(TextureImportParameters* parameters, TextureData* data) override;
};