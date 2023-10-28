#pragma once

#include <glm/vec2.hpp>

#include "Texture2D.h"
#include "freetype/ftglyph.h"

class Character: public Texture2D
{
public:
    Character(FT_GlyphSlot glyph);

    glm::vec2 GetBearing() const { return m_Bearing; }
    glm::vec2 GetAdvance() const { return m_Advance; }
private:
    glm::vec2 m_Bearing;
    glm::vec2 m_Advance;
};
