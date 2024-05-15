#pragma once

#include "Core/Assets/Asset.h"
#include "Core/Rendering/Resource.h"
#include "Core/Rendering/Types.h"
#include <glm/vec3.hpp>

ED_ABSTRACT_CLASS(Texture) : public Asset, public Resource {
    ED_CLASS_BODY(Texture, Asset)
public:
	Texture(const std::string& name);

	virtual void Initialize() = 0;

    uint32_t GetID() const;

    virtual TextureType GetTextureType() const = 0;

    virtual void Resize(uint32_t width, uint32_t height, uint32_t depth) = 0;
    virtual void Resize(glm::u32vec3 size) = 0;

    virtual glm::u32vec3 GetSize() const = 0;

    void SetWrapS(WrapMode mode);
    WrapMode GetWrapS() const;

    void SetWrapT(WrapMode mode);
    WrapMode GetWrapT() const;

    void SetPixelFormat(PixelFormat format);
    PixelFormat GetPixelFormat() const;

    void SetFilteringMode(FilteringMode mode);
    FilteringMode GetFilteringMode() const;

	virtual void ResetState() override;

	virtual void Serialize(Archive& archive) override;

    bool IsInitialized() const;

    virtual ~Texture() = default;
protected:
    uint32_t m_Id = 0;
    bool m_bIsInitialized = false;

	WrapMode m_WrapS = WrapMode::Repeat;
	WrapMode m_WrapT = WrapMode::Repeat;

	PixelFormat m_PixelFormat = PixelFormat::RGB8F;
	FilteringMode m_FilteringMode = FilteringMode::Linear;

    virtual void RefreshData() = 0;
    virtual void RefreshParameters() = 0;
};