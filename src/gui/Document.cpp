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
 * @file Document.cpp
 */

#include <config.h>

#include "Document.hpp"

#include <iostream>

#include "Paragraph.hpp"
#include "XmlReader.hpp"
#include "Vector2.hpp"
#include "Painter.hpp"
#include "ComponentFactory.hpp"
#include "DocumentImage.hpp"
#include "callback/Callback.hpp"

Document::Document()
{
    setFlags(FLAG_RESIZABLE);
}

Document::~Document()
{
}

void
Document::parse(XmlReader& reader)
{
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttribute(attribute, value)) {
            continue;
        } else if(style.parseAttribute(attribute, value)) {
            continue;
        } else if(strcmp(attribute, "src") == 0) {
            XmlReader fileReader(value);
            parse(fileReader);
            return;
        } else {
            std::cerr << "Skipping unknown attribute '"
                << attribute << "'.\n";
        }
    }

    int depth = reader.getDepth();
    while(reader.read() && reader.getDepth() > depth) {
        if(reader.getNodeType() == XML_READER_TYPE_ELEMENT) {
            std::string node = (const char*) reader.getName();
            if(node == "p" || node=="Paragraph" || node == "li") {
                std::auto_ptr<Paragraph> paragraph (new Paragraph());
                if(node != "li") {
                    paragraph->parse(reader, style);
                } else {
                    paragraph->parseList(reader, style);
                }
                paragraph->linkClicked.connect(
                    makeCallback(*this, &Document::paragraphLinkClicked));
                addChild(paragraph.release());
            } else if(node == "img") {
                std::auto_ptr<DocumentImage> image (new DocumentImage());
                image->parse(reader, style);
                addChild(image.release());
            } else {
                std::cerr << "Skipping unknown node type '" << node << "'.\n";
                reader.nextNode();
            }
        } else if(reader.getNodeType() == XML_READER_TYPE_TEXT) {
            // TODO create anonymous paragraph...
            std::cerr << "Warning: text outside paragraph not allowed (yet).\n";
        }
    }
}

void
Document::resize(float newwidth, float newheight)
{
    height = 0;
    for(Childs::iterator i = childs.begin(); i != childs.end(); ++i) {
        Child& child = *i;
        Component* component = child.getComponent();
        DocumentElement* element = dynamic_cast<DocumentElement*> (component);
        if(!element) {
            std::cerr << "Component not a DocumentElement in Document::resize!\n";
            continue;
        }

        float compwidth = newwidth - element->getStyle().margin_left - element->getStyle().margin_right;
        if(compwidth < 0)
            compwidth = 0;
        
        component->resize(compwidth, -1);
        float posx = element->getStyle().margin_left;
        switch(element->getStyle().alignment) 
        {
            case Style::ALIGN_LEFT:
                posx += 0;
                break;
            case Style::ALIGN_RIGHT:
                posx += compwidth - component->getWidth();
                break;
            case Style::ALIGN_CENTER:
                posx += (compwidth - component->getWidth()) / 2;
                break;
            default:
                assert(false);
                break;
        }
        height += element->getStyle().margin_top;
        child.setPos(Vector2(posx, height));
        height += component->getHeight() + element->getStyle().margin_bottom;
    }
    width = newwidth;

    if(height < newheight)
        height = newheight;
}

void
Document::addParagraph(Paragraph* paragraph)
{
    paragraph->linkClicked.connect(
            makeCallback(*this, &Document::paragraphLinkClicked));
    addChild(paragraph);
    resize(width, height);
}

void
Document::draw(Painter& painter)
{
    if(style.background.a != 0) {
        painter.setFillColor(style.background);
        painter.fillRectangle(Rect2D(0, 0, width, height));
    }

    Component::draw(painter);
}

void
Document::paragraphLinkClicked(Paragraph* paragraph, const std::string& href)
{
    linkClicked(paragraph, href);
}

IMPLEMENT_COMPONENT_FACTORY(Document)

/** @file gui/Document.cpp */

