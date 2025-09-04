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
 * @file FilledRectangle.cpp
 */

#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml++/ustring.h>             // for ustring
#include <string>                         // for basic_string, allocator

#include "ComponentFactory.hpp"           // for IMPLEMENT_COMPONENT_FACTORY
#include "FilledRectangle.hpp"
#include "Painter.hpp"                    // for Painter
#include "Rect2D.hpp"                     // for Rect2D
#include "util/xmlutil.hpp"               // for unexpectedXmlAttribute

FilledRectangle::FilledRectangle()
{}

FilledRectangle::~FilledRectangle()
{}

void
FilledRectangle::parse(xmlpp::TextReader& reader) {
  while(reader.move_to_next_attribute()) {
    xmlpp::ustring name = reader.get_name();
    xmlpp::ustring value = reader.get_value();
    if(parseAttribute(reader));
    else if(name == "color")
      color.parse(value);
    else
      unexpectedXmlAttribute(reader);
  }
  reader.move_to_element();

  flags |= FLAG_RESIZABLE;
}

void
FilledRectangle::resize(float width, float height)
{
    if(width < 0) width = 0;
    if(height < 0) height = 0;
    this->width = width;
    this->height = height;
}

void
FilledRectangle::draw(Painter& painter)
{
    painter.setFillColor(color);
    painter.fillRectangle(Rect2D(0, 0, width, height));
}

IMPLEMENT_COMPONENT_FACTORY(FilledRectangle)

/** @file gui/FilledRectangle.cpp */
