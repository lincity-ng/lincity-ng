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
 * @file SwitchComponent.cpp
 * @brief Implement component switch for the implementation of tabbed dialogs.
 * @author Matthias Braun.
 */

#include "SwitchComponent.hpp"

#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml++/ustring.h>             // for ustring
#include <iostream>                       // for basic_ostream, operator<<
#include <vector>                         // for vector

#include "Child.hpp"                      // for Childs, Child
#include "ComponentFactory.hpp"           // for IMPLEMENT_COMPONENT_FACTORY
#include "ComponentLoader.hpp"            // for createComponent
#include "Vector2.hpp"                    // for Vector2
#include "util/xmlutil.hpp"               // for unexpectedXmlAttribute

/**
 * Class constructor.
 */
SwitchComponent::SwitchComponent()
{
    setFlags(FLAG_RESIZABLE);
}

SwitchComponent::~SwitchComponent()
{
}

void
SwitchComponent::parse(xmlpp::TextReader& reader) {
  while(reader.move_to_next_attribute()) {
    if(parseAttribute(reader));
    else
      unexpectedXmlAttribute(reader);
  }
  reader.move_to_element();

  bool first = true;
  if(!reader.is_empty_element() && reader.read())
  while(reader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
    if(reader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
      reader.next();
      continue;
    }
    xmlpp::ustring element = reader.get_name();

    Child& child = addChild(createComponent(element, reader));
    child.enable(first);
    first = false;
    reader.next();
  }
}

/**
 * Function to resize all resizable components.
 *
 * @param width New width to resize to.
 * @param height New height to resize to.
 */
void
SwitchComponent::resize(float width, float height)
{
    if(width < 0) width = 0;
    if(height < 0) height = 0;
    for(Childs::iterator i = childs.begin(); i != childs.end(); ++i) {
        Child& child = *i;
        if(child.getComponent() == 0) {
#ifdef DEBUG
            std::cerr << "Child in SwitchComponent==0 ?!?\n";
#endif
            continue;
        }
        if(! (child.getComponent()->getFlags() & FLAG_RESIZABLE))
            continue;

        child.getComponent()->resize(width, height);
        // TODO: honor minimum size of children
    }
    this->width = width;
    this->height = height;
}

/**
 * Function to switch to a given component.
 *
 * @param name Name of the component to switch to.
 */
void
SwitchComponent::switchComponent(const std::string& name)
{
    bool found = false;
    for(Childs::iterator i = childs.begin(); i != childs.end(); ++i) {
        Child& child = *i;
        if(child.getComponent()->getName() == name) {
            child.enable(true);
            found = true;
        } else {
            child.enable(false);
        }
    }

    if(!found) {
#ifdef DEBUG
        std::cerr << "No component named '" << name << "' found "
            << "while switching components.\n";
#endif
        if(!childs.empty()) {
            childs[0].enable(true);
        }
    }
    setDirty();
}

/**
 * Function to get the current active component.
 *
 * @return Returns a pointer to the active component.
 */
Component*
SwitchComponent::getActiveComponent()
{
    for(Childs::iterator i = childs.begin(); i != childs.end(); ++i) {
        Child& child = *i;
        if(child.isEnabled())
            return child.getComponent();
    }

    return 0;
}

/**
 * Check if a given component, identified by its position, is opaque or not.
 *
 * @param pos Constant vector representing the component's position.
 * @return True if the component is opaque at this place.
 * @todo Remove code duplication with TableLayout::opaque (pos) and
 *       Panel::opaque(pos).
 */
bool
SwitchComponent::opaque(const Vector2& pos) const
{
    for(Childs::const_iterator i = childs.begin(); i != childs.end(); ++i) {
        const Child& child = *i;
        if(child.getComponent() == 0 || !child.isEnabled())
            continue;

        if(child.getComponent()->opaque(pos - child.getPos()))
            return true;
    }

    return false;
}

IMPLEMENT_COMPONENT_FACTORY(SwitchComponent)

/** @file gui/SwitchComponent.cpp */
