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
 * @file Child.cpp
 */

#include <config.h>

#include "Child.hpp"
#include "Component.hpp"

Child::Child(Component* _component)
    : useClipRect(false), component(_component)
{ 
    enabled = component != 0;
}

Child::~Child()
{
}

bool
Child::inside(const Vector2& pos) const
{
    if(!enabled)
        return false;

    if(pos.x > position.x && pos.y > position.y
            && pos.x < position.x + component->getWidth()
            && pos.y < position.y + component->getHeight())
        return true;

    return false;
}

void
Child::setComponent(Component* component)
{
    delete this->component;
    this->component = component;
    enabled = component != 0;
}

//---------------------------------------------------------------------------

Childs::Childs()
{
}

Childs::~Childs()
{
    for(iterator i = begin(); i != end(); ++i)
        delete i->component;
}

Childs::iterator
Childs::erase(Childs::iterator i)
{
    delete i->component;
    return std::vector<Child>::erase(i);
}

void
Childs::clear()
{
    for(iterator i = begin(); i != end(); ++i)
        delete i->component;
    std::vector<Child>::clear();
}


/** @file gui/Child.cpp */

