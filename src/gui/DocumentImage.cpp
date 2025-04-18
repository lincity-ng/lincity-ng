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
 * @file DocumentImage.cpp
 */

#include "DocumentImage.hpp"

#include <stdio.h>             // for sscanf
#include <string.h>            // for strcmp
#include <filesystem>          // for path
#include <iostream>            // for basic_ostream, operator<<, stringstream
#include <sstream>             // for basic_stringstream
#include <stdexcept>           // for runtime_error

#include "Painter.hpp"         // for Painter
#include "Texture.hpp"         // for Texture
#include "TextureManager.hpp"  // for TextureManager, texture_manager
#include "Vector2.hpp"         // for Vector2
#include "XmlReader.hpp"       // for XmlReader

DocumentImage::DocumentImage()
	: texture(0)
{}

DocumentImage::~DocumentImage()
{}

void
DocumentImage::parse(XmlReader& reader, const Style& parentstyle)
{
    style = parentstyle;
    
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttribute(attribute, value)) {
            continue;
        } else if(style.parseAttribute(attribute, value)) {
            continue;
        } else if(strcmp(attribute, "width") == 0) {
            if(sscanf(value, "%f", &width) != 1) {
                std::stringstream msg;
                msg << "Couldn't parse width '" << value << "'.";
                throw std::runtime_error(msg.str());
            }
        } else if(strcmp(attribute, "height") == 0) {
            if(sscanf(value, "%f", &height) != 1) {
                std::stringstream msg;
                msg << "Couldn't parse height '" << value << "'.";
                throw std::runtime_error(msg.str());
            }
        } else if(strcmp(attribute, "src") == 0) {
            filename=value;
            texture = 0;
            texture = texture_manager->load(value);
        } else {
            std::cerr << "Skipping unknown attribute '"
                << attribute << "'.\n";
        }
    }

    width = texture->getWidth();
    height = texture->getHeight();
}

void
DocumentImage::resize(float , float )
{}

void
DocumentImage::draw(Painter& painter)
{
    painter.drawTexture(texture, Vector2(0, 0));
}


/** @file gui/DocumentImage.cpp */

