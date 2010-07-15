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
 * @file Document.hpp
 */

#ifndef __DOCUMENT_HPP__
#define __DOCUMENT_HPP__

#include <vector>
#include "Style.hpp"
#include "Component.hpp"
#include "callback/Signal.hpp"

class XmlReader;
class Paragraph;

/**
 * @class Document
 */
class Document : public Component
{
public:
    Document();
    virtual ~Document();

    void parse(XmlReader& reader);

    void draw(Painter& painter);
    void resize(float width, float height);
    void addParagraph(Paragraph* p);

    Style style;
    Signal<Paragraph*, const std::string& > linkClicked;

private:
    void paragraphLinkClicked(Paragraph* paragraph, const std::string& href);
};

#endif


/** @file gui/Document.hpp */

