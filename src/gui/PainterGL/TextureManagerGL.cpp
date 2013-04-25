/*
Copyright (C) 2005 Matthias Braun <matze@braunis.de>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include <config.h>

#include "TextureManagerGL.hpp"

#include <SDL_opengl.h>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "TextureGL.hpp"

TextureManagerGL::TextureManagerGL()
{
}

TextureManagerGL::~TextureManagerGL()
{
}

static int powerOfTwo(int val) {
    int result = 1;
    while(result < val)
        result *= 2;
    return result;
}

Texture*
TextureManagerGL::create(SDL_Surface* image)
{
    int texture_w = powerOfTwo(image->w);
    int texture_h = powerOfTwo(image->h);

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    SDL_Surface* convert = SDL_CreateRGBSurface(SDL_SWSURFACE,
            texture_w, texture_h, 32,
            0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
    SDL_Surface* convert = SDL_CreateRGBSurface(SDL_SWSURFACE,
        texture_w, texture_h, 32,
        0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif
    if(convert == 0) {
        std::ostringstream msg;
        msg << "Couldn't convert SDL_Surface while creating texture"
            << " (out of memory?): " << SDL_GetError();
        throw std::runtime_error(msg.str());
    }
    SDL_SetAlpha(image, 0, 0);
    SDL_BlitSurface(image, 0, convert, 0);

    GLuint handle;
    glGenTextures(1, &handle);

    SDL_PixelFormat* format = convert->format;

    glBindTexture(GL_TEXTURE_2D, handle);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, convert->pitch/format->BytesPerPixel);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexImage2D(GL_TEXTURE_2D, 0, format->BytesPerPixel,
            convert->w, convert->h, 0, GL_RGBA,
            GL_UNSIGNED_BYTE, convert->pixels);

	GLfloat yellow[4] = { 1.0f, 1.0f, 0.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, yellow);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);

    TextureGL* texture = new TextureGL(handle);
    texture->rect = Rect2D(0, 0,
            (float) image->w/(float) texture_w,
            (float) image->h/(float) texture_h);
    texture->width = image->w;
    texture->height = image->h;

    SDL_FreeSurface(image);
    SDL_FreeSurface(convert);
    return texture;
}


/** @file gui/PainterGL/TextureManagerGL.cpp */

