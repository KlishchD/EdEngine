#pragma once

#include "Texture.h"

class Texture2D : public Texture {
public:
	uint32_t GetWidth() const;
	uint32_t GetHeight() const;

	virtual glm::u32vec3 GetSize() const;

	virtual void SetData(const Texture2DData& data) = 0;
	virtual void SetData(Texture2DData&& data) = 0;

	virtual void Resize(uint32_t width, uint32_t height, uint32_t depth) override;
	virtual void Resize(glm::u32vec3 size) override;
	virtual void Resize(uint32_t width, uint32_t height) = 0;
protected:
	Texture2D(std::shared_ptr<Texture2DDescriptor> descriptor);
};
