#pragma once

#include "Asset.h"
#include "Texture.h"
#include "Utils/AssetUtils.h"

struct Texture2DImportParameters
{
	std::string ImagePath = "";

	int32_t WrapS = GL_REPEAT;
	int32_t WrapT = GL_REPEAT;
	int32_t InternalFormat = GL_RGBA8;
	int32_t ExternalFormat = GL_RGBA;
	int32_t Filtering = GL_LINEAR;
	int32_t DataType = GL_UNSIGNED_BYTE;
	
	static Texture2DImportParameters GetDefaultBaseColorTextureImportParameters(const std::string& path);
	static Texture2DImportParameters GetDefaultNormalTextureImportParameters(const std::string& path);
	static Texture2DImportParameters GetDefaultMetalicTextureImportParameters(const std::string& path);
	static Texture2DImportParameters GetDefaultRoughnessTextureImportParameters(const std::string& path);
};

struct Texture2DData
{
	Texture2DData();
	Texture2DData(int32_t width, int32_t height, int32_t channels, uint8_t* data);
	Texture2DData(Texture2DData&& data) noexcept;

	Texture2DData& operator=(const Texture2DData&) = default;
	
	int32_t Width;
	int32_t Height;
	int32_t Channels;

	uint8_t* Data = nullptr;
};

struct Texture2DDescriptor : public AssetDescriptor
{
	Texture2DImportParameters ImportParameters;
	Texture2DData TextureData;
};

namespace boost
{
	namespace serialization
	{
		template <class Archive>
		void serialize(Archive& ar, Texture2DImportParameters& parameters, uint32_t version)
		{
			ar & parameters.ImagePath;
			
			ar & parameters.WrapS;
			ar & parameters.WrapT;
			
			ar & parameters.InternalFormat;
			ar & parameters.ExternalFormat;
			
			ar & parameters.Filtering;
			ar & parameters.DataType;
		}
		
		template <class Archive>
		void serialize(Archive& ar, Texture2DData& data, uint32_t version)
		{
			ar & data.Width;
			ar & data.Height;
			ar & data.Channels;

			if (AssetUtils::UseFullDescriptor())
			{
				int32_t size = data.Width * data.Height * 4 * sizeof(uint8_t);
				if (!data.Data)
				{
					data.Data = static_cast<uint8_t*>(std::malloc(size));
				}
				ar & make_binary_object(data.Data, size); // TODO: Add proper size calculation base on channel pixel size ;)
			}
		}
		
		template <class Archive>
		void serialize(Archive& ar, Texture2DDescriptor& descriptor, uint32_t version)
		{
			ar & boost::serialization::base_object<AssetDescriptor>(descriptor);
			ar & descriptor.ImportParameters;
			ar & descriptor.TextureData;
		}
	}
}

class Texture2D : public Texture {
public:
	Texture2D(const std::shared_ptr<Texture2DDescriptor>& descriptor);
	Texture2D(const Texture2DImportParameters& parameters, const Texture2DData& data);

	virtual void Resize(uint32_t width, uint32_t height) override;
	
	static std::shared_ptr<Texture2D> GetWhiteTexture();
private:
	static inline std::shared_ptr<Texture2D> WhiteTexture;

	void Init(const Texture2DImportParameters& parameters, const Texture2DData& data);
};

BOOST_CLASS_EXPORT_KEY(Texture2DDescriptor)
BOOST_CLASS_VERSION(Texture2DDescriptor, 1)