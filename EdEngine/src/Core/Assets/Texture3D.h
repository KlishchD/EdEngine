#pragma once

#include "Asset.h"
#include "Texture.h"

struct Texture3DImportParameters
{
    std::string ImagePath;

    int32_t WrapS;
    int32_t WrapT;
    int32_t WrapR;
    int32_t InternalFormat;
    int32_t ExternalFormat;
    int32_t Filtering;
    int32_t DataType;
};

struct Texture3DData
{
    int32_t Size;
    int32_t Channels;

    uint8_t* Data[6];
};

struct Texture3DDescriptor : public AssetDescriptor
{
    Texture3DImportParameters ImportParameters;
    Texture3DData TextureData;
};

class Texture3D: public Texture
{
public:
    Texture3D(const std::shared_ptr<Texture3DDescriptor>& descriptor);
    Texture3D(const Texture3DImportParameters& parameters, const Texture3DData& data);

    virtual void Resize(uint32_t width, uint32_t height) override;
private:
    void Init(const Texture3DImportParameters& parameters, const Texture3DData& data);
};
