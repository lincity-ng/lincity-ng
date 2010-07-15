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
#ifndef __TEXTUREGL_HPP__
#define __TEXTUREGL_HPP__

#include <SDL_opengl.h>
#include "gui/Rect2D.hpp"
#include "gui/Texture.hpp"

class TextureGL : public Texture
{
public:
    virtual ~TextureGL();

    float getWidth() const
    {
        return width;
    }

    float getHeight() const
    {
        return height;
    }

private:
    friend class PainterGL;
    friend class TextureManagerGL;

    TextureGL(GLuint newhandle);
    
    GLuint handle;
    float width, height;
    Rect2D rect;
};

#endif


/** @file gui/PainterGL/TextureGL.hpp */

