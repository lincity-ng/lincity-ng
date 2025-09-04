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
 * @file TooltipManager.cpp
 */

#include "TooltipManager.hpp"

#include <libxml++/parsers/textreader.h>  // for TextReader
#include <iostream>                       // for basic_ostream, operator<<
#include <map>                            // for map, operator==
#include <memory>                         // for unique_ptr
#include <utility>                        // for move, pair

#include "ComponentFactory.hpp"           // for IMPLEMENT_COMPONENT_FACTORY
#include "Document.hpp"                   // for Document
#include "Event.hpp"                      // for Event
#include "Paragraph.hpp"                  // for Paragraph
#include "Style.hpp"                      // for Style, styleRegistry
#include "Vector2.hpp"                    // for Vector2
#include "util/xmlutil.hpp"               // for unexpectedXmlAttribute, une...

TooltipManager* tooltipManager = 0;

TooltipManager::TooltipManager()
{
    childs.assign(1, Child());
    if(tooltipManager == 0)
        tooltipManager = this;

    setFlags(FLAG_RESIZABLE);
}

TooltipManager::~TooltipManager()
{
    if(tooltipManager == this)
        tooltipManager = 0;
}

void
TooltipManager::parse(xmlpp::TextReader& reader) {
  while(reader.move_to_next_attribute()) {
    if(parseAttribute(reader));
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
    unexpectedXmlElement(reader);
    reader.next();
  }
}

void
TooltipManager::resize(float width, float height)
{
    if(width < 0) width = 0;
    if(height < 0) height = 0;
    this->width = width;
    this->height = height;
}

bool
TooltipManager::opaque(const Vector2& ) const
{
    return false;
}

void
TooltipManager::event(const Event& event)
{
    if(event.type == Event::MOUSEMOTION) {
        if(comp_tooltip().getComponent() != 0) {
            comp_tooltip().setComponent(0);
        }
    }
}

void
TooltipManager::showTooltip(const std::string& text, const Vector2& pos)
{
    std::unique_ptr<Document> d (new Document());
    std::unique_ptr<Paragraph> p (new Paragraph());

    std::map<std::string, Style>::iterator s = styleRegistry.find("tooltip");
    if(s == styleRegistry.end()) {
        std::cerr << "Warning: No tooltip style defined.\n";
        p->setText(text);
    } else {
        p->setText(text, s->second);
        d->style = s->second;
    }
    d->addParagraph(std::move(p));
    d->resize(250, -1);
    Vector2 dest = pos + Vector2(-100, 26);
    if(dest.x < 20)
        dest.x = 20;
    if(dest.y < 20)
        dest.y = 20;
    if(dest.x + d->getWidth() > getWidth() - 20)
        dest.x = getWidth() - 20 - d->getWidth();
    if(dest.y + d->getHeight() > getHeight() - 20)
        dest.y = pos.y - 20 - d->getHeight();
    /* Show minimap tooltip above. Hardcoded size corresponding to .xml  */
    if(dest.x > getWidth() - 310)
        dest.y = pos.y - 20 - d->getHeight();
    comp_tooltip().setComponent(std::move(d));
    comp_tooltip().setPos(dest);
		setDirty();
}

IMPLEMENT_COMPONENT_FACTORY(TooltipManager)

/** @file gui/TooltipManager.cpp */
