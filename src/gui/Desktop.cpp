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
 * @file Desktop.cpp
 */

#include <config.h>

#include "Desktop.hpp"

#include <iostream>
#include <stdexcept>

#include "XmlReader.hpp"
#include "ComponentFactory.hpp"
#include "ComponentLoader.hpp"
#include "Style.hpp"

Desktop::Desktop()
{
    setFlags(FLAG_RESIZABLE);
}

Desktop::~Desktop()
{
}

void
Desktop::parse(XmlReader& reader)
{
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttribute(attribute, value)) {
            continue;
        } else {
            std::cerr << "Skipping unknown attribute '" << attribute << "'.\n";
        }
    }

    int depth = reader.getDepth();
    while(reader.read() && reader.getDepth() > depth) {
        if(reader.getNodeType() == XML_READER_TYPE_ELEMENT) {
            std::string element = (const char*) reader.getName();

            if(element == "DefineStyle") {
                parseStyleDef(reader);
            } else {
                Component* component = createComponent(element, reader);
                addChild(component);
            }
        }
    }
}

void
Desktop::event(const Event& event)
{
    Component::event(event);

    // process pending remove events...
    for(std::vector<Component*>::iterator i = removeQueue.begin();
            i != removeQueue.end(); ++i) {
        internal_remove(*i);
    }
    removeQueue.clear();

    // process pending child adds...
    for(std::vector<Component*>::iterator i = addQueue.begin();
            i != addQueue.end(); ++i) {
        Child& child = addChild(*i);
        child.setPos(Vector2( 
                    (getWidth() - child.getComponent()->getWidth()) / 2,
                    (getHeight() - child.getComponent()->getHeight()) / 2));
    }
    addQueue.clear();
}

bool
Desktop::needsRedraw() const
{
    return dirtyRectangles.size() > 0;
}

void
Desktop::draw(Painter& painter)
{
    if(dirtyRectangles.size() > 0)
        Component::draw(painter);
    dirtyRectangles.clear();
}

bool
Desktop::opaque(const Vector2& pos) const
{
    for(Childs::const_iterator i = childs.begin(); i != childs.end(); ++i) {
        const Child& child = *i;
        if(child.getComponent() == 0)
            continue;
        
        if(child.getComponent()->opaque(pos + child.getPos())) {
            return true;
        }
    }

    return false;
}

void
Desktop::resize(float width, float height)
{
    for(Childs::iterator i = childs.begin(); i != childs.end(); ++i) {
        Component* component = i->getComponent();
        if(component->getFlags() & FLAG_RESIZABLE)
            component->resize(width, height);
#ifdef DEBUG
        if(! (component->getFlags() & FLAG_RESIZABLE) 
                && (component->getWidth() <= 0 || component->getHeight() <= 0))
            std::cerr << "Warning: component with name '" 
                << component->getName() 
                << "' has invalid width/height but is not resizable.\n";
#endif
    }
    this->width = width;
    this->height = height;
    Component::setDirty();
}

Vector2
Desktop::getPos(Component* component)
{
    // find child
    Child* child = 0;
    for(Childs::iterator i = childs.begin(); i != childs.end(); ++i) {
        if(i->getComponent() == component) {
            child = &(*i);
            break;
        }
    }
    if(child == 0)
        throw std::runtime_error(
                "Trying to getPos a component that is not a direct child");

    return child->getPos();
}

void
Desktop::move(Component* component, Vector2 newpos)
{
    if(component->getFlags() & FLAG_RESIZABLE)
        throw std::runtime_error("Can't move resizable components around");

    // find child
    Child* child = 0;
    for(Childs::iterator i = childs.begin(); i != childs.end(); ++i) {
        if(i->getComponent() == component) {
            child = &(*i);
            break;
        }
    }
    if(child == 0)
        throw std::runtime_error(
                "Trying to getPos a component that is not a direct child");
    
    // keep component in bounds...
    if(newpos.x + component->getWidth() > width)
        newpos.x = width - component->getWidth();
    if(newpos.y + component->getHeight() > height)
        newpos.y = height - component->getHeight();
    if(newpos.x < 0)
        newpos.x = 0;
    if(newpos.y < 0)
        newpos.y = 0;

    child->setPos(newpos);
    Component::setDirty();
}

void
Desktop::remove(Component* component)
{
    removeQueue.push_back(component);
}

void
Desktop::internal_remove(Component* component)
{
    // find child
    for(Childs::iterator i = childs.begin(); i != childs.end(); ++i) {
        if(i->getComponent() == component) {
            childs.erase(i);
            return;
        }
    }
    throw std::runtime_error(
            "Trying to remove a component that is not a direct child");
}

void
Desktop::setDirty(const Rect2D& rect)
{
    // check if rectangle overlaps with 1 of the existing rectangles
    for(DirtyRectangles::iterator i = dirtyRectangles.begin();
            i != dirtyRectangles.end(); ++i) {
        if(i->overlap(rect)) {
            i->join(rect);
            return;
        }
    }

    // add a new dirty rectangle if no overlap occured
    /*std::cout << "Adding new rectangle: " 
        << rect.p1.x << "," << rect.p1.y << ","
        << rect.p2.x << "," << rect.p2.y << "\n"; */
    dirtyRectangles.push_back(rect);

    Component::setDirty(rect);
}

void
Desktop::addChildComponent(Component* component)
{
    assert( component != 0 );
    assert( component->getParent() == 0 );
    addQueue.push_back(component);
}

/** @file gui/Desktop.cpp */

