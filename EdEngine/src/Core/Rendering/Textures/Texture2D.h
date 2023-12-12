#pragma once

#include "Texture.h"
#include "Utils/AssetUtils.h"

namespace boost
{
	namespace serialization
	{
		template <class Archive>
		void serialize(Archive& ar, Texture2DImportParameters& parameters, uint32_t version)
		{
			ar & parameters.Path;
			
			ar & parameters.WrapS;
			ar & parameters.WrapT;
			ar & parameters.Format;
			ar & parameters.Filtering;
		}
		
		template <class Archive>
		void serialize(Archive& ar, Texture2DData& data, uint32_t version)
		{
			ar & data.Width;
			ar & data.Height;

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
			ar & descriptor.Data;
		}
	}
}

class Texture2D : public Texture {
public:
	uint32_t GetWidth() const;
	uint32_t GetHeight() const;

	virtual void Resize(uint32_t width, uint32_t height) = 0;
protected:
	Texture2D(const std::shared_ptr<TextureDescriptor>& descriptor);
};

BOOST_CLASS_EXPORT_KEY(Texture2DDescriptor)
BOOST_CLASS_VERSION(Texture2DDescriptor, 1)