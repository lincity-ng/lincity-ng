#include <config.h>

#include "TextureGL.hpp"

#include <SDL_opengl.h>

TextureGL::TextureGL(int newhandle)
    : handle(newhandle)
{
}

TextureGL::~TextureGL()
{
    unsigned int handles[1] = { handle };
    glDeleteTextures(1, handles);
}

