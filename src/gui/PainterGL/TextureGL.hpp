#ifndef __TEXTUREGL_HPP__
#define __TEXTUREGL_HPP__

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

    TextureGL(int newhandle);
    
    int handle;
    float width, height;
    Rect2D rect;
};

#endif

