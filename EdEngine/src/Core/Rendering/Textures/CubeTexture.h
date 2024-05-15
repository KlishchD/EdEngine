#pragma once

#include "Texture.h"
#include "TextureData.h"

ED_ABSTRACT_CLASS(CubeTexture) : public Texture
{
    ED_CLASS_BODY(CubeTexture, Texture)
public:
    CubeTexture(const std::string& name);

	virtual AssetType GetType() const override;

    glm::u32vec3 GetSize() const override;

    void SetWrapR(WrapMode mode);
    WrapMode GetWrapMode() const;

	virtual void SetData(const CubeTextureData& data);
	virtual void SetData(CubeTextureData&& data);

	virtual TextureType GetTextureType() const override;

    virtual void Resize(uint32_t width, uint32_t height, uint32_t depth) override;
    virtual void Resize(glm::u32vec3 size) override;
    virtual void Resize(uint32_t size);

	virtual void ResetState() override;

    virtual void Serialize(Archive& archive) override;
    virtual void SerializeData(Archive& archive) override;
protected:
    CubeTextureData m_Data;

    WrapMode m_WrapR = WrapMode::Repeat;
};
