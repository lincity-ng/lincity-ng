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
 * @file Desktop.hpp
 */

#ifndef __DESKTOP_HPP__
#define __DESKTOP_HPP__

#include <vector>
#include "Component.hpp"

class XmlReader;

/**
 * @class Desktop
 */
class Desktop : public Component
{
public:
    Desktop();
    virtual ~Desktop();

    void parse(XmlReader& reader);

    void resize(float width, float height);
    void event(const Event& event);
    bool needsRedraw() const;
    void draw(Painter& painter);
    bool opaque(const Vector2& pos) const;

    void addChildComponent(Component* child);
    void centerChildComponent(Component* child);

    Vector2 getPos(Component* component);
    void move(Component* component, Vector2 pos);
    void remove(Component* component);

protected:
    void setDirty(const Rect2D& rect);
    
private:
    void internal_remove(Component* component);
    
    std::vector<Component*> removeQueue;
    std::vector<Component*> addQueue;
    typedef std::vector<Rect2D> DirtyRectangles;
    DirtyRectangles dirtyRectangles;
};

#endif


/** @file gui/Desktop.hpp */

