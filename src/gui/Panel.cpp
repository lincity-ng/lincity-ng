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

#include <cassert>                        // for assert
#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml++/ustring.h>             // for ustring
#include <filesystem>                     // for path
#include <stdexcept>                      // for runtime_error
#include <string>                         // for basic_string, operator==
#include <vector>                         // for vector

#include "Child.hpp"                      // for Childs, Child
#include "ComponentFactory.hpp"           // for IMPLEMENT_COMPONENT_FACTORY
#include "ComponentLoader.hpp"            // for createComponent
#include "Painter.hpp"                    // for Painter
#include "Rect2D.hpp"                     // for Rect2D
#include "Style.hpp"                      // for parseStyleDef
#include "Texture.hpp"                    // for Texture
#include "TextureManager.hpp"             // for TextureManager, texture_man...
#include "Vector2.hpp"                    // for Vector2
#include "util/xmlutil.hpp"               // for xmlParse, unexpectedXmlAttr...

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

void
Panel::parse(xmlpp::TextReader& reader) {
  while(reader.move_to_next_attribute()) {
    xmlpp::ustring name = reader.get_name();
    xmlpp::ustring value = reader.get_value();
    if(parseAttribute(reader));
    else if(name == "background") {
      std::filesystem::path bgFile = xmlParse<std::filesystem::path>(value);
      background = texture_manager->load(bgFile);
      background->setScaleMode(Texture::ScaleMode::ANISOTROPIC);
    }
    else if(name == "width")
      width = xmlParse<float>(value);
    else if(name == "height")
      height = xmlParse<float>(value);
    else if(name == "resizable")
      (this->*(xmlParse<bool>(value) ? &Panel::setFlags : &Panel::clearFlags))
        (FLAG_RESIZABLE);
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
        painter.drawStretchTexture(background, Rect2D(0, 0, width, height));

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
