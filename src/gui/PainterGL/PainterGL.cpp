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
    glColor4ub( 0xff, 0xff, 0xff, 0xff );

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
    glColor4ub(fillColor.r, fillColor.g, fillColor.b, fillColor.a);
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
    glColor4ub(lineColor.r, lineColor.g, lineColor.b, lineColor.a);
    (void) rect;
    glDisable(GL_TEXTURE_2D);

    glBegin(GL_LINE_LOOP);
    glVertex2f(rect.p1.x, rect.p1.y);
    glVertex2f(rect.p1.x, rect.p2.y);
    glVertex2f(rect.p2.x, rect.p2.y);
    glVertex2f(rect.p2.x, rect.p1.y);
    glEnd();

    glEnable(GL_TEXTURE_2D);
}

void
PainterGL::fillPolygon(int numberPoints, const Vector2* points)
{
    glColor4ub(fillColor.r, fillColor.g, fillColor.b, fillColor.a);
    glDisable(GL_TEXTURE_2D);

    glBegin(GL_POLYGON);
    for( int i = 0; i < numberPoints; i++ )
        glVertex2f(points[i].x, points[i].y);
    glEnd();

    glEnable(GL_TEXTURE_2D);
}

void
PainterGL::drawPolygon(int numberPoints, const Vector2* points)
{
    glColor4ub(lineColor.r, lineColor.g, lineColor.b, lineColor.a);
    glDisable(GL_TEXTURE_2D);

    glBegin(GL_LINE_LOOP);
    for( int i = 0; i < numberPoints; i++ )
        glVertex2f(points[i].x, points[i].y);
    glEnd();

    glEnable(GL_TEXTURE_2D);
}

    



void
PainterGL::setFillColor(Color color)
{
    fillColor = color;
}

void
PainterGL::setLineColor(Color color)
{
    lineColor = color;
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
    //TODO: make this work
    GLdouble equation[4];
    equation[0] = rect.p1.x;
    equation[1] = rect.p1.y;
    equation[2] = rect.p2.x;
    equation[3] = rect.p2.y;
    glClipPlane( GL_CLIP_PLANE0, equation );
    glEnable( GL_CLIP_PLANE0 );
}

void
PainterGL::clearClipRectangle()
{
    glDisable( GL_CLIP_PLANE0 );
}

Painter*
PainterGL::createTexturePainter(Texture* texture)
{
    (void) texture;
    // TODO
    return 0;
}

