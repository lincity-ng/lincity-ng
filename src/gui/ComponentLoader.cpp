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
 * @file ComponentLoader.cpp
 */

#include "ComponentLoader.hpp"

#include <libxml/xmlreader.h>    // for XML_READER_TYPE_ELEMENT
#include <string.h>              // for strcmp
#include <exception>             // for exception
#include <iostream>              // for basic_ostream, operator<<, stringstream
#include <map>                   // for operator==, _Rb_tree_iterator
#include <memory>                // for unique_ptr
#include <sstream>               // for basic_stringstream
#include <stdexcept>             // for runtime_error
#include <utility>               // for pair

#include "Component.hpp"         // for Component
#include "ComponentFactory.hpp"  // for component_factories, ComponentFactories
#include "Desktop.hpp"           // for Desktop
#include "Style.hpp"             // for parseStyleDef
#include "XmlReader.hpp"         // for XmlReader
#include "lincity-ng/Config.hpp"

//void initFactories();

Component* createComponent(const std::string& type, XmlReader& reader)
{
    //static int depth = 0;
    //initFactories();
    //Component * component = 0;
    if(component_factories == 0)
        throw std::runtime_error("No component factories registered");

    ComponentFactories::iterator i = component_factories->find(type);
    if(i == component_factories->end()) {
        std::stringstream msg;
        msg << "Couldn't find a component factory for '" << type << "'";
        throw std::runtime_error(msg.str());
    }
/*
    for(int i=0;i<depth;++i)
        std::cout << "\t";
    std::cout << type << ": begin" <<std::endl;
    std::cout.flush();
    ++depth;
    component = i->second->createComponent(reader);
    --depth;
    for(int i=0;i<depth;++i)
        std::cout << "\t";
    std::cout << type << ": end" << std::endl;
*/
    try {
        return i->second->createComponent(reader);
    } catch(std::exception& e) {
        std::stringstream msg;
        msg << "Error while parsing component '" << type << "': " << e.what();
        throw std::runtime_error(msg.str());
    } catch(...) {
        throw;
    }
}

Component* loadGUIFile(const std::filesystem::path& filename)
{
    XmlReader reader(getConfig()->appDataDir / filename);

    std::string componentName = (const char*) reader.getName();
    if(componentName == "gui") {
        std::unique_ptr<Desktop> desktop (new Desktop());
        desktop->parse(reader);
        return desktop.release();
    }

    std::unique_ptr<Component> component (createComponent(componentName, reader));
    return component.release();
}

Component* parseEmbeddedComponent(XmlReader& reader)
{
    Component* component = 0;
    try {
        int depth = reader.getDepth();
        while(reader.read() && reader.getDepth() > depth) {
            if(reader.getNodeType() == XML_READER_TYPE_ELEMENT) {
                const char* name = (const char*) reader.getName();
                if(strcmp(name, "DefineStyle") == 0) {
                    parseStyleDef(reader);
                } else if(component == 0) {
                    component = createComponent(name, reader);
                } else {
                    std::cerr << "Multiple components specified."
                        << " Skipping '" << name << "'.\n";
                    continue;
                }
            }
        }
    } catch(...) {
        delete component;
        throw;
    }

    return component;
}


/** @file gui/ComponentLoader.cpp */
