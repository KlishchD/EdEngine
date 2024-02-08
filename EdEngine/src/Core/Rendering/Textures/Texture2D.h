#pragma once

#include "Texture.h"

class Texture2D : public Texture {
public:
	uint32_t GetWidth() const;
	uint32_t GetHeight() const;

	virtual void SetData(const Texture2DData& data) = 0;
	virtual void SetData(Texture2DData&& data) = 0;

	virtual void Resize(uint32_t width, uint32_t height) = 0;
protected:
	Texture2D(std::shared_ptr<TextureDescriptor> descriptor);
};
