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
#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include <memory>       // for unique_ptr
#include <string>       // for string, basic_string

#include "Child.hpp"    // for Child (ptr only), Childs
#include "Rect2D.hpp"   // for Rect2D
#include "Vector2.hpp"  // for Vector2

class Desktop;
class Event;
class Painter;
namespace xmlpp {
class TextReader;
}  // namespace xmlpp

/**
 * @author Matthias Braun.
 * @file Component.hpp
 * @class Component
 * @brief Implementation of blocks which are a rectangular area on screen.
 *
 * The Component is the basic building block of the GUI. It represents a
 * rectangular area on screen which receives events and is able to draw itself.
 * This class has to be used as a base class to implement specific
 * gui-components like buttons or windows.
 *
 * Currently there are 2 sorts of components: resizable ones and fixed-size
 * ones. A fixed size component should have it's size set after creation (you
 * should set the width and height fields in the constructor).
 * Resizable components should set the resize flag in the constructor and
 * override the resize function. If the resize function is called the component
 * should decide on the size it needs (it might only need a fraction of the size
 * passed in in the resize call), and then set the width and height members
 * accordingly.
 *
 * Components are typically put in a tree (for example you can put
 * child-components in a TableLayout or in a Window). However components
 * typically don't have to take care of this. The corrdinates in the mouse
 * events are transformed into corrdinates relative to the component origin.
 * Also the painter passed to the draw function will have a transformation
 * set, so that you can simply use coordinates ralative to the component origin.
 */
class Component
{
public:
    /** Values for the flags bitfield. */
    enum {
        FLAG_RESIZABLE = 0x00000001
    };

    Component();
    virtual ~Component();

    virtual void draw(Painter& painter);
    virtual void event(const Event& event);
    virtual void resize(float width, float height);
    void resize(Vector2 newSize) {resize(newSize.x, newSize.y);}

    /** Causes the component to layout it's child components again */
    virtual void reLayout();

    /**
     * @return true if the component is opaque at this place.
     */
    virtual bool opaque(const Vector2& pos) const
    {
        if(pos.x >= 0 && pos.y >= 0 && pos.x < width && pos.y < height)
            return true;

        return false;
    }

    float getWidth() const
    {
        return width;
    }

    float getHeight() const
    {
        return height;
    }

    Vector2 getSize() const {
        return size;
    }

    const std::string& getName() const
    {
        return name;
    }

    void setName(const std::string& name)
    {
        this->name = name;
    }

    /** @return The component flags (this is a bitfield). */
    int getFlags() const
    {
        return flags;
    }

    /**
     * Get the current component parent.
     * @return The parent component or 0 if the component has no parent. */
    Component* getParent() const
    {
        return parent;
    }
    Component* findComponent(const std::string& name);
    Desktop* getDesktop() const {
        return desktop;
    }
    Child *getParentChild() const;

    /**
     * Maps a relative coordinate from this component to a global one.
     */
    Vector2 relative2Global(const Vector2& pos);

protected:
    Childs childs;

    Child& addChild(std::unique_ptr<Component>&& component);
    void resetChild(Child& child, std::unique_ptr<Component>&& component);
    void drawChild(Child& child, Painter& painter);
    bool eventChild(Child& child, const Event& event, bool visible = false);
    void setChildDirty(Component* child, const Rect2D& area);
    Child& findChild(Component* component);

    void setDirty()
    {
        setDirty(Rect2D(0, 0, width, height));
    }
    virtual void setDirty(const Rect2D& area);

    virtual bool parseAttribute(xmlpp::TextReader& reader);

    void setFlags(int flags)
    {
        this->flags |= flags;
    }

    void clearFlags(int flags)
    {
        this->flags &= ~flags;
    }

    Component* parent;
    Desktop *desktop;
    int flags;
    Vector2 size;
    float &width = size.x;
    float &height = size.y;
    std::string name;
};

#endif

/** @file gui/Component.hpp */
