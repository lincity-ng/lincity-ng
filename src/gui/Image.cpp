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

#include "Image.hpp"

#include <assert.h>                       // for assert
#include <fmt/base.h>                     // for println
#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml++/ustring.h>             // for ustring
#include <stdio.h>                        // for stderr
#include <filesystem>                     // for path

#include "ComponentFactory.hpp"           // for IMPLEMENT_COMPONENT_FACTORY
#include "Painter.hpp"                    // for Painter
#include "Rect2D.hpp"                     // for Rect2D
#include "Texture.hpp"                    // for Texture
#include "TextureManager.hpp"             // for TextureManager, texture_man...
#include "Vector2.hpp"                    // for Vector2
#include "util/xmlutil.hpp"               // for xmlParse, missingXmlAttribute

Image::Image()
    : texture(0)
{
}

Image::~Image()
{
}

void
Image::parse(xmlpp::TextReader& reader) {
  bool resizable = false;
  bool grey = false;

  while(reader.move_to_next_attribute()) {
    xmlpp::ustring name = reader.get_name();
    xmlpp::ustring value = reader.get_value();
    if(parseAttribute(reader));
    else if(name == "width")
      width = xmlParse<float>(value);
    else if(name == "height")
      height = xmlParse<float>(value);
    else if(name == "src")
      filename = xmlParse<std::filesystem::path>(value);
    else if(name == "filter") {
      if(value == "grey") {
        grey = true;
      } else if(value == "no") {
        grey = false;
      } else {
        fmt::println(stderr,
          "warning: unknown filter value {:?}. Should be 'grey' or 'no'.",
          value
        );
        assert(false);
      }
    }
    else if(name == "resizable")
      resizable = xmlParse<bool>(value);
    else
      unexpectedXmlAttribute(reader);
  }
  reader.move_to_element();

  if(filename.empty())
    missingXmlAttribute(reader, "src");

  texture = texture_manager->load(filename,
    grey ? TextureManager::FILTER_GREY : TextureManager::NO_FILTER);

  if(width <= 0 || height <= 0) {
      width = texture->getWidth();
      height = texture->getHeight();
  }

  if(resizable) {
    flags |= FLAG_RESIZABLE;
    texture->setScaleMode(Texture::ScaleMode::ANISOTROPIC);
  }
}

void
Image::resize(float width, float height)
{
    if(width < 0) width = 0;
    if(height < 0) height = 0;
    this->width = width;
    this->height = height;
}

void
Image::draw(Painter& painter) {
  if(flags & FLAG_RESIZABLE)
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

    if(width <= 0 || height <= 0) {
      width = texture->getWidth() + 1;
      height = texture->getHeight() + 1;
    }
    if(flags & FLAG_RESIZABLE) {
      texture->setScaleMode(Texture::ScaleMode::ANISOTROPIC);
    }
}

IMPLEMENT_COMPONENT_FACTORY(Image)


/** @file gui/Image.cpp */
