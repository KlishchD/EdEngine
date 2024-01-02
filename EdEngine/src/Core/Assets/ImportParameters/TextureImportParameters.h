#pragma once

#include <string>
#include "Core/Rendering/Types.h"

struct TextureImportParameters
{
	std::string Path;

	WrapMode WrapS = WrapMode::Repeat;
	WrapMode WrapT = WrapMode::Repeat;

	PixelFormat Format = PixelFormat::RGB8F;
	FilteringMode Filtering = FilteringMode::Linear;
};

struct Texture2DImportParameters : public TextureImportParameters
{
};

struct CubeTextureImportParameters: public TextureImportParameters
{
	WrapMode WrapR = WrapMode::Repeat;
};

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
		void serialize(Archive& ar, CubeTextureImportParameters& parameters, uint32_t version)
		{
			ar & parameters.Path;

			ar & parameters.WrapS;
			ar & parameters.WrapT;
			ar & parameters.WrapR;
			ar & parameters.Format;
			ar & parameters.Filtering;
		}
	}
}

