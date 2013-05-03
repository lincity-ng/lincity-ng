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
 * @file TooltipManager.cpp
 */

#include <config.h>

#include "TooltipManager.hpp"
#include "XmlReader.hpp"
#include "ComponentFactory.hpp"
#include "Paragraph.hpp"
#include "Document.hpp"
#include "Event.hpp"

TooltipManager* tooltipManager = 0;

TooltipManager::TooltipManager()
{
    childs.assign(1, Child());
    if(tooltipManager == 0)
        tooltipManager = this;

    setFlags(FLAG_RESIZABLE);
}

TooltipManager::~TooltipManager()
{
    if(tooltipManager == this)
        tooltipManager = 0;
}

void
TooltipManager::parse(XmlReader& reader)
{
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttribute(attribute, value)) {
            continue;
        } else {
            std::cerr << "Skipping unknown attribute '" << attribute
                      << "' in TooltipManager.\n";
        }
    }

    int depth = reader.getDepth();
    while(reader.read() && reader.getDepth() > depth) {
        if(reader.getNodeType() == XML_READER_TYPE_ELEMENT) {
            const char* element = (const char*) reader.getName();
            std::cerr << "Skipping unknown child '" << element
                      << "in TooltipManager.\n";
        }
    }
}

void
TooltipManager::resize(float width, float height)
{
    this->width = width;
    this->height = height;
}

bool
TooltipManager::opaque(const Vector2& ) const
{
    return false;
}

void
TooltipManager::event(const Event& event)
{
    if(event.type == Event::MOUSEMOTION) {
        if(comp_tooltip().getComponent() != 0) {
            comp_tooltip().setComponent(0);
        }
    }    
}

void
TooltipManager::showTooltip(const std::string& text, const Vector2& pos)
{
    std::auto_ptr<Document> d (new Document());
    std::auto_ptr<Paragraph> p (new Paragraph());

    std::map<std::string, Style>::iterator s = styleRegistry.find("tooltip");
    if(s == styleRegistry.end()) {
        std::cerr << "Warning: No tooltip style defined.\n";
        p->setText(text);
    } else {
        p->setText(text, s->second);
        d->style = s->second;
    }
    d->addParagraph(p.release());
    d->resize(250, -1);
    Vector2 dest = pos + Vector2(-100, 26);
    if(dest.x < 20)
        dest.x = 20;
    if(dest.y < 20)
        dest.y = 20;
    if(dest.x + d->getWidth() > getWidth() - 20)
        dest.x = getWidth() - 20 - d->getWidth();
    if(dest.y + d->getHeight() > getHeight() - 20)
        dest.y = pos.y - 20 - d->getHeight();
    /* Show minimap tooltip above. Hardcoded size corresponding to .xml  */
    if(dest.x > getWidth() - 310)
        dest.y = pos.y - 20 - d->getHeight();
    comp_tooltip().setComponent(d.release());
    comp_tooltip().setPos(dest);
		setDirty();
}

IMPLEMENT_COMPONENT_FACTORY(TooltipManager)

/** @file gui/TooltipManager.cpp */

