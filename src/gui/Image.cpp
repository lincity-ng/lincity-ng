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
 * @file Image.cpp
 */

#include <config.h>

#include "Image.hpp"
#include "XmlReader.hpp"
#include "TextureManager.hpp"
#include "ComponentFactory.hpp"
#include "Painter.hpp"

Image::Image()
    : texture(0)
{
}

Image::~Image()
{
}

void
Image::parse(XmlReader& reader)
{
    bool resizable = false;

    bool grey = false;

    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttribute(attribute, value)) {
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
        } else if(strcmp(attribute, "filter") == 0) {
            if(strcmp(value, "grey") == 0) {
                grey = true;
            } else if(strcmp(value, "no") == 0) {
                grey = false;
            } else {
                std::cerr << "Unknown filter value '" << value << "'.\n";
                std::cerr << "Should be 'grey' or 'no'.\n";
            }
        } else if(strcmp(attribute, "resizable") == 0) {
            if(strcmp(value, "yes") == 0)
                resizable = true;
            else if(strcmp(value, "no") == 0)
                resizable = false;
            else
                std::cerr
                    << "You should specify 'yes' or 'no' for the resizable"
                    << "attribute\n";
        } else {
            std::cerr << "Skipping unknown attribute '"
                << attribute << "'.\n";
        }
    }

    if(filename == "")
        throw std::runtime_error("No filename specified for image");

    texture = 0;
    texture = texture_manager->load(filename,
				grey ? TextureManager::FILTER_GREY : TextureManager::NO_FILTER);

    if(width <= 0 || height <= 0) {
        width = texture->getWidth();
        height = texture->getHeight();
    }

    if(resizable)
        flags |= FLAG_RESIZABLE;
}

void
Image::resize(float width, float height)
{
    this->width = width;
    this->height = height;
}

void
Image::draw(Painter& painter)
{
    if(width != texture->getWidth() || height != texture->getHeight())
        painter.drawStretchTexture(texture, Rect2D(0, 0, width, height));
    else
        painter.drawTexture(texture, Vector2(0, 0));
}

std::string Image::getFilename() const
{
    return filename;
}

void Image::setFile(const std::string &pfilename)
{
    filename=pfilename;
    texture = 0;
    texture = texture_manager->load(pfilename);
}

IMPLEMENT_COMPONENT_FACTORY(Image)


/** @file gui/Image.cpp */

