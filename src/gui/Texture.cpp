#include "Texture.hpp"

Texture::~Texture()
{
    SDL_FreeSurface(surface);
}
