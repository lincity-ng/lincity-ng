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

#include <fmt/format.h>                   // for format
#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml++/ustring.h>             // for ustring
#include <exception>                      // for exception
#include <map>                            // for operator==
#include <memory>                         // for unique_ptr, allocator
#include <sstream>                        // for basic_stringstream, basic_o...
#include <stdexcept>                      // for runtime_error
#include <utility>                        // for pair
#include <fmt/std.h> // IWYU pragma: keep

#include "Component.hpp"                  // for Component
#include "ComponentFactory.hpp"           // for component_factories, Compon...
#include "Desktop.hpp"                    // for Desktop
#include "Style.hpp"                      // for parseStyleDef
#include "lincity-ng/Config.hpp"          // for getConfig, Config
#include "util/xmlutil.hpp"               // for unexpectedXmlElement

//void initFactories();

std::unique_ptr<Component>
createComponent(const std::string& type, xmlpp::TextReader& reader) {
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
    return i->second->createComponent(reader);
    try {
    } catch(std::exception& e) {
        std::stringstream msg;
        msg << "Error while parsing component '" << type << "': " << e.what();
        throw std::runtime_error(msg.str());
    }
}

std::unique_ptr<Component> loadGUIFile(const std::filesystem::path& filename) {
  std::filesystem::path fullpath = getConfig()->appDataDir.get() / filename;
  xmlpp::TextReader reader(fullpath);

  // seek to the first XML element
  if(!reader.read())
    throw std::runtime_error(fmt::format("file is empty: {}", fullpath));
  while(reader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
    if(!reader.next())
      throw std::runtime_error(
        fmt::format("file doesn't contain XML data: {}", fullpath));
  }

  std::string componentName = reader.get_name();
  if(componentName == "gui") {
    std::unique_ptr<Desktop> desktop(new Desktop());
    desktop->parse(reader);
    return desktop;
  }
  else {
    return createComponent(componentName, reader);
  }
}

std::unique_ptr<Component> parseEmbeddedComponent(xmlpp::TextReader& reader) {
  std::unique_ptr<Component> component;
  if(!reader.is_empty_element() && reader.read())
  while(reader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
    if(reader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
      reader.next();
      continue;
    }
    xmlpp::ustring name = reader.get_name();
    if(name == "DefineStyle") {
      parseStyleDef(reader);
    } else if(!component) {
      component = createComponent(name, reader);
    } else {
      unexpectedXmlElement(reader);
    }
    reader.next();
  }

  return component;
}


/** @file gui/ComponentLoader.cpp */
