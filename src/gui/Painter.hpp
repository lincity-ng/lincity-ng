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

/**
 * @author Matthias Braun
 * @file Painter.hpp
 */

#ifndef __PAINTER_HPP__
#define __PAINTER_HPP__

#include <SDL.h>
#include <vector>
#include "Rect2D.hpp"
#include "TextureManager.hpp"
#include "Color.hpp"

/**
 * @class Painter
 * @brief This class is needed to perform drawing operations.
 *
 * It contains a stack of
 * trasnformations (currently only translation) which is applied to the drawing
 * operations. This is usefull for child widgets in the gui-component tree.
 */
class Painter
{
public:
    virtual ~Painter()
    { }

    virtual void drawTexture(const Texture* texture, const Vector2& pos) = 0;
    virtual void drawStretchTexture(Texture* texture, const Rect2D& rect) = 0;
    virtual void fillRectangle(const Rect2D& rect) = 0;
    virtual void drawRectangle(const Rect2D& rect) = 0;
    virtual void fillPolygon(int numberPoints, const Vector2* points) = 0;
    virtual void drawPolygon(int numberPoints, const Vector2* points) = 0;
    virtual void drawLine(const Vector2 pointA, const Vector2 pointB) = 0;

    virtual void pushTransform() = 0;
    virtual void popTransform() = 0;

    virtual void setClipRectangle(const Rect2D& rect) = 0;
    virtual void clearClipRectangle() = 0;
    //virtual Rect2D getClipRectangle() = 0;

    virtual void translate(const Vector2& vec) = 0;
    virtual void setFillColor(Color color) = 0;
    virtual void setLineColor(Color color) = 0;

    /** create a new painter instance that draws on a texture */
    virtual Painter* createTexturePainter(Texture* texture) = 0;
};

#endif

/** @file gui/Painter.hpp */

