#include "Character.h"
#include "Helpers/RenderingHelper.h"

Character::Character(FT_GlyphSlot glyph): m_Bearing(glyph->bitmap_left, glyph->bitmap_top), m_Advance(glyph->advance.x, glyph->advance.y)
{
	m_Texture = RenderingHelper::CreateTexture2D("Character");

    m_Texture->SetWrapS(WrapMode::ClampToBorder);
    m_Texture->SetWrapT(WrapMode::ClampToBorder);

    m_Texture->SetPixelFormat(PixelFormat::R8F);
    m_Texture->SetFilteringMode(FilteringMode::Linear);

    Texture2DData data(glyph->bitmap.width, glyph->bitmap.rows, glyph->bitmap.buffer, glyph->bitmap.width * glyph->bitmap.rows * sizeof(uint8_t), false);
    m_Texture->SetData(std::move(data));

    m_Texture->Initialize();
}

std::shared_ptr<Texture2D> Character::GetTexture() const
{
    return m_Texture;
}
