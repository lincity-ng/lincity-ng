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

#include <config.h>

#include "Style.hpp"
#include "XmlReader.hpp"

#include <iostream>
#include <sstream>
#include <stdexcept>

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

void
Style::parseAttributes(XmlReader& reader)
{
    XmlReader::AttributeIterator iter(reader);

    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(!parseAttribute(attribute, value)) {
            std::cerr << "Skipping unknown style attribute '"
                << attribute << "'.\n";
        }
    }
}

bool
Style::parseAttribute(const char* attribute, const char* value)
{
    if(strcmp(attribute, "style") == 0) {
        std::map<std::string, Style>::iterator i = styleRegistry.find(value);
        if(i == styleRegistry.end()) {
            std::stringstream msg;
            msg << "No style with name '" << value << "' defined.";
            throw std::runtime_error(msg.str());
        }
        *this = i->second;
    } else if(strcmp(attribute, "font-size") == 0) {
        if(sscanf(value, "%f", &font_size) != 1) {
            std::cerr << "Warning problem parsing size '" <<
                value << "'\n";
        }
    } else if(strcmp(attribute, "font-family") == 0) {
        font_family = value;
    } else if(strcmp(attribute, "font-style") == 0) {
        if(strcmp(value, "normal") == 0) {
            italic = false;
        } else if(strcmp(value, "italic") == 0) {
            italic = true;
        } else {
            std::cerr << "Invalid value for font-style "
                << "(only 'normal' and 'italic' allowed)\n";
        }
    } else if(strcmp(attribute, "font-weight") == 0) {
        if(strcmp(value, "normal") == 0) {
            bold = false;
        } else if(strcmp(value, "bold") == 0) {
            bold = true;
        } else {
            std::cerr << "Invalid value for font-weight "
                << "(only 'normal' and 'bold' allowed)\n";
        }
    } else if(strcmp(attribute, "halign") == 0) {
        if(strcmp(value, "left") == 0) {
            alignment = Style::ALIGN_LEFT;
        } else if(strcmp(value, "center") == 0) {
            alignment = Style::ALIGN_CENTER;
        } else if(strcmp(value, "right") == 0) {
            alignment = Style::ALIGN_RIGHT;
        } else {
            std::cerr << "Invalid value for halign attribute "
                << "(only 'left', 'center' and 'right' allowed)\n";
        }
    } else if(strcmp(attribute, "width") == 0) {
        if(sscanf(value, "%f", &width) != 1) {
            std::cerr << "Couldn't parse value for width: '"
                << value << "'\n";
        }
    } else if(strcmp(attribute, "height") == 0) {
        if(sscanf(value, "%f", &height) != 1) {
            std::cerr << "Couldn't parse value for height: '"
                << value << "'\n";
        }
    } else if(strcmp(attribute, "min-width") == 0) {
        if(sscanf(value, "%f", &min_width) != 1) {
            std::cerr << "Couldn't parse value for min-width: '"
                << value << "'\n";
        }
    } else if(strcmp(attribute, "min-height") == 0) {
        if(sscanf(value, "%f", &min_height) != 1) {
            std::cerr << "Couldn't parse value for min-height: '"
                << value << "'\n";
        }
    } else if(strcmp(attribute, "margin-left") == 0) {
        if(sscanf(value, "%f", &margin_left) != 1) {
            std::cerr << "Couldn't parse value for margin-left: '"
                << value << "'\n";
        }
    } else if(strcmp(attribute, "margin-right") == 0) {
        if(sscanf(value, "%f", &margin_right) != 1) {
            std::cerr << "Couldn't parse value for margin-right: '"
                << value << "'\n";
        }
    } else if(strcmp(attribute, "margin-top") == 0) {
        if(sscanf(value, "%f", &margin_top) != 1) {
            std::cerr << "Couldn't parse value for margin-top: '"
                << value << "'\n";
        }
    } else if(strcmp(attribute, "margin-bottom") == 0) {
        if(sscanf(value, "%f", &margin_bottom) != 1) {
            std::cerr << "Couldn't parse value for argin-bottom: '"
                << value << "'\n";
        }
    } else if(strcmp(attribute, "color") == 0) {
        text_color.parse(value);
    } else if(strcmp(attribute, "background") == 0) {
        background.parse(value);
    } else if(strcmp(attribute, "href") == 0) {
        href = value;
    } else if(strncmp(attribute, "xmlns", 5) == 0) {
        // simply ignore xmlns attributes
        return true;
    } else {
        return false;
    }

    return true;
}

void parseStyleDef(XmlReader& reader)
{
    Style style;
    std::string name;
    XmlReader::AttributeIterator iter(reader);

    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(style.parseAttribute(attribute, value)) {
            continue;
        } else if(strcmp(attribute, "name") == 0) {
            name = value;
        } else {
            std::cerr << "Unknown attribute '" << attribute
                << "' in style definition.\n";
        }
    }

    reader.nextNode();

    if(name == "")
        throw std::runtime_error("Missing name in style definition");
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

