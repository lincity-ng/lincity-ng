#include <config.h>

#include "FontManager.hpp"

#include <SDL_ttf.h>
#include <stdexcept>
#include <sstream>
#include <fstream>

#include "PhysfsStream/PhysfsSDL.hpp"

TTF_Font*
FontManager::getFont(Style style)
{
    std::string fontfile = "fonts/" + style.font_family + ".ttf";
    
    TTF_Font* font = TTF_OpenFontRW(getPhysfsSDLRWops(fontfile), 1,
            (int) style.font_size);
    if(!font) {
        std::stringstream msg;
        msg << "Error opening font '" << fontfile << "': " << SDL_GetError();
        throw std::runtime_error(msg.str());
    }
    int fontstyle = 0;
    if(style.italic)
        fontstyle |= TTF_STYLE_ITALIC;
    if(style.bold)
        fontstyle |= TTF_STYLE_BOLD;

    if(fontstyle != 0)
        TTF_SetFontStyle(font, fontstyle);

    return font;
}

