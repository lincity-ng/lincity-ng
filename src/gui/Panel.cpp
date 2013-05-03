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

#include <config.h>

#include "Panel.hpp"

#include <sstream>
#include <stdexcept>
#include <iostream>

#include "XmlReader.hpp"
#include "TextureManager.hpp"
#include "ComponentFactory.hpp"
#include "ComponentLoader.hpp"
#include "Painter.hpp"

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
        } else {
            std::cerr << "Skipping unknown attribute '" << attribute << "'.\n";
        }
    }

    if(width <= 0 || height <= 0) {
        throw std::runtime_error("invalid width/height");
    }

    Component* component = parseEmbeddedComponent(reader);
    addChild(component);
    if(component->getFlags() & FLAG_RESIZABLE) {
        component->resize(width, height);
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

IMPLEMENT_COMPONENT_FACTORY(Panel)


/** @file gui/Panel.cpp */

