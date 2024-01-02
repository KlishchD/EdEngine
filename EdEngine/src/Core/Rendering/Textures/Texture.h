#pragma once

#include "Core/Assets/Asset.h"
#include "Core/Assets/Descriptors/TextureDescriptor.h"

class Texture : public Asset {
public:
    uint32_t GetID() const;

    virtual TextureType GetType() const = 0;

    PixelFormat GetPixelFormat() const;

    virtual ~Texture() = default;
protected:
    uint32_t m_Id = 0;

    Texture(const std::shared_ptr<TextureDescriptor>& descriptor);


	const TextureImportParameters& GetImportParameters() const;
	TextureData& GetData() const;

    void Initialize();
	virtual void Initialize(TextureImportParameters* parameters, TextureData* data);
};