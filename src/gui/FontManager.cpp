#include <config.h>

#include "FontManager.hpp"

#include <SDL_ttf.h>
#include <stdexcept>
#include <sstream>

#include "PhysfsStream/PhysfsSDL.hpp"

FontManager* fontManager = 0;

FontManager::FontManager()
{
}

FontManager::~FontManager()
{
    for(Fonts::iterator i = fonts.begin(); i != fonts.end(); ++i)
        TTF_CloseFont(i->second);
}

TTF_Font*
FontManager::getFont(Style style)
{
    FontInfo info;
    info.name = style.font_family;
    info.fontsize = (int) style.font_size;
    info.fontstyle = 0;
    if(style.italic)
        info.fontstyle |= TTF_STYLE_ITALIC;
    if(style.bold)
        info.fontstyle |= TTF_STYLE_BOLD;

    Fonts::iterator i = fonts.find(info);
    if(i != fonts.end())
        return i->second;
    
    std::string fontfile = "fonts/" + info.name + ".ttf";
    TTF_Font* font = TTF_OpenFontRW(getPhysfsSDLRWops(fontfile), 1,
            info.fontsize);
    if(!font) {
        std::stringstream msg;
        msg << "Error opening font '" << fontfile 
            << "': " << SDL_GetError();
        throw std::runtime_error(msg.str());
    }
    if(info.fontstyle != 0)
        TTF_SetFontStyle(font, info.fontstyle);

    fonts.insert(std::make_pair(info, font));
    return font;
}

