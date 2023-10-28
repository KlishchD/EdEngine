#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>

class Font;

class FontManager
{
public:
    Font LoadFont(const char* path, uint32_t width, uint32_t height) const;

    static FontManager& GetFontManager();

    ~FontManager();
private:
    FT_Library m_Ft;

    FontManager();
};
