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
 * @file Document.cpp
 */

#include "Document.hpp"

#include <assert.h>                       // for assert
#include <fmt/base.h>                     // for println
#include <fmt/format.h>                   // for format
#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml++/ustring.h>             // for ustring
#include <stdio.h>                        // for stderr
#include <filesystem>                     // for path, operator/
#include <functional>                     // for bind, _1, _2, function
#include <iostream>                       // for basic_ostream, operator<<
#include <memory>                         // for unique_ptr, allocator
#include <stdexcept>                      // for runtime_error
#include <utility>                        // for move
#include <vector>                         // for vector
#include <fmt/std.h> // IWYU pragma: keep

#include "Child.hpp"                      // for Childs, Child
#include "Color.hpp"                      // for Color
#include "ComponentFactory.hpp"           // for IMPLEMENT_COMPONENT_FACTORY
#include "DocumentElement.hpp"            // for DocumentElement
#include "DocumentImage.hpp"              // for DocumentImage
#include "Painter.hpp"                    // for Painter
#include "Paragraph.hpp"                  // for Paragraph
#include "Rect2D.hpp"                     // for Rect2D
#include "Vector2.hpp"                    // for Vector2
#include "lincity-ng/Config.hpp"          // for getConfig, Config
#include "util/xmlutil.hpp"               // for unexpectedXmlElement, unexp...

using namespace std::placeholders;

Document::Document()
{
    setFlags(FLAG_RESIZABLE);
}

Document::~Document()
{
}

void
Document::parse(xmlpp::TextReader& reader) {
  std::filesystem::path srcPath;

  while(reader.move_to_next_attribute()) {
    xmlpp::ustring name = reader.get_name();
    xmlpp::ustring value = reader.get_value();
    if(parseAttribute(reader));
    else if(style.parseAttribute(reader));
    else if(name == "src")
      srcPath = getConfig()->appDataDir.get() /
        xmlParse<std::filesystem::path>(value);
    else
      unexpectedXmlAttribute(reader);
  }
  reader.move_to_element();

  if(!srcPath.empty()) {
    xmlpp::TextReader reader(srcPath);

    // seek to the first XML element
    if(!reader.read())
      throw std::runtime_error(fmt::format("file is empty: {}", srcPath));
    while(reader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
      if(!reader.next())
        throw std::runtime_error(
          fmt::format("file doesn't contain XML data: {}", srcPath));
    }

    parse(reader);

    while(reader.next()) {
      if(reader.get_node_type() != xmlpp::TextReader::NodeType::Element)
        continue;
      unexpectedXmlElement(reader);
    }
    return;
  }

  if(!reader.is_empty_element() && reader.read())
  while(reader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
    if(reader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
      if(reader.get_node_type() == xmlpp::TextReader::NodeType::Text) {
        fmt::println(stderr, "warning: text outside paragraph not allowed");
        assert(false);
      }
      reader.next();
      continue;
    }
    xmlpp::ustring node = reader.get_name();
    if(node == "p" || node=="Paragraph" || node == "li") {
      std::unique_ptr<Paragraph> paragraph(new Paragraph());
      if(node != "li") {
        paragraph->parse(reader, style);
      } else {
        paragraph->parseList(reader, style);
      }
      paragraph->linkClicked.connect(
        std::bind(&Document::paragraphLinkClicked, this, _1, _2));
      addChild(std::move(paragraph));
    } else if(node == "img") {
      std::unique_ptr<DocumentImage> image(new DocumentImage());
      image->parse(reader, style);
      addChild(std::move(image));
    } else {
      unexpectedXmlElement(reader);
    }
    reader.next();
  }
}

void
Document::resize(float newwidth, float newheight)
{
    height = 0;
    for(Childs::iterator i = childs.begin(); i != childs.end(); ++i) {
        Child& child = *i;
        Component* component = child.getComponent();
        DocumentElement* element = dynamic_cast<DocumentElement*> (component);
        if(!element) {
            std::cerr << "Component not a DocumentElement in Document::resize!\n";
            continue;
        }

        float compwidth = newwidth - element->getStyle().margin_left - element->getStyle().margin_right;
        if(compwidth < 0)
            compwidth = 0;

        if(dynamic_cast<DocumentImage *>(component)) {
          // we don't want to up-scale images
          component->resize(-1, -1);
          if(component->getWidth() > compwidth)
            component->resize(compwidth, -1);
        }
        else {
          component->resize(compwidth, -1);
        }

        float posx = element->getStyle().margin_left;
        switch(element->getStyle().alignment)
        {
            case Style::ALIGN_LEFT:
                posx += 0;
                break;
            case Style::ALIGN_RIGHT:
                posx += compwidth - component->getWidth();
                break;
            case Style::ALIGN_CENTER:
                posx += (compwidth - component->getWidth()) / 2;
                break;
            default:
                assert(false);
                break;
        }
        height += element->getStyle().margin_top;
        child.setPos(Vector2(posx, height));
        height += component->getHeight() + element->getStyle().margin_bottom;
    }
    width = newwidth;
    if(width < 0) width = 0;

    if(height < newheight)
        height = newheight;
}

void
Document::addParagraph(std::unique_ptr<Paragraph> paragraph)
{
    paragraph->linkClicked.connect(
      std::bind(&Document::paragraphLinkClicked, this, _1, _2));
    addChild(std::move(paragraph));
    resize(width, height);
}

void
Document::draw(Painter& painter)
{
    if(style.background.a != 0) {
        painter.setFillColor(style.background);
        painter.fillRectangle(Rect2D(0, 0, width, height));
    }

    Component::draw(painter);
}

void
Document::paragraphLinkClicked(Paragraph* paragraph, const std::string& href)
{
    linkClicked(paragraph, href);
}

IMPLEMENT_COMPONENT_FACTORY(Document)

/** @file gui/Document.cpp */
