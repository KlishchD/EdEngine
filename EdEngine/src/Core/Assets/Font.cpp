#include "Font.h"
#include "Core/Macros.h"
#include "Character.h"

Font::Font(const FT_Library& library, const char* path, uint32_t width, uint32_t height): m_Library(library), m_Width(width), m_Height(height)
{
    if (FT_New_Face(library, path, 0, &m_Face))
    {
        ED_LOG(Font, err, "Failed to load font {}", path)
    }
        
    FT_Set_Pixel_Sizes(m_Face, width, height);

    for (uint8_t i = 0; i < 128; ++i)
    {
        if (FT_Load_Char(m_Face, i, FT_LOAD_RENDER))
        {
			ED_LOG(Font, err, "FREETYTPE: Failed to load Glyph for char {}", (char) i)
            continue;
        }

        std::shared_ptr<Character> texture = std::make_shared<Character>(m_Face->glyph);
           
        m_Textures[i] = texture;
    }
}

Font::~Font()
{
    FT_Done_Face(m_Face);
}
