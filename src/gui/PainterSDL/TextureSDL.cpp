#include <config.h>

#include "TextureSDL.hpp"

TextureSDL::~TextureSDL()
{
    SDL_FreeSurface(surface);
}
