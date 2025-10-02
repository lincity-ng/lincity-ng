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
**/

#include "DocumentImage.hpp"

#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml++/ustring.h>             // for ustring
#include <filesystem>                     // for path

#include "Painter.hpp"                    // for Painter
#include "Texture.hpp"                    // for Texture
#include "TextureManager.hpp"             // for TextureManager, texture_man...
#include "Vector2.hpp"                    // for Vector2
#include "util/xmlutil.hpp"               // for xmlParse, missingXmlAttribute

DocumentImage::DocumentImage()
  : texture(0)
{}

DocumentImage::~DocumentImage()
{}

void
DocumentImage::parse(xmlpp::TextReader& reader, const Style& parentstyle) {
  style = parentstyle;
  std::filesystem::path filename;

  while(reader.move_to_next_attribute()) {
    xmlpp::ustring name = reader.get_name();
    xmlpp::ustring value = reader.get_value();
    if(parseAttribute(reader));
    else if(style.parseAttribute(reader));
    else if(name == "width")
      width = xmlParse<float>(value);
    else if(name == "height")
      height = xmlParse<float>(value);
    else if(name == "src") {
      filename = xmlParse<std::filesystem::path>(value);
    }
    else
      unexpectedXmlAttribute(reader);
  }
  reader.move_to_element();
  if(filename.empty())
    missingXmlAttribute(reader, "src");

	texture = texture_manager->load(filename);
  width = texture->getWidth();
  height = texture->getHeight();
}

void
DocumentImage::resize(float newWidth, float newHeight) {
  if(newWidth == -1.f && newHeight == -1.f) {
    width = texture->getWidth();
    height = texture->getHeight();
  }
  else if(newHeight == -1.f) {
    if(newWidth > texture->getWidth()) {
      width = texture->getWidth();
      height = texture->getHeight();
    }
    else {
      width = newWidth;
      height = newWidth * texture->getHeight() / texture->getWidth();
    }
  }
  else if(newWidth == -1.f) {
    if(newHeight > texture->getHeight()) {
      width = texture->getWidth();
      height = texture->getHeight();
    }
    else {
      width = newHeight * texture->getWidth() / texture->getHeight();
      height = newHeight;
    }
  }
  else {
    width = newWidth;
    height = newHeight;
  }

  setDirty();
}

void
DocumentImage::draw(Painter& painter)
{
    painter.drawStretchTexture(texture, Rect2D(0, 0, width, height));
}


/** @file gui/DocumentImage.cpp */
