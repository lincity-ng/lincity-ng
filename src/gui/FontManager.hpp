#ifndef __FONTMANAGER_HPP__
#define __FONTMANAGER_HPP__

#include "Style.hpp"
#include <SDL_ttf.h>

class FontManager
{
public:
    TTF_Font* getFont(Style style);
};

#endif

