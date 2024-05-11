#pragma once

#include <memory>
#include <glm/vec2.hpp>
#include "freetype/ftglyph.h"
#include "Core/Rendering/Textures/Texture2D.h"

class Character
{
public:
    Character(FT_GlyphSlot glyph);

    glm::vec2 GetBearing() const { return m_Bearing; }
    glm::vec2 GetAdvance() const { return m_Advance; }

    std::shared_ptr<Texture2D> GetTexture() const;
private:
    std::shared_ptr<Texture2D> m_Texture;
    glm::vec2 m_Bearing;
    glm::vec2 m_Advance;
};
