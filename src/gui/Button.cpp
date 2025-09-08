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
 * @file Button.cpp
 */

#include <SDL.h>                          // for SDL_GetTicks, Uint32
#include <cassert>                        // for assert
#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml++/ustring.h>             // for ustring
#include <memory>                         // for unique_ptr
#include <utility>                        // for move
#include <vector>                         // for vector

#include "Button.hpp"
#include "ComponentFactory.hpp"           // for IMPLEMENT_COMPONENT_FACTORY
#include "Event.hpp"                      // for Event
#include "Image.hpp"                      // for Image
#include "Painter.hpp"                    // for Painter
#include "Paragraph.hpp"                  // for Paragraph
#include "TooltipManager.hpp"             // for tooltipManager, TOOLTIP_TIME
#include "util/gettextutil.hpp"           // for _
#include "util/xmlutil.hpp"               // for unexpectedXmlElement, xmlParse

Button::Button()
    : state(STATE_NORMAL), lowerOnClick(true), mouseholdTicks(0)
{
    fixWidth = -1;
    fixHeight = -1;
}

void
Button::parse(xmlpp::TextReader& reader) {
  while(reader.move_to_next_attribute()) {
    xmlpp::ustring name = reader.get_name();
    xmlpp::ustring value = reader.get_value();
    if(parseAttribute(reader));
    else if(name == "width")
      fixWidth = xmlParse<float>(value);
    else if(name == "height")
      fixHeight = xmlParse<float>(value);
    else if(name == "lower")
      lowerOnClick = xmlParse<bool>(value);
    else
      unexpectedXmlAttribute(reader);
  }
  reader.move_to_element();

  // we need 4 child components
  childs.assign(4, Child());

  // parse contents of the xml-element
  if(!reader.is_empty_element() && reader.read())
  while(reader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
    if(reader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
      reader.next();
      continue;
    }

    std::string element = reader.get_name();
    if(element == "image") {
      setChildImage(comp_normal(), reader);
    } else if(element == "text") {
      setChildText(comp_normal(), reader);
    } else if(element == "image-hover") {
      setChildImage(comp_hover(), reader);
    } else if(element == "text-hover") {
      setChildText(comp_hover(), reader);
    } else if(element == "image-clicked") {
      setChildImage(comp_clicked(), reader);
    } else if(element == "text-clicked") {
      setChildText(comp_clicked(), reader);
    } else if(element == "image-caption") {
      setChildImage(comp_caption(), reader);
    } else if(element == "text-caption") {
      setChildText(comp_caption(), reader);
    } else if(element == "tooltip") {
      tooltip = _(reader.read_inner_xml());
    } else {
      unexpectedXmlElement(reader);
    }
    reader.next();
  }

  if(!comp_normal().getComponent())
    missingXmlElement(reader, "image"); // or "text"

  reLayout();
}

void
Button::reLayout()
{
    // if no width/height was specified we use the one from the biggest image
    if(fixWidth <= 0 || fixHeight <= 0) {
        width = 0;
        height = 0;
        for(Childs::iterator i = childs.begin(); i != childs.end(); ++i) {
            Component* component = i->getComponent();
            if(!component)
                continue;
            if(component->getFlags() & FLAG_RESIZABLE)
                component->resize(-1, -1);

            if(component->getWidth() > width)
                width = component->getWidth();
            if(component->getHeight() > height)
                height = component->getHeight();
        }
    } else {
        for(Childs::iterator i = childs.begin(); i != childs.end(); ++i) {
            Component* component = i->getComponent();
            if(!component)
                continue;
            if(component->getFlags() & FLAG_RESIZABLE)
                component->resize(fixWidth, fixHeight);
        }
        width = fixWidth;
        height = fixHeight;
    }

    // place components at the middle of the button
    for(Childs::iterator i = childs.begin(); i != childs.end(); ++i) {
        Child& child = *i;
        Component* component = child.getComponent();
        if(!component)
            continue;

        child.setPos( Vector2 ((width - component->getWidth())/2,
                               (height - component->getHeight())/2));
    }
}

void
Button::setChildImage(Child& child, xmlpp::TextReader& reader) {
  if(child.getComponent())
    unexpectedXmlElement(reader);
  std::unique_ptr<Image> image(new Image());
  image->parse(reader);
  resetChild(child, std::move(image));
}

void
Button::setChildText(Child& child, xmlpp::TextReader& reader) {
  if(child.getComponent())
    unexpectedXmlElement(reader);
  std::unique_ptr<Paragraph> paragraph(new Paragraph());
  paragraph->parse(reader);
  resetChild(child, std::move(paragraph));
}

void Button::setCaptionText(const std::string &pText)
{
    Child &c=comp_caption();
    Component *cm=c.getComponent();
    if(cm)
    {
        Paragraph *p=dynamic_cast<Paragraph*>(cm);
        if(p)
            p->setText(pText);
    }

    // place components at the middle of the button
    for(Childs::iterator i = childs.begin(); i != childs.end(); ++i) {
        Child& child = *i;
        if(!child.getComponent())
            continue;
        Component* component = child.getComponent();

        child.setPos( Vector2 ((width - component->getWidth())/2,
                    (height - component->getHeight())/2));
    }
}

std::string Button::getCaptionText()
{
  std::string s;
  Child &c=comp_caption();
  Component *cm=c.getComponent();
  if(cm)
  {
    Paragraph *p=dynamic_cast<Paragraph*>(cm);
    if(p)
      s=p->getText();
  }

  return s;
}


Button::~Button()
{
}

void
Button::event(const Event& event)
{
    State oldState = state;
    switch(event.type) {
        case Event::MOUSEMOTION:
            if(event.inside) {
                if(state == STATE_NORMAL) {
                    state = STATE_HOVER;
                }
                mouseholdTicks = SDL_GetTicks();
                mouseholdPos = event.mousepos;
            } else {
                mouseholdTicks = 0;
                if(state == STATE_HOVER) {
                    state = STATE_NORMAL;
                }
            }
            break;
        case Event::MOUSEBUTTONDOWN:
            if(event.inside) {
                state = STATE_CLICKED;
                pressed(this);
            } else {
                state = STATE_NORMAL;
            }
            break;
        case Event::MOUSEBUTTONUP:
            if(event.inside && state == STATE_CLICKED) {
                clicked(this);
            }
            released(this);
            state = event.inside ? STATE_HOVER : STATE_NORMAL;
            break;
        case Event::UPDATE: {
                Uint32 ticks = SDL_GetTicks();
                if(mouseholdTicks != 0 && ticks - mouseholdTicks > TOOLTIP_TIME) {
                    if(tooltipManager && tooltip != "") {
                        tooltipManager->showTooltip(tooltip,
                                relative2Global(mouseholdPos));
                    }
                    mouseholdTicks = 0;
                }
            }
            break;
        default:
            break;
    }
    if(state != oldState)
        setDirty();

    Component::event(event);
}

void
Button::draw(Painter& painter)
{
    switch(state) {
        case STATE_CLICKED:
            if(comp_clicked().isEnabled()) {
                drawChild(comp_clicked(), painter);
                break;
            }
            // fallthrough
        case STATE_HOVER:
            if(comp_hover().isEnabled()) {
                drawChild(comp_hover(), painter);
                break;
            }
            // fallthrough
        case STATE_NORMAL:
            drawChild(comp_normal(), painter);
            break;

        default:
            assert(false);
    }
    if(lowerOnClick)
    {
        if(state == STATE_CLICKED)
        {
            painter.pushTransform();
            painter.translate(Vector2(3,3));
        }
        else if(state == STATE_HOVER)
        {
            painter.pushTransform();
            painter.translate(Vector2(1,1));
        }
    }
    if(comp_caption().isEnabled())
    {   drawChild(comp_caption(), painter);}
    if(lowerOnClick && (state==STATE_CLICKED || state == STATE_HOVER))
    {   painter.popTransform();}
}

IMPLEMENT_COMPONENT_FACTORY(Button)

/** @file gui/Button.cpp */
