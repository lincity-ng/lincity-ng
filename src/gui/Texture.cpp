#include <config.h>

#include "Texture.hpp"

Texture::~Texture()
{
    SDL_FreeSurface(surface);
}
