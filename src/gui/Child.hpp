/* ---------------------------------------------------------------------- *
 * src/gui/Chile.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2005      Matthias Braun <matze@braunis.de>
 * Copyright (C) 2026      David Bears <dbear4q@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
** ---------------------------------------------------------------------- */

#ifndef __CHILD_HPP__
#define __CHILD_HPP__

#include <memory>
#include <vector>       // for vector

#include "Rect2D.hpp"   // for Rect2D
#include "Vector2.hpp"  // for Vector2

class Component;

/**
 * @class Child
 */
class Child
{
public:
    Child(Component* parent, std::unique_ptr<Component>&& _component = nullptr);
    ~Child();

    Component* getComponent() const
    {
        return component;
    }
    void setComponent(std::unique_ptr<Component>&& component);

    void enable(bool enabled);

    bool isEnabled() const
    {
        return enabled;
    }

    void setPos(const Vector2& position)
    {
        this->position = position;
    }

    const Vector2& getPos() const
    {
        return position;
    }

    bool inside(const Vector2& pos) const;

    void setClipRect(const Rect2D& rect)
    {
        useClipRect = true;
        clipRect = rect;
    }

private:
    friend class Component;
    friend class Childs;

    Vector2 position;
    bool enabled;
    bool useClipRect;
    Rect2D clipRect;
    Component *component = nullptr;
    Component *parent;
};

/**
 * @class Childs
 * I create an own Child vector class here. So that components can get deleted
 * when they are removed from the vector.
 * (This can't be done in the Child destructor, because making Child the owner
 * of a component, makes big trouble when implementing copy constructors)
 */
class Childs : public std::vector<Child>
{
public:
    Childs();
    ~Childs();

    iterator erase(iterator i);
    void clear();
};

#endif

/** @file gui/Child.hpp */
