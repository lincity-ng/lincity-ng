#include <config.h>

#include "PainterGL.hpp"

#include <SDL_opengl.h>
#include <iostream>
#include <typeinfo>

#include "TextureGL.hpp"

PainterGL::PainterGL()
{
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

PainterGL::~PainterGL()
{
    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);       
}

void
PainterGL::drawTexture(const Texture* texture, const Vector2& pos)
{
    Rect2D rect(pos, pos + Vector2(texture->getWidth(), texture->getHeight()));
    drawStretchTexture(texture, rect);
}

void
PainterGL::drawStretchTexture(const Texture* texture, const Rect2D& rect)
{
    assert(typeid(*texture) == typeid(TextureGL));
    const TextureGL* textureGL = static_cast<const TextureGL*> (texture);

    if(texture == 0) {
        std::cerr << "Trying to render 0 texture.";
#ifdef DEBUG
        assert(false);
#endif
        return;
    }

    const Rect2D& r = textureGL->rect;
    glBindTexture(GL_TEXTURE_2D, textureGL->handle);
    glBegin(GL_QUADS);
    glTexCoord2f(r.p1.x, r.p1.y);
    glVertex3f(rect.p1.x, rect.p1.y, 0);
    glTexCoord2f(r.p1.x, r.p2.y);
    glVertex3f(rect.p1.x, rect.p2.y, 0);
    glTexCoord2f(r.p2.x, r.p2.y);
    glVertex3f(rect.p2.x, rect.p2.y, 0);
    glTexCoord2f(r.p2.x, r.p1.y);
    glVertex3f(rect.p2.x, rect.p1.y, 0);
    glEnd();
}

void
PainterGL::fillRectangle(const Rect2D& rect)
{
    glDisable(GL_TEXTURE_2D);

    //printf("Drawrect: %f %f %f %f.\n", rect.p1.x, rect.p1.y, rect.p2.x, rect.p2.y);

    glBegin(GL_POLYGON);
    glVertex2f(rect.p1.x, rect.p1.y);
    glVertex2f(rect.p1.x, rect.p2.y);
    glVertex2f(rect.p2.x, rect.p2.y);
    glVertex2f(rect.p2.x, rect.p1.y);
    glEnd();

    glEnable(GL_TEXTURE_2D);
}

void
PainterGL::drawRectangle(const Rect2D& rect)
{
    // TODO
}

void
PainterGL::setFillColor(Color color)
{
    glColor4ub(color.r, color.g, color.b, color.a);
}

void
PainterGL::setLineColor(Color color)
{
    // TODO
}

void
PainterGL::translate(const Vector2& vec)
{
    glTranslatef(vec.x, vec.y, 0);
}

void
PainterGL::pushTransform()
{
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();    
}

void
PainterGL::popTransform()
{
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void
PainterGL::setClipRectangle(const Rect2D& rect)
{
    // TODO
}

void
PainterGL::clearClipRectangle()
{
    // TODO
}

Painter*
PainterGL::createTexturePainter(Texture* texture)
{
    // TODO
    return 0;
}

