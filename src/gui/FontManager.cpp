#include "FontManager.hpp"

#include <SDL_ttf.h>
#include <stdexcept>
#include <sstream>
#include <fstream>

TTF_Font*
FontManager::getFont(Style style)
{
    std::string fontfile = "data/fonts/" + style.font_family + ".ttf";
    
    // for some stupid reason SDL_TTF seems to crash if a font file doesn't
    // exist :-/, so we check for existance first...
    std::ifstream in(fontfile.c_str());
    if(!in.good()) {
        std::stringstream msg;
        msg << "Couldn't open fontfile '" << fontfile << "'";
        throw std::runtime_error(msg.str());
    }
    in.close();
        
    TTF_Font* font = TTF_OpenFont(fontfile.c_str(),
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

