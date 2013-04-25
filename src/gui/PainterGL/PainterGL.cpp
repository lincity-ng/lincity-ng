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
#include <config.h>

#include "PainterGL.hpp"

#include <SDL.h>
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
PainterGL::drawTextureRect(const Texture* texture, const Rect2D& rect)
{
    const TextureGL* textureGL = static_cast<const TextureGL*> (texture);
    const Rect2D& r = textureGL->rect;

    glColor4ub( 0xff, 0xff, 0xff, 0xff );

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
PainterGL::drawTexture(const Texture* texture, const Vector2& pos)
{
    Rect2D rect(pos, pos + Vector2(texture->getWidth(), texture->getHeight()));
    drawTextureRect(texture, rect);
}

void
PainterGL::drawStretchTexture(Texture* texture, const Rect2D& rect)
{
    assert(typeid(*texture) == typeid(TextureGL));

    if(texture == 0) {
        std::cerr << "Trying to render 0 texture.";
#ifdef DEBUG
        assert(false);
#endif
        return;
    }
    drawTextureRect(texture, rect);
}

void
PainterGL::drawLine( const Vector2 pointA, const Vector2 pointB )
{
    glColor4ub(lineColor.r, lineColor.g, lineColor.b, lineColor.a);
    glDisable(GL_TEXTURE_2D);

    glBegin(GL_LINES);
    glVertex2f(pointA.x, pointA.y);
    glVertex2f(pointB.x, pointB.y);
    glEnd();

    glEnable(GL_TEXTURE_2D);
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
    GLfloat matrix[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, matrix);

    int screenHeight = SDL_GetVideoSurface()->h;
    glViewport((GLint) (rect.p1.x + matrix[12]),
               (GLint) (screenHeight - rect.getHeight() - (rect.p1.y + matrix[13])),
               (GLsizei) rect.getWidth(),
               (GLsizei) rect.getHeight());
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(rect.p1.x + matrix[12], rect.p1.x + matrix[12] + rect.getWidth(),
            rect.p1.y + matrix[13] + rect.getHeight(),
            rect.p1.y + matrix[13], -1, 1);
}

void
PainterGL::clearClipRectangle()
{
    int width = SDL_GetVideoSurface()->w;
    int height = SDL_GetVideoSurface()->h;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
}

Painter*
PainterGL::createTexturePainter(Texture* texture)
{
    (void) texture;
    // TODO
    return 0;
}


/** @file gui/PainterGL/PainterGL.cpp */

