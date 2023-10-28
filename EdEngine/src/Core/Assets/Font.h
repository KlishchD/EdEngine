#pragma once

#include "Core/Ed.h"

#include "Character.h"

class Font
{
public:
    Font(const FT_Library& library, const char* path, uint32_t width, uint32_t height);

    uint32_t GetWidth() const { return m_Width; }
    uint32_t GetHeight() const { return m_Height; }
    std::shared_ptr<Character> GetCharacterTexture(char c) const { return m_Textures.at(c); }
    
    ~Font();
private:
    const FT_Library& m_Library;
    FT_Face m_Face;
    uint32_t m_Width;
    uint32_t m_Height;
    std::map<char, std::shared_ptr<Character>> m_Textures;
};
