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
 * @file Desktop.cpp
 */

#include "Desktop.hpp"

#include <SDL.h>                          // for SDL_SystemCursor, SDL_GetDe...
#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml++/ustring.h>             // for ustring
#include <stddef.h>                       // for NULL
#include <iostream>                       // for basic_ostream, operator<<
#include <stdexcept>                      // for runtime_error
#include <string>                         // for char_traits, basic_string

#include "Child.hpp"                      // for Childs, Child
#include "ComponentLoader.hpp"            // for createComponent
#include "Style.hpp"                      // for parseStyleDef
#include "util/xmlutil.hpp"               // for unexpectedXmlAttribute

Desktop::Desktop()
{
    setFlags(FLAG_RESIZABLE);
    desktop = this;
    cursor = SDL_GetDefaultCursor();
    cursorOwner = NULL;
}

Desktop::~Desktop() {
    freeAllSystemCursors();
}

void
Desktop::parse(xmlpp::TextReader& reader) {
  while(reader.move_to_next_attribute()) {
    if(parseAttribute(reader));
    else
      unexpectedXmlAttribute(reader);
  }
  reader.move_to_element();

  if(!reader.is_empty_element() && reader.read())
  while(reader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
    if(reader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
      reader.next();
      continue;
    }
    xmlpp::ustring element = reader.get_name();
    if(element == "DefineStyle") {
      parseStyleDef(reader);
    } else {
      addChild(createComponent(element, reader));
    }
    reader.next();
  }
}

void
Desktop::event(const Event& event)
{
    Component::event(event);
}

bool
Desktop::needsRedraw() const
{
    return dirtyRectangles.size() > 0;
}

void
Desktop::draw(Painter& painter)
{
    if(dirtyRectangles.size() > 0) {
        Component::draw(painter);
        if(cursor != SDL_GetCursor())
            SDL_SetCursor(cursor);
    }
    dirtyRectangles.clear();
}

bool
Desktop::opaque(const Vector2& pos) const
{
    for(Childs::const_iterator i = childs.begin(); i != childs.end(); ++i) {
        const Child& child = *i;
        if(!child.getComponent() || !child.isEnabled())
            continue;

        if(child.getComponent()->opaque(pos - child.getPos())) {
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
Desktop::setCursor(Component *owner, SDL_Cursor *cursor) {
    if(cursor != this->cursor)
        setDirty(Rect2D());
    this->cursor = cursor;
    cursorOwner = owner;
}

void
Desktop::setSystemCursor(Component *owner, SDL_SystemCursor id) {
    setCursor(owner, getSystemCursor(id));
}

void
Desktop::tryClearCursor(Component *owner) {
    if(owner == cursorOwner) {
        setCursor(NULL, SDL_GetDefaultCursor());
    }
}

SDL_Cursor *
Desktop::getSystemCursor(SDL_SystemCursor id) {
    SDL_Cursor *&cursor = systemCursors[id];
    if(!cursor)
        cursor = SDL_CreateSystemCursor(id);
    return cursor;
}

void
Desktop::freeSystemCursor(SDL_SystemCursor id) {
    SDL_FreeCursor(systemCursors[id]);
    systemCursors[id] = NULL;
}

void
Desktop::freeAllSystemCursors() {
    for(int id = 0; id < SDL_NUM_SYSTEM_CURSORS; id++)
        freeSystemCursor((SDL_SystemCursor)id);
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

/** @file gui/Desktop.cpp */
