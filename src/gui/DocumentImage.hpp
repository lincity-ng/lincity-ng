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
 * @file DocumentImage.hpp
 */

#ifndef __DOCUMENTIMAGE_HPP__
#define __DOCUMENTIMAGE_HPP__

#include "Component.hpp"
#include "DocumentElement.hpp"
#include "Style.hpp"
#include "Texture.hpp"

/**
 * @class DocumentImage
 */
class DocumentImage : public Component, public DocumentElement {
public:
    DocumentImage();
    virtual ~DocumentImage();

    void parse(XmlReader& reader, const Style& style);

    void resize(float width, float height);
    void draw(Painter& painter);
    const Style& getStyle() const
    {
        return style;
    }

private:
    Texture* texture;
    Style style;
    std::string filename;
};

#endif


/** @file gui/DocumentImage.hpp */

