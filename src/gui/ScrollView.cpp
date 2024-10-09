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
 * @file ScrollView.cpp
 */

#include "ScrollView.hpp"

#include <libxml/xmlreader.h>    // for XML_READER_TYPE_ELEMENT
#include <functional>            // for bind, function, _1, _2
#include <iostream>              // for basic_ostream, operator<<, cerr
#include <memory>                // for unique_ptr
#include <stdexcept>             // for runtime_error
#include <string>                // for char_traits, basic_string, operator==

#include "ComponentFactory.hpp"  // for IMPLEMENT_COMPONENT_FACTORY
#include "ComponentLoader.hpp"   // for parseEmbeddedComponent
#include "Event.hpp"             // for Event
#include "Rect2D.hpp"            // for Rect2D
#include "ScrollBar.hpp"         // for ScrollBar
#include "Signal.hpp"            // for Signal
#include "Vector2.hpp"           // for Vector2
#include "XmlReader.hpp"         // for XmlReader

#ifdef DEBUG
#include <assert.h>
#endif

using namespace std::placeholders;

static const float MOUSEWHEELSCROLL = 90;

ScrollView::ScrollView()
{
}

ScrollView::~ScrollView()
{
}

void
ScrollView::parse(XmlReader& reader)
{
    // parse xml attributes
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttribute(attribute, value)) {
            continue;
        } else {
            std::cerr << "Skipping unknown attribute '"
                << attribute << "'.\n";
        }
    }

    // we need 2 child components
    childs.assign(2, Child());

    // parse xml contents
    int depth = reader.getDepth();
    while(reader.read() && reader.getDepth() > depth) {
        if(reader.getNodeType() == XML_READER_TYPE_ELEMENT) {
            std::string element = (const char*) reader.getName();

            if(element == "scrollbar") {
                std::unique_ptr<ScrollBar> scrollbar (new ScrollBar());
                scrollbar->parse(reader);
                resetChild(scrollBar(), scrollbar.release());
            } else if(element == "contents") {
                resetChild(contents(), parseEmbeddedComponent(reader));
            } else {
                std::cerr << "Skipping unknown element '" << element << "'.\n";
            }
        }
    }

    if(scrollBar().getComponent() == 0) {
        throw std::runtime_error("No ScrollBar specified in ScrollView");
    }
    ScrollBar* scrollBarComponent = (ScrollBar*) scrollBar().getComponent();
    scrollBarComponent->valueChanged.connect(
      std::bind(&ScrollView::scrollBarChanged, this, _1, _2));

    setFlags(FLAG_RESIZABLE);
}

void
ScrollView::resize(float newwidth, float newheight)
{
    if(newwidth < 1) newwidth = 1;
    if(newheight < 1) newheight = 1;
    float scrollBarWidth = scrollBar().getComponent()->getWidth();
    scrollBar().getComponent()->resize(scrollBarWidth, newheight);
    scrollBar().setPos(Vector2(newwidth - scrollBarWidth, 0));
    if(newwidth < scrollBar().getComponent()->getWidth() + 1)
        newwidth = scrollBar().getComponent()->getWidth() + 1;
    if(newheight != scrollBar().getComponent()->getHeight())
        newheight = scrollBar().getComponent()->getHeight();

    float scrollarea = 0;
    if(contents().getComponent() != 0) {
        Component* component = contents().getComponent();
        if(component->getFlags() & FLAG_RESIZABLE)
            component->resize(newwidth - scrollBarWidth, newheight);
        contents().setClipRect(
                Rect2D(0, 0, newwidth - scrollBarWidth, newheight));
        scrollarea = component->getHeight() - newheight;
        if(scrollarea < 0)
            scrollarea = 0;
    }

    ScrollBar* scrollBarComponent = (ScrollBar*) scrollBar().getComponent();
    scrollBarComponent->setRange(0, scrollarea);
    scrollBarComponent->setValue(0);

    width = newwidth;
    height = newheight;

    setDirty();
}

void
ScrollView::scrollBarChanged(ScrollBar* , float newvalue)
{
    contents().setPos(Vector2(0, -newvalue));
    setDirty();
}

void
ScrollView::event(const Event& event)
{
    if(event.type == Event::MOUSEWHEEL) {
        if(!event.inside)
            return;

        ScrollBar* scrollBarComp
            = dynamic_cast<ScrollBar*> (scrollBar().getComponent());
        if(scrollBarComp == 0) {
#ifdef DEBUG
            assert(false);
#endif
            return;
        }
        float val = - contents().getPos().y;
        val -= event.scrolly * 20;
        if(val < scrollBarComp->getRangeMin())
            val = scrollBarComp->getRangeMin();
        if(val > scrollBarComp->getRangeMax())
            val = scrollBarComp->getRangeMax();
        contents().setPos(Vector2(0, -val));
        scrollBarComp->setValue(val);
        setDirty();
    }

    Component::event(event);
}

void
ScrollView::replaceContents(Component* component)
{
    resetChild(contents(), component);
    contents().setPos(Vector2(0, 0));
    resize(width, height);
}

IMPLEMENT_COMPONENT_FACTORY(ScrollView)


/** @file gui/ScrollView.cpp */
