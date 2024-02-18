#pragma once

#include "Texture.h"

class Texture2DArray : public Texture
{
public:
	uint32_t GetWidth() const;
	uint32_t GetHeight() const;
	uint32_t GetDepth() const;

	virtual glm::u32vec3 GetSize() const override;

	virtual TextureType GetType() const override;

	virtual void Resize(glm::u32vec3 size) override;
	virtual void Resize(uint32_t width, uint32_t height);
	virtual void Resize(uint32_t width, uint32_t height, uint32_t depth) = 0;
protected:
	Texture2DArray(std::shared_ptr<Texture2DArrayDescriptor> descriptor);
};