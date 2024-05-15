#pragma once

#include "Texture.h"
#include "TextureData.h"

ED_ABSTRACT_CLASS(Texture2DArray) : public Texture
{
	ED_CLASS_BODY(Texture2DArray, Texture)
public:
	Texture2DArray(const std::string& name);

	virtual AssetType GetType() const override;

	uint32_t GetWidth() const;
	uint32_t GetHeight() const;
	uint32_t GetDepth() const;

	virtual glm::u32vec3 GetSize() const override;

	virtual void SetData(const Texture2DArrayData& data);
	virtual void SetData(Texture2DArrayData&& data);

	virtual TextureType GetTextureType() const override;

	virtual void Resize(glm::u32vec3 size) override;
	virtual void Resize(uint32_t width, uint32_t height);
	virtual void Resize(uint32_t width, uint32_t height, uint32_t depth) override;

	virtual void SerializeData(Archive& archive) override;
protected:
	Texture2DArrayData m_Data;
};