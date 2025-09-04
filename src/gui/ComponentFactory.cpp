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
 * @file ComponentFactory.cpp
 */

#include "ComponentFactory.hpp"

#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml++/ustring.h>             // for ustring
#include <filesystem>                     // for path, operator/
#include <memory>                         // for allocator, unique_ptr
#include <fmt/format.h>
#include <fmt/std.h> // IWYU pragma: keep
#include <stdexcept>

#include "Component.hpp"                  // for Component
#include "ComponentLoader.hpp"            // for createComponent
#include "lincity-ng/Config.hpp"          // for getConfig, Config
#include "util/xmlutil.hpp"               // for missingXmlAttribute, unexpe...

ComponentFactories* component_factories = 0;

// import factory
/**
 * @class ImportFactory
 */
class ImportFactory : public Factory
{
public:
    ImportFactory()
    {
        if(component_factories == 0)
            component_factories = new ComponentFactories;

        component_factories->insert(std::make_pair("Import", this));
    }

    std::unique_ptr<Component> createComponent(xmlpp::TextReader& reader);
};
//static ImportFactory factory_Import;

std::unique_ptr<Component>
ImportFactory::createComponent(xmlpp::TextReader& reader) {
  std::filesystem::path importfile;
  while(reader.move_to_next_attribute()) {
    xmlpp::ustring name = reader.get_name();
    xmlpp::ustring value = reader.get_value();
    if(name == "src")
      importfile = getConfig()->appDataDir.get()
        / xmlParse<std::filesystem::path>(value);
    else
      unexpectedXmlAttribute(reader);
  }
  reader.move_to_element();
  if(importfile.empty())
    missingXmlAttribute(reader, "src");


  xmlpp::TextReader nreader(importfile);
  if(!nreader.read())
    throw std::runtime_error(fmt::format("file is empty: {}", importfile));
  while(nreader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
    if(!nreader.next())
      throw std::runtime_error(
        fmt::format("file doesn't contain XML data: {}", importfile));
  }

  std::unique_ptr<Component> component =
    ::createComponent(nreader.get_name(), nreader);

  while(nreader.next()) {
    if(nreader.get_node_type() != xmlpp::TextReader::NodeType::Element)
      continue;
    unexpectedXmlElement(nreader);
  }
  return component;
}

//---------------------------------------------------------------------------

/**
 * @note It seems ar or g++ strip out unused functions in static libraries. For
 * some reasons also global constructor seems to be considered unused if noone
 * uses the global object. So can't use our slick component factory registration
 * tricks :-/ And have to fill in the list manually here
**/

#include "Button.hpp"                     // for Button
#include "CheckButton.hpp"                // for CheckButton
#include "Desktop.hpp"                    // for Desktop
#include "Document.hpp"                   // for Document
#include "FilledRectangle.hpp"            // for FilledRectangle
#include "Gradient.hpp"                   // for Gradient
#include "Image.hpp"                      // for Image
#include "Panel.hpp"                      // for Panel
#include "Paragraph.hpp"                  // for Paragraph
#include "ScrollBar.hpp"                  // for ScrollBar
#include "ScrollView.hpp"                 // for ScrollView
#include "SwitchComponent.hpp"            // for SwitchComponent
#include "TableLayout.hpp"                // for TableLayout
#include "TooltipManager.hpp"             // for TooltipManager
#include "Window.hpp"                     // for Window
#include "WindowManager.hpp"              // for WindowManager

DECLARE_COMPONENT_FACTORY(Button)
DECLARE_COMPONENT_FACTORY(CheckButton);
DECLARE_COMPONENT_FACTORY(Desktop)
DECLARE_COMPONENT_FACTORY(Document)
DECLARE_COMPONENT_FACTORY(FilledRectangle)
DECLARE_COMPONENT_FACTORY(Gradient)
DECLARE_COMPONENT_FACTORY(Image)
DECLARE_COMPONENT_FACTORY(Panel)
DECLARE_COMPONENT_FACTORY(Paragraph)
DECLARE_COMPONENT_FACTORY(ScrollBar)
DECLARE_COMPONENT_FACTORY(ScrollView)
DECLARE_COMPONENT_FACTORY(SwitchComponent)
DECLARE_COMPONENT_FACTORY(TableLayout)
DECLARE_COMPONENT_FACTORY(TooltipManager)
DECLARE_COMPONENT_FACTORY(Window)
DECLARE_COMPONENT_FACTORY(WindowManager)

void initFactories()
{
#ifdef DEBUG
    static bool initialized = false;
    if(!initialized)
    {
#endif
        new INTERN_ButtonFactory();
        new INTERN_CheckButtonFactory();
        new INTERN_DesktopFactory();
        new INTERN_DocumentFactory();
        new INTERN_FilledRectangleFactory();
        new INTERN_GradientFactory();
        new INTERN_ImageFactory();
        new INTERN_PanelFactory();
        new INTERN_ParagraphFactory();
        new INTERN_ScrollBarFactory();
        new INTERN_ScrollViewFactory();
        new INTERN_SwitchComponentFactory();
        new INTERN_TableLayoutFactory();
        new INTERN_TooltipManagerFactory();
        new INTERN_WindowFactory();
        new INTERN_WindowManagerFactory();
        new ImportFactory();

#ifdef DEBUG
        initialized = true;
    }
#endif
}


/** @file gui/ComponentFactory.cpp */
