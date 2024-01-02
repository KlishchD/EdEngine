#pragma once

#include "Texture.h"

class Texture2D : public Texture {
public:
	uint32_t GetWidth() const;
	uint32_t GetHeight() const;

	virtual void Resize(uint32_t width, uint32_t height) = 0;
protected:
	Texture2D(const std::shared_ptr<TextureDescriptor>& descriptor);
};
