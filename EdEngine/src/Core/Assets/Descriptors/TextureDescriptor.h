#pragma once

#include "AssetDescriptor.h"
#include "Core/Assets/ImportParameters/TextureImportParameters.h"
#include "Core/Rendering/Textures/TextureData.h"

struct TextureDescriptor : public AssetDescriptor
{
	virtual TextureImportParameters* GetImportParameters() const = 0;
	virtual TextureData* GetData() const = 0;
};

struct Texture2DDescriptor : public TextureDescriptor
{
	Texture2DImportParameters ImportParameters;
	Texture2DData Data;

	virtual TextureImportParameters* GetImportParameters() const override;
	virtual TextureData* GetData() const override;
	virtual bool HasData() const override;
};

struct CubeTextureDescriptor : public TextureDescriptor
{
	CubeTextureImportParameters ImportParameters;
	CubeTextureData Data;

	virtual TextureImportParameters* GetImportParameters() const override;
	virtual TextureData* GetData() const override;
	virtual bool HasData() const override;
};

namespace boost
{
	namespace serialization
	{
		template <class Archive>
		void serialize(Archive& ar, Texture2DDescriptor& descriptor, uint32_t version)
		{
			ar & boost::serialization::base_object<AssetDescriptor>(descriptor);
			ar & descriptor.ImportParameters;
			
			if (!Archive::is_saving::value)
			{
				descriptor.Data.PixelSize = Types::GetPixelSize(descriptor.ImportParameters.Format);
			}

			if (descriptor.ShouldHaveData() || (Archive::is_saving::value && descriptor.HasData()))
			{
				ar & descriptor.Data;
			}
		}

		template <class Archive>
		void serialize(Archive& ar, CubeTextureDescriptor& descriptor, uint32_t version)
		{
			ar & boost::serialization::base_object<AssetDescriptor>(descriptor);
			ar & descriptor.ImportParameters;

			if (!Archive::is_saving::value)
			{
				descriptor.Data.PixelSize = Types::GetPixelSize(descriptor.ImportParameters.Format);
			}

			if (descriptor.ShouldHaveData() || (Archive::is_saving::value && descriptor.HasData()))
			{
				ar & descriptor.Data;
			}
		}
	}
}

BOOST_CLASS_EXPORT_KEY(Texture2DDescriptor)
BOOST_CLASS_VERSION(Texture2DDescriptor, 1)

BOOST_CLASS_EXPORT_KEY(CubeTextureDescriptor)
BOOST_CLASS_VERSION(CubeTextureDescriptor, 1)