#pragma once

#include "Texture.h"
#include "Core/Assets/ImportParameters/TextureImportParameters.h"
#include "TextureData.h"

ED_ABSTRACT_CLASS(Texture2D) : public Texture {
	ED_CLASS_BODY(Texture2D, Texture)
public:
	Texture2D(const std::string& name);

	virtual AssetType GetType() const override;

	uint32_t GetWidth() const;
	uint32_t GetHeight() const;

	virtual glm::u32vec3 GetSize() const;

	void SetData(const Texture2DData& data);
	void SetData(Texture2DData&& data);

	void SetMipMapsEnabled(bool enabled);
	bool AreMipMapsEnabled() const;

	virtual void Resize(uint32_t width, uint32_t height, uint32_t depth) override;
	virtual void Resize(glm::u32vec3 size) override;
	void Resize(uint32_t width, uint32_t height);

	virtual void ResetState() override;

	virtual void Serialize(Archive& archive) override;
	virtual void SerializeData(Archive& archive) override;

	virtual TextureType GetTextureType() const override;
protected:
	Texture2DData m_Data;

	bool m_bMipMapsEnabled = false;

	virtual void GenerateMipMaps() = 0;
	virtual void DeleteMipMaps() = 0;
};
