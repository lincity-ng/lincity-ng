#ifndef __PAINTER_HPP__
#define __PAINTER_HPP__

#include <SDL.h>
#include <vector>
#include "Rect2D.hpp"
#include "TextureManager.hpp"
#include "Color.hpp"

/**
 * This class is needed to perform drawing operations. It contains a stack of
 * trasnformations (currently only translation) which is applied to the drawing
 * operations. This is usefull for child widgets in the gui-component tree.
 */
class Painter
{
public:
    virtual void drawTexture(const Texture* texture, const Vector2& pos) = 0;
    virtual void drawStretchTexture(const Texture* texture, const Rect2D& rect) = 0;
    virtual void fillRectangle(const Rect2D& rect) = 0;
    virtual void drawRectangle(const Rect2D& rect) = 0;
    //virtual void fillDiamond(Rect2D rect) = 0;

    virtual void pushTransform() = 0;
    virtual void popTransform() = 0;

    virtual void setClipRectangle(const Rect2D& rect) = 0;
    virtual void clearClipRectangle() = 0;

    virtual void translate(const Vector2& vec) = 0;
    virtual void setFillColor(Color color) = 0;
    virtual void setLineColor(Color color) = 0;

    /** create a new painter instance that draws on a texture */
    virtual Painter* createTexturePainter(Texture* texture) = 0;
};

#endif
