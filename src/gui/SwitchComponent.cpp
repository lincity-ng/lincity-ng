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

#include <config.h>

#include "SwitchComponent.hpp"
#include "XmlReader.hpp"
#include "ComponentLoader.hpp"
#include "ComponentFactory.hpp"

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

/**
 * Function for XML parsing.
 *
 * @param reader XmlReader object that represents a XML file.
 */
void
SwitchComponent::parse(XmlReader& reader)
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
    bool first = true;
    while(reader.read() && reader.getDepth() > depth) {
        if(reader.getNodeType() == XML_READER_TYPE_ELEMENT) {
            std::string element = (const char*) reader.getName();

            Component* component = createComponent(element, reader);
            Child& child = addChild(component);
            if(first) {
                child.enable(true);
                first = false;
            } else {
                child.enable(false);
            }
        }
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

