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
 * @file Panel.cpp
 */

#include "Panel.hpp"

#include <assert.h>              // for assert
#include <libxml/xmlreader.h>    // for XML_READER_TYPE_ELEMENT
#include <stdio.h>               // for sscanf
#include <string.h>              // for strcmp
#include <filesystem>            // for path
#include <iostream>              // for basic_ostream, operator<<, stringstream
#include <sstream>               // for basic_stringstream
#include <stdexcept>             // for runtime_error
#include <string>                // for char_traits, allocator, basic_string
#include <vector>                // for vector

#include "Child.hpp"             // for Childs, Child
#include "ComponentFactory.hpp"  // for IMPLEMENT_COMPONENT_FACTORY
#include "ComponentLoader.hpp"   // for createComponent
#include "Painter.hpp"           // for Painter
#include "Style.hpp"             // for parseStyleDef
#include "TextureManager.hpp"    // for TextureManager, texture_manager
#include "Vector2.hpp"           // for Vector2
#include "XmlReader.hpp"         // for XmlReader

/**
 * Class constructor.
 */
Panel::Panel()
  : background(0)
{
}

/**
 * Class destructor.
 */
Panel::~Panel()
{
}

/**
 * Function for XML parsing.
 *
 * @param reader XmlReader object that represents a XML file.
 */
void
Panel::parse(XmlReader& reader)
{
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttribute(attribute, value)) {
            continue;
        } else if(strcmp(attribute, "background") == 0) {
            background = 0;
            background = texture_manager->load(value);
        } else if(strcmp(attribute, "width") == 0) {
            if(sscanf(value, "%f", &width) != 1) {
                std::stringstream msg;
                msg << "Parse error when parsing width (" << value << ")";
                throw std::runtime_error(msg.str());
           }
        } else if(strcmp(attribute, "height") == 0) {
            if(sscanf(value, "%f", &height) != 1) {
                std::stringstream msg;
                msg << "Parse error when parsing height (" << value << ")";
                throw std::runtime_error(msg.str());
            }
        } else if(!strcmp(attribute, "resizable")) {
          if(!strcmp(value, "yes") || !strcmp(value, "true")) {
            setFlags(FLAG_RESIZABLE);
          }
          else if(!strcmp(value, "no") || !strcmp(value, "false")) {
            clearFlags(FLAG_RESIZABLE);
          }
          else {
            throw std::runtime_error(std::string() +
              "invalid value for attribute resizable: '" + value + "'");
          }
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

    if(width > 0 && height > 0) {
      resize(width, height);
    }
    else if(!(getFlags() & FLAG_RESIZABLE)) {
      throw std::runtime_error("invalid width/height");
    }
}

/**
 * @param Painter Painter object that represent the widget that needs to be
 *                drawn.
 */
void
Panel::draw(Painter& painter)
{
    if(background)
        painter.drawTexture(background, Vector2(0, 0));

    Component::draw(painter);
}

/**
 * Check if a given component, identified by its position, is opaque or not.
 *
 * @param pos Constant vector representing the component's position.
 * @return True if the component is opaque at this place.
 * @todo Remove code duplication with SwitchComponent::opaque (pos) and
 *       TableLayout::opaque(pos).
 */
bool
Panel::opaque(const Vector2& pos) const
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

void
Panel::resize(float width, float height) {
  for(Childs::iterator i = childs.begin(); i != childs.end(); ++i) {
    Component* component = i->getComponent();
    if(component->getFlags() & FLAG_RESIZABLE) {
      component->resize(width, height);
    }
    else {
      assert(false);
    }
  }
  this->width = width;
  this->height = height;
  Component::setDirty();
}

IMPLEMENT_COMPONENT_FACTORY(Panel)


/** @file gui/Panel.cpp */
