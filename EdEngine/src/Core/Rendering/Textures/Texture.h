#pragma once

#include "Core/Assets/Asset.h"
#include "Core/Assets/Descriptors/TextureDescriptor.h"
#include "Core/Rendering/Resource.h"

class Texture : public Asset, public Resource {
public:
    uint32_t GetID() const;

    virtual TextureType GetType() const = 0;

    virtual void Resize(uint32_t width, uint32_t height, uint32_t depth) = 0;
    virtual void Resize(glm::u32vec3 size) = 0;

    virtual glm::u32vec3 GetSize() const = 0;

    PixelFormat GetPixelFormat() const;

    virtual ~Texture() = default;
protected:
    uint32_t m_Id = 0;

    Texture(std::shared_ptr<AssetDescriptor> descriptor);

    virtual void Initialize();
};