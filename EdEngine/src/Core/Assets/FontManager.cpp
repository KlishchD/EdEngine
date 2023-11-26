#include "FontManager.h"
#include "Core/Macros.h"
#include "Font.h"

Font FontManager::LoadFont(const char* path, uint32_t width, uint32_t height) const
{
    return Font(m_Ft, path, width, height);
}

FontManager& FontManager::GetFontManager()
{
    static FontManager fontManager;
    return fontManager;
}

FontManager::~FontManager()
{
    FT_Done_FreeType(m_Ft);
}

FontManager::FontManager()
{
    if (FT_Init_FreeType(&m_Ft))
    {
		ED_LOG(FontManager, err, "Failed to initialize FreeType library")
        return;
    }
}
