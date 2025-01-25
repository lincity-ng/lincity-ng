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
 * @file ScrollBar.cpp
 */

#include "ScrollBar.hpp"

#include <assert.h>              // for assert
#include <libxml/xmlreader.h>    // for XML_READER_TYPE_ELEMENT
#include <stdio.h>               // for sscanf
#include <string.h>              // for strcmp
#include <functional>            // for bind, _1, function
#include <iostream>              // for basic_ostream, operator<<, cerr, str...
#include <memory>                // for unique_ptr
#include <sstream>               // for basic_stringstream
#include <stdexcept>             // for runtime_error
#include <string>                // for char_traits, basic_string, operator==

#include "Button.hpp"            // for Button
#include "ComponentFactory.hpp"  // for IMPLEMENT_COMPONENT_FACTORY
#include "Event.hpp"             // for Event
#include "Vector2.hpp"           // for Vector2
#include "XmlReader.hpp"         // for XmlReader

using namespace std::placeholders;

static const float SCROLLSPEED = 200;

ScrollBar::ScrollBar()
    : minVal(0), maxVal(0), currentVal(0), scrollspeed(0), scrolling(false)
{
    setFlags(FLAG_RESIZABLE);
}

ScrollBar::~ScrollBar()
{
}

void
ScrollBar::parse(XmlReader& reader)
{
    width = 30; // default width...
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttribute(attribute, value)) {
            continue;
        } else if(strcmp(attribute, "width") == 0) {
            if(sscanf(value, "%f", &width) != 1) {
                std::stringstream msg;
                msg << "Couldn't parse width value '"
                    << value << "'.";
                throw std::runtime_error(msg.str());
            }
        } else {
            std::cerr << "Skipping unknown attribute '"
                << attribute << "'.\n";
        }
    }

    // we have 3 child components
    while(childs.size() < 3)
        childs.push_back(Child());

    int depth = reader.getDepth();
    while(reader.read() && reader.getDepth() > depth) {
        if(reader.getNodeType() == XML_READER_TYPE_ELEMENT) {
            std::string element = (const char*) reader.getName();
            if(element == "button1") {
                std::unique_ptr<Button> button(new Button());
                button->parse(reader);
                resetChild(button1(), button.release());
            } else if(element == "button2") {
                std::unique_ptr<Button> button(new Button());
                button->parse(reader);
                resetChild(button2(), button.release());
            } else if(element == "scroller") {
                std::unique_ptr<Button> button(new Button());
                button->parse(reader);
                resetChild(scroller(), button.release());
            } else {
                std::cerr << "Skipping unknown element '"
                    << element << "'.\n";
                reader.nextNode();
            }
        }
    }
    if(scroller().getComponent() == 0 || button1().getComponent() == 0
            || button2().getComponent() == 0) {
        throw std::runtime_error("Not all components specified for scrollbar.");
    }

    Button* b1 = dynamic_cast<Button*> (button1().getComponent());
    if(!b1)
        throw std::runtime_error("Button1 of ScrollBar not a button.");
    b1->pressed.connect(std::bind(&ScrollBar::buttonPressed, this, _1));
    b1->released.connect(std::bind(&ScrollBar::buttonReleased, this, _1));
    Button* b2 = dynamic_cast<Button*> (button2().getComponent());
    if(!b2)
        throw std::runtime_error("Button2 of ScrollBar not a button.");
    b2->pressed.connect(std::bind(&ScrollBar::buttonPressed, this, _1));
    b2->released.connect(std::bind(&ScrollBar::buttonReleased, this, _1));
}

void
ScrollBar::resize(float newwidth, float newheight)
{
    (void) newwidth;

    // ensure a minimum height...
    float minHeight = scroller().getComponent()->getHeight()
        + button1().getComponent()->getHeight()
        + button2().getComponent()->getHeight()
        + 32;
    if(newheight < minHeight)
        newheight = minHeight;
    this->height = newheight;

    button1().setPos(Vector2(0, 0));
    button2().setPos(Vector2(0, height-button2().getComponent()->getHeight()));
    setValue(currentVal);

    setDirty();
}

void
ScrollBar::draw(Painter& painter)
{
    // TODO draw rectangle around scrollbar...
    Component::draw(painter);
}

void
ScrollBar::event(const Event& event)
{
    switch(event.type) {
        case Event::MOUSEBUTTONDOWN:
            if(event.inside && scroller().inside(event.mousepos)) {
                scrolling = true;
                scrollOffset = event.mousepos.y - scroller().getPos().y;
            }
            break;

        case Event::MOUSEBUTTONUP:
            scrolling = false;
            break;

        case Event::MOUSEMOTION: {
            if(!scrolling)
                break;

            float val = event.mousepos.y - scrollOffset;
            if(val < button1().getComponent()->getHeight())
                val = button1().getComponent()->getHeight();
            if(val > button2().getPos().y
                    - scroller().getComponent()->getHeight())
                val = button2().getPos().y
                    - scroller().getComponent()->getHeight();
            scroller().setPos(Vector2(0, val));

            // map val to scrollrange...
            float scrollScreenRange = height
                - button1().getComponent()->getHeight()
                - button2().getComponent()->getHeight()
                - scroller().getComponent()->getHeight();
            float scrollScreenRatio
                = (val - button1().getComponent()->getHeight()) /
                        scrollScreenRange;
            float newScrollVal = minVal +
                ((maxVal - minVal) * scrollScreenRatio);
            assert(newScrollVal >= minVal && newScrollVal <= maxVal);
            currentVal = newScrollVal;
            valueChanged(this, newScrollVal);
            setDirty();
            break;
        }
        case Event::UPDATE: {
            if(scrollspeed == 0)
                break;

            float newVal = currentVal + scrollspeed * event.elapsedTime;
            if(newVal < minVal)
                newVal = minVal;
            if(newVal > maxVal)
                newVal = maxVal;
            if(newVal != currentVal) {
                setValue(newVal);
                valueChanged(this, currentVal);
            }
            break;
        }
        default:
            break;
    }

    Component::event(event);
}

void
ScrollBar::setRange(float min, float max)
{
    if(max < min) {
        throw std::runtime_error("Invalid scroll range specified (max < min)");
    }
    minVal = min;
    maxVal = max;

    if(currentVal < minVal)
        currentVal = minVal;
    else if(currentVal > maxVal)
        currentVal = maxVal;
}

void
ScrollBar::setValue(float value)
{
    if(value < minVal)
        value = minVal;
    else if(value > maxVal)
        value = maxVal;

    float scrollScreenRange = height
        - button1().getComponent()->getHeight()
        - button2().getComponent()->getHeight()
        - scroller().getComponent()->getHeight();
    float range = maxVal - minVal;
    float y = button1().getComponent()->getHeight();
    if(range != 0) {
        y += value * scrollScreenRange / range;
    }
    currentVal = value;
    scroller().setPos(Vector2(0, y));
    setDirty();
}

void
ScrollBar::buttonPressed(Button* button)
{
    if(button == button1().getComponent()) {
        scrollspeed = -SCROLLSPEED;
    } else {
        scrollspeed = SCROLLSPEED;
    }
}

void
ScrollBar::buttonReleased(Button* )
{
    scrollspeed = 0;
}

IMPLEMENT_COMPONENT_FACTORY(ScrollBar)

/** @file gui/ScrollBar.cpp */
