#include "Texture2DAssetImporter.h"
#include "Core/Assets/AssetManager.h"
#include "Core/Rendering/Textures/Texture2D.h"
#include "Helpers/RenderingHelper.h"
#include "Helpers/stb_image.h"

Texture2DImporter::Texture2DImporter(std::shared_ptr<AssetManager> manager) : AssetImporter(manager)
{

}

std::shared_ptr<Asset> Texture2DImporter::Import(std::shared_ptr<AssetImportParameters> inParameters)
{
	std::shared_ptr<Texture2DImportParameters> parameters = std::static_pointer_cast<Texture2DImportParameters>(inParameters);

	const std::string& texturePath = parameters->Path;
	if (texturePath.empty()) return nullptr;

	stbi_set_flip_vertically_on_load(true);

	int32_t width;
	int32_t height;
	int32_t channals;
	int32_t pixelSize = Types::GetPixelSize(parameters->Format);

	uint8_t* imageData = stbi_load(texturePath.c_str(), &width, &height, &channals, Types::GetChannelNumber(parameters->Format));

	ED_ASSERT(imageData, "Image data cannot be null")

	std::string name = std::filesystem::path(texturePath).filename().string();
	Texture2DData data(width, height, imageData, width * height * pixelSize, true);
	std::shared_ptr<Texture2D> texture = RenderingHelper::CreateTexture2D(name, parameters, std::move(data));

	std::string savePath = FilesHelper::GetSavePath(texturePath, AssetType::Texture2D);

	Archive archive(savePath, ArchiveMode::Write);
	archive & texture;

	m_Manager->RegisterAsset(texture, savePath);

	return texture;
}

std::vector<std::shared_ptr<Asset>> Texture2DImporter::ImportMultiple(std::shared_ptr<AssetImportParameters> parameters)
{
	return { Import(parameters) };
}
