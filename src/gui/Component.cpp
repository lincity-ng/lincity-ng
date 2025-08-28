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
 * @file Component.cpp
 * @author Matthias Braun
 */

#include <assert.h>                       // for assert
#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml++/ustring.h>             // for ustring
#include <string.h>                       // for NULL
#include <stdexcept>                      // for runtime_error
#include <utility>                        // for move
#include <vector>                         // for vector

#include "Component.hpp"
#include "Event.hpp"                      // for Event
#include "Painter.hpp"                    // for Painter
#include "util/xmlutil.hpp"               // for xmlParse

Component::Component() :
  parent(0), desktop(NULL), flags(0)
{
}

Component::~Component()
{
}

bool
Component::parseAttribute(xmlpp::TextReader& reader) {
  xmlpp::ustring aname = reader.get_name();
  xmlpp::ustring value = reader.get_value();
  if(aname == "name")
    name = xmlParse<std::string>(value);
  else if(aname.substr(0,5) == "xmlns")
    ; // can be ignored for now
  else
    return false;
  return true;
}

void
Component::drawChild(Child& child, Painter& painter)
{
    assert(child.getComponent() != 0);

    if(child.useClipRect) {
        painter.pushClipRect(child.clipRect);
    }
    if(child.position != Vector2(0, 0)) {
        painter.pushTransform();
        painter.translate(child.position);
    }
    child.component->draw(painter);
    if(child.position != Vector2(0, 0)) {
        painter.popTransform();
    }
    if(child.useClipRect) {
        painter.popClipRect();
    }
}

void
Component::draw(Painter& painter)
{
    for(Childs::iterator i = childs.begin(); i != childs.end(); ++i) {
        Child& child = *i;
        if(child.enabled)
            drawChild(child, painter);
    }
}

bool
Component::eventChild(Child& child, const Event& event, bool visible)
{
    assert(child.getComponent() != 0);

    Event ev = event;
    if(event.type == Event::MOUSEMOTION
        || event.type == Event::MOUSEBUTTONDOWN
        || event.type == Event::MOUSEBUTTONUP
        || event.type == Event::MOUSEWHEEL) {
        ev.mousepos -= child.position;
        if(visible && child.component->opaque(ev.mousepos))
            ev.inside = true;
        else
            ev.inside = false;
    }

    child.component->event(ev);
    return ev.inside;
}

void
Component::event(const Event& event) {
    bool visible = event.inside;
    for(Childs::reverse_iterator i = childs.rbegin(); i != childs.rend(); ++i) {
        Child& child = *i;
        if(!child.enabled)
          continue;

        if(eventChild(child, event, visible))
            visible = false;
    }
}

void
Component::reLayout()
{
    if(getFlags() & FLAG_RESIZABLE) {
        resize(getWidth(), getHeight());
    }
}

Component*
Component::findComponent(const std::string& name)
{
    if(getName() == name)
        return this;

    for(Childs::const_iterator i = childs.begin(); i != childs.end(); ++i) {
        const Child& child = *i;
        if (child.getComponent()) {
            Component* component = child.component->findComponent(name);
            if(component)
                return component;
        }
    }

    return 0;
}

Child *
Component::getParentChild() const {
  Component *p = getParent();
  if(!p) return NULL;
  for(Child& pc : p->childs)
    if(pc.getComponent() == this)
      return &pc;
  assert(false); // we must be a child of our parent
  return NULL;
}

Vector2
Component::relative2Global(const Vector2& pos)
{
    if(!parent)
        return pos;

    Child& me = parent->findChild(this);
    return parent->relative2Global(me.getPos() + pos);
}

Child&
Component::addChild(std::unique_ptr<Component>&& component) {
  assert(!component->parent);
  Component *comp = component.get();
  childs.push_back(Child(std::move(component)));
  comp->parent = this;
  comp->desktop = this->desktop;
  comp->setDirty();
  return childs.back();
}

void
Component::resetChild(Child& child, std::unique_ptr<Component>&& component)
{
    assert(child.component != component.get());

    delete child.component;
    child.component = component.release();
    if(child.component != 0) {
        child.component->parent = this;
        child.component->desktop = this->desktop;
        child.component->setDirty();
        child.enabled = true;
    }
}

void
Component::resize(float , float )
{
}

void
Component::setDirty(const Rect2D& rect)
{
    if(parent)
        parent->setChildDirty(this, rect);
}

Child&
Component::findChild(Component* component)
{
    for(Childs::iterator i = childs.begin(); i != childs.end(); ++i) {
        Child& child = *i;
        if(child.getComponent() == component)
            return child;
    }
    throw std::runtime_error("Child not found");
}

void
Component::setChildDirty(Component* childComponent, const Rect2D& area)
{
    for(Childs::const_iterator i = childs.begin(); i != childs.end(); ++i) {
        const Child& child = *i;
        if(child.getComponent() != childComponent)
            continue;
        if(!child.enabled)
            return;
        Rect2D rect = area;
        rect.move(child.position);
        setDirty(rect);
        return;
    }

    assert(false);
}


/** @file gui/Component.cpp */
