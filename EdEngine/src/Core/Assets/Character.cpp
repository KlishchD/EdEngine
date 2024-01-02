#include "Character.h"
#include "ImportParameters/TextureImportParameters.h"
#include "Core/Rendering/Textures/TextureData.h"
#include "Utils/RenderingHelper.h"

Character::Character(FT_GlyphSlot glyph): m_Bearing(glyph->bitmap_left, glyph->bitmap_top), m_Advance(glyph->advance.x, glyph->advance.y)
{
    Texture2DImportParameters parameters;
    parameters.WrapS = WrapMode::ClampToBorder;
    parameters.WrapT = WrapMode::ClampToBorder;
    parameters.Format = PixelFormat::R8F;
    parameters.Filtering = FilteringMode::Linear;

    Texture2DData data;
    data.Width = glyph->bitmap.width;
    data.Height = glyph->bitmap.rows;
    data.Data = glyph->bitmap.buffer;

    m_Texture = RenderingHelper::CreateTexture2D(parameters, data, "character");
}

std::shared_ptr<Texture2D> Character::GetTexture() const
{
    return m_Texture;
}
