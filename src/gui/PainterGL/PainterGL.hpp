#ifndef __PAINTERGL_HPP__
#define __PAINTERGL_HPP__

#include <SDL.h>
#include <vector>
#include "gui/Rect2D.hpp"
#include "gui/TextureManager.hpp"
#include "gui/Color.hpp"
#include "gui/Painter.hpp"

class TextureGL;

class PainterGL : public Painter
{
public:
    PainterGL();
    virtual ~PainterGL();
    
    void drawTexture(const Texture* texture, const Vector2& pos);
    void drawStretchTexture(const Texture* texture, const Rect2D& rect);
    void fillRectangle(const Rect2D& rect);
    void drawRectangle(const Rect2D& rect);
    void fillDiamond(const Rect2D& rect);

    void pushTransform();
    void popTransform();

    void setClipRectangle(const Rect2D& rect);
    void clearClipRectangle();

    void translate(const Vector2& vec);
    void setFillColor(Color color);
    void setLineColor(Color color);

    Painter* createTexturePainter(Texture* texture);
    
private:
    PainterGL(TextureGL* texture);
    
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

    Color fillColor,lineColor;
};

#endif
