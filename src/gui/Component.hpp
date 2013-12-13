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
#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include <SDL.h>
#include <vector>
#include <string>
#include <assert.h>
#include "Rect2D.hpp"
#include "Child.hpp"

class Painter;
class Event;

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

    /** Causes the component to layout it's child components again */
    virtual void reLayout();

    /**
     * @return true if the component is opaque at this place.
     */
    virtual bool opaque(const Vector2& pos) const
    {
        if(pos.x >= 0 && pos.y >= 0 && pos.x <= width && pos.y <= height)
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

    /**
     * Maps a relative coordinate from this component to a global one.
     */
    Vector2 relative2Global(const Vector2& pos);

protected:
    Childs childs;

    Child& addChild(Component* component);
    void resetChild(Child& child, Component* component);
    void drawChild(Child& child, Painter& painter);
    bool eventChild(Child& child, const Event& event, bool visible = false);
    void setChildDirty(Component* child, const Rect2D& area);
    Child& findChild(Component* component);

    void setDirty()
    {
        setDirty(Rect2D(0, 0, width, height));
    }
    virtual void setDirty(const Rect2D& area);

    /**
     * Used to parse attributes (from an xml stream for example). Currently
     * parses only the name attribute.
     * @return True if the attribute has been used, false else.
     */
    virtual bool parseAttribute(const char* attribute, const char* value);

    void setFlags(int flags)
    {
        this->flags |= flags;
    }

    void clearFlags(int flags)
    {
        this->flags &= ~flags;
    }

    Component* parent;
    int flags;
    float width, height;
    std::string name;

    friend class ButtonPanel;
};

#endif

/** @file gui/Component.hpp */

