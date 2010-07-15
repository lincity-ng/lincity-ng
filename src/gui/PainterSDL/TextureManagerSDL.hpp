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
#ifndef __TEXTUREMANAGERSDL_HPP__
#define __TEXTUREMANAGERSDL_HPP__

#include "gui/TextureManager.hpp"

/**
 * This handles the creation and sharing of textures.
 * 
 * A texture is a wrapper around a pixmaps. Using a wrapper allows to make use
 * of hardware acceleration in some scenarios (for example in opengl where the
 * pixmaps can get uploaded into the gfx cards memory).
 */
class TextureManagerSDL : public TextureManager
{
public:
    TextureManagerSDL();
    virtual ~TextureManagerSDL();

    Texture* create(SDL_Surface* surface);
};

#endif

/** @file gui/PainterSDL/TextureManagerSDL.hpp */

