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
    Painter(SDL_Surface* target);
    Painter(Texture* texture);
    ~Painter();
    
    void drawTexture(const Texture* texture, Rect2D rect);
    void drawStretchTexture(const Texture* texture, Rect2D rect);
    void fillRectangle(Rect2D rect);
    void drawRectangle(Rect2D rect);
    void fillDiamond(Rect2D rect);

    void pushTransform();
    void popTransform();

    void setClipRectangle(Rect2D rect);
    void clearClipRectangle();

    void translate(const Vector2& vec);
    void setFillColor(Color color);
    void setLineColor(Color color);
private:
    class Transform
    {
    public:
        Vector2 translation;

        Vector2 apply(const Vector2& v) const
        {
            return v - translation;
        }
    };

    // the stack used by push-/popTransform
    std::vector<Transform> transformStack;
    // the currently active transform
    Transform transform;

    SDL_Surface* target;
    //Uint32 fillColor;
    Color fillColor,lineColor;
};

#endif
