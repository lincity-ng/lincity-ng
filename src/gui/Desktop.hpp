/*
Copyright (C) 2005 Matthias Braun <matze@braunis.de>
Copyright (C) 2024 David Bears <dbear4q@gmail.com>

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
 * @file Desktop.hpp
 */

#ifndef __DESKTOP_HPP__
#define __DESKTOP_HPP__

#include <SDL.h>          // for SDL_Cursor, SDL_SystemCursor
#include <vector>         // for vector

#include "Component.hpp"  // for Component
#include "Rect2D.hpp"     // for Rect2D
#include "Vector2.hpp"    // for Vector2

namespace xmlpp {
class TextReader;
}  // namespace xmlpp

/**
 * @class Desktop
 */
class Desktop : public Component
{
public:
    Desktop();
    virtual ~Desktop();

    void parse(xmlpp::TextReader& reader);

    void resize(float width, float height);
    void event(const Event& event);
    bool needsRedraw() const;
    void draw(Painter& painter);
    bool opaque(const Vector2& pos) const;

    Vector2 getPos(Component* component);

    void setCursor(Component *owner, SDL_Cursor *cursor);
    void setSystemCursor(Component *owner, SDL_SystemCursor id);
    void tryClearCursor(Component *owner);
    SDL_Cursor *getSystemCursor(SDL_SystemCursor id);
    void freeSystemCursor(SDL_SystemCursor id);
    void freeAllSystemCursors();

protected:
    void setDirty(const Rect2D& rect);

private:

    typedef std::vector<Rect2D> DirtyRectangles;
    DirtyRectangles dirtyRectangles;

    SDL_Cursor *cursor;
    Component *cursorOwner;
    SDL_Cursor *systemCursors[SDL_NUM_SYSTEM_CURSORS] = {0};
};

#endif


/** @file gui/Desktop.hpp */
