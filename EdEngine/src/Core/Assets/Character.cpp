#include "Character.h"

Character::Character(FT_GlyphSlot glyph): Texture2D({"", GL_CLAMP_TO_BORDER, GL_CLAMP_TO_BORDER, GL_RED, GL_RED, GL_LINEAR, GL_UNSIGNED_BYTE}, { static_cast<int>(glyph->bitmap.width), static_cast<int>(glyph->bitmap.rows), 1, glyph->bitmap.buffer }), m_Bearing(glyph->bitmap_left, glyph->bitmap_top), m_Advance(glyph->advance.x, glyph->advance.y)
{
    
}
