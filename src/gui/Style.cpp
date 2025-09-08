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
 * @file Style.cpp
 */

#include "Style.hpp"

#include <fmt/format.h>                   // for format
#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml++/ustring.h>             // for ustring
#include <stdexcept>                      // for runtime_error
#include <utility>                        // for pair, make_pair

#include "util/xmlutil.hpp"               // for xmlParse, unexpectedXmlAttr...

using namespace std::string_literals;

std::map<std::string, Style> styleRegistry;

Style::Style()
    : italic(false), bold(false), font_size(20),
    alignment(ALIGN_LEFT), margin_left(0), margin_right(0),
    margin_top(0), margin_bottom(0), width(-1), height(-1),
    min_width(-1), min_height(-1)
{
    font_family = "sans";
    background.a = 0;
}

Style::~Style()
{
}

bool
Style::parseAttribute(xmlpp::TextReader& reader) {
  xmlpp::ustring name = reader.get_name();
  xmlpp::ustring value = reader.get_value();
  if(name == "style") {
    std::map<std::string, Style>::iterator i = styleRegistry.find(value);
    if(i == styleRegistry.end())
      throw std::runtime_error(fmt::format("no style {:?}", value));
    *this = i->second;
  }
  else if(name == "font-size")
    font_size = xmlParse<float>(value);
  else if(name == "font-family")
    font_family = xmlParse<std::string>(value);
  else if(name == "font-style") {
    if(value == "normal")
      italic = false;
    else if(value == "italic")
      italic = true;
    else
      throw std::runtime_error(fmt::format(
        "invalid font style {:?}. Expected 'normal' or 'italic'.", value));
  }
  else if(name == "font-weight") {
    if(value == "normal")
      bold = false;
    else if(value == "bold")
      bold = true;
    else
      throw std::runtime_error(fmt::format(
        "invalid font weight {:?}. Expected 'normal' or 'bold'.", value));
  }
  else if(name == "halign") {
    if(value == "left")
      alignment = Style::ALIGN_LEFT;
    else if(value == "center")
      alignment = Style::ALIGN_CENTER;
    else if(value == "right")
      alignment = Style::ALIGN_RIGHT;
    else
      throw std::runtime_error(fmt::format(
        "invalid horizontal alignment {:?}"s +
        ". Expected 'left', 'center', or 'right'.",
        value
      ));
  }
  else if(name == "width")
    width = xmlParse<float>(value);
  else if(name == "height")
    height = xmlParse<float>(value);
  else if(name == "min-width")
    min_width = xmlParse<float>(value);
  else if(name == "min-height")
    min_height = xmlParse<float>(value);
  else if(name == "margin-left")
    margin_left = xmlParse<float>(value);
  else if(name == "margin-right")
    margin_right = xmlParse<float>(value);
  else if(name == "margin-top")
    margin_top = xmlParse<float>(value);
  else if(name == "margin-bottom")
    margin_bottom = xmlParse<float>(value);
  else if(name == "color")
    text_color.parse(value);
  else if(name == "background")
    background.parse(value);
  else if(name == "href")
    href = xmlParse<std::string>(value);
  else if(name == "xmlns")
    ; // ignore xmlns attributes
  else
    return false;

  return true;
}

void
Style::parseAttributes(xmlpp::TextReader& reader) {
  while(reader.move_to_next_attribute()) {
    if(parseAttribute(reader));
    else
      unexpectedXmlAttribute(reader);
  }
  reader.move_to_element();
}

void parseStyleDef(xmlpp::TextReader& reader) {
  Style style;
  std::string name;

  while(reader.move_to_next_attribute()) {
    xmlpp::ustring aname = reader.get_name();
    xmlpp::ustring value = reader.get_value();
    if(style.parseAttribute(reader));
    else if(aname == "name")
      name = xmlParse<std::string>(value);
    else
      unexpectedXmlAttribute(reader);
  }
  reader.move_to_element();

  if(name.empty())
    missingXmlAttribute(reader, "name");

  styleRegistry.insert(std::make_pair(name, style));
}

void
Style::toSpan(void)
{
    margin_right = 0;
    margin_left = 0;
    margin_bottom = 0;
    margin_top = 0;
    if (alignment == ALIGN_CENTER)
    {   alignment = ALIGN_LEFT;}
}


/** @file gui/Style.cpp */
