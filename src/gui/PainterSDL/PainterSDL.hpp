#ifndef __PAINTERSDL_HPP__
#define __PAINTERSDL_HPP__

#include <SDL.h>
#include <vector>
#include "gui/Rect2D.hpp"
#include "gui/TextureManager.hpp"
#include "gui/Color.hpp"
#include "gui/Painter.hpp"

class TextureSDL;

/**
 * This class is needed to perform drawing operations. It contains a stack of
 * trasnformations (currently only translation) which is applied to the drawing
 * operations. This is usefull for child widgets in the gui-component tree.
 */
class PainterSDL : public Painter
{
public:
    PainterSDL(SDL_Surface* target);
    virtual ~PainterSDL();
    
    void drawTexture(const Texture* texture, const Vector2& pos);
    void drawStretchTexture(const Texture* texture, const Rect2D& rect);
    void fillRectangle(const Rect2D& rect);
    void drawRectangle(const Rect2D& rect);
    void fillPolygon(int numberPoints, const Vector2* points);
    void drawPolygon(int numberPoints, const Vector2* points);
    void drawLine(const Vector2 pointA, const Vector2 pointB);

    void pushTransform();
    void popTransform();

    void setClipRectangle(const Rect2D& rect);
    void clearClipRectangle();

    void translate(const Vector2& vec);
    void setFillColor(Color color);
    void setLineColor(Color color);

    Painter* createTexturePainter(Texture* texture);
    
private:
    PainterSDL(TextureSDL* texture);
    
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
    Color fillColor,lineColor;
};

#endif
