/*
Copyright (C) 2005 David Kamphausen <david.kamphausen@web.de>
Copyright (C) 2024 David Bears <dbear4q@gmail.com>

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

#include "CheckButton.hpp"

#include <SDL.h>                          // for SDL_GetTicks, SDL_BUTTON_LEFT
#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml++/ustring.h>             // for ustring
#include <memory>                         // for unique_ptr
#include <utility>                        // for move
#include <vector>                         // for vector

#include "Child.hpp"                      // for Child, Childs
#include "Component.hpp"                  // for Component, Component::FLAG_...
#include "Signal.hpp"                     // for Signal
#include "Vector2.hpp"                    // for Vector2
#include "gui/ComponentFactory.hpp"       // for IMPLEMENT_COMPONENT_FACTORY
#include "gui/Event.hpp"                  // for Event
#include "gui/Image.hpp"                  // for Image
#include "gui/Painter.hpp"                // for Painter
#include "gui/Paragraph.hpp"              // for Paragraph
#include "gui/TooltipManager.hpp"         // for tooltipManager, TOOLTIP_TIME
#include "util/gettextutil.hpp"           // for _
#include "util/xmlutil.hpp"               // for xmlParse, unexpectedXmlElement

CheckButton::CheckButton() :
  autoCheck(true),
  autoUncheck(true),
  lowerOnClick(true)
{}

CheckButton::~CheckButton() {}

void
CheckButton::parse(xmlpp::TextReader& reader) {
  while(reader.move_to_next_attribute()) {
    xmlpp::ustring name = reader.get_name();
    xmlpp::ustring value = reader.get_value();
    if(parseAttribute(reader));
    else if(name == "width")
      width = xmlParse<float>(value);
    else if(name == "height")
      height = xmlParse<float>(value);
    else if(name == "auto-check")
      autoCheck = xmlParse<bool>(value);
    else if(name == "auto-uncheck")
      autoUncheck = xmlParse<bool>(value);
    else if(name == "lower")
      lowerOnClick = xmlParse<bool>(value);
    else if(name == "checked")
      mchecked = xmlParse<bool>(value);
    else
      unexpectedXmlAttribute(reader);
  }
  reader.move_to_element();

  // we need 6 child components
  childs.assign(6, Child());

  // parse contents of the xml-element
  if(!reader.is_empty_element() && reader.read())
  while(reader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
    if(reader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
      reader.next();
      continue;
    }
    xmlpp::ustring element = reader.get_name();
    if(element == "image") {
      setChildImage(comp_normal(), reader);
    } else if(element == "text") {
      setChildText(comp_normal(), reader);
    } else if(element == "image-hover") {
      setChildImage(comp_hover(), reader);
    } else if(element == "image-checked") {
      setChildImage(comp_checked(), reader);
    } else if(element == "text-hover") {
      setChildText(comp_hover(), reader);
    } else if(element == "image-clicked") {
      setChildImage(comp_clicked(), reader);
    } else if(element == "text-clicked") {
      setChildText(comp_clicked(), reader);
    } else if(element == "image-caption") {
      setChildImage(comp_caption(), reader);
    } else if(element == "image-disabled") {
      setChildImage(comp_disabled(), reader);
    } else if(element == "text-caption") {
      setChildText(comp_caption(), reader);
    } else if (element == "tooltip") {
      tooltip = _(reader.read_inner_xml());
    } else {
      unexpectedXmlElement(reader);
    }
    reader.next();
  }

  if(!comp_normal().getComponent())
    missingXmlElement(reader, "image");

  // if no width/height was specified we use the one from the biggest image
  if(width <= 0 || height <= 0) {
    for(Childs::iterator i = childs.begin(); i != childs.end(); ++i) {
      Component* component = i->getComponent();
      if(!component) continue;
      if(component->getFlags() & FLAG_RESIZABLE)
        component->resize(-1, -1);

      if(component->getWidth() > width)
        width = component->getWidth();
      if(component->getHeight() > height)
        height = component->getHeight();
    }
  }

  // place components at the middle of the button
  for(Childs::iterator i = childs.begin(); i != childs.end(); ++i) {
    Child& child = *i;
    if(!child.getComponent()) continue;
    Component* component = child.getComponent();

    child.setPos((size - component->getSize()) / 2);
  }
}

void
CheckButton::setChildImage(Child& child, xmlpp::TextReader& reader) {
  if(child.getComponent())
    unexpectedXmlElement(reader);
  std::unique_ptr<Image> image(new Image());
  image->parse(reader);
  resetChild(child, std::move(image));
}

void
CheckButton::setChildText(Child& child, xmlpp::TextReader& reader) {
  std::unique_ptr<Paragraph> paragraph(new Paragraph());
  paragraph->parse(reader);
  resetChild(child, std::move(paragraph));
}

void
CheckButton::uncheck() {
  if(!mchecked)
    return;
  mchecked=false;
  unchecked(this);
  setDirty();
}

void
CheckButton::check() {
  if(mchecked)
    return;
  mchecked=true;
  checked(this);
  setDirty();
}

void
CheckButton::tryUncheck() {
  if(mdisabled)
    return;
  uncheck();
}

void
CheckButton::tryCheck() {
  if(mdisabled)
    return;
  check();
}

bool
CheckButton::isChecked() const {
  return mchecked;
}

void
CheckButton::setAutoCheck(bool check, bool uncheck) {
  autoCheck = check;
  autoUncheck = uncheck;
}

void
CheckButton::enable(bool enabled) {
  mdisabled = !enabled;
  setDirty();
}

bool
CheckButton::isEnabled() const {
  return !mdisabled;
}

void
CheckButton::event(const Event& event) {
  switch(event.type) {
  case Event::MOUSEMOTION:
    if((mhovered = event.inside)) {
      mouseholdTicks = SDL_GetTicks();
      mouseholdPos = event.mousepos;
    } else {
      mouseholdTicks = 0;
    }
    break;
  case Event::MOUSEBUTTONDOWN:
    if(!event.inside)
      break;
    mpressed = true;
    pressed(this, event.mousebutton);
    setDirty();
    break;
  case Event::MOUSEBUTTONUP:
    if(!mpressed)
      break;
    mpressed = false;
    if(event.inside) {
      if(event.mousebutton == SDL_BUTTON_LEFT && !mdisabled) {
        if(mchecked && autoUncheck) {
          mchecked = false;
          unchecked(this);
        }
        else if(!mchecked && autoCheck) {
          mchecked = true;
          checked(this);
        }
      }
      clicked(this, event.mousebutton);
    }
    released(this, event.mousebutton);
    setDirty();
    break;
  case Event::UPDATE: {
    Uint32 ticks = SDL_GetTicks();
    if(mouseholdTicks && ticks - mouseholdTicks > TOOLTIP_TIME) {
      if(tooltipManager && tooltip != "") {
        tooltipManager->showTooltip(tooltip,
          relative2Global(mouseholdPos));
      }
      mouseholdTicks = 0;
    }
    break;
  }
  default:
    break;
  }
  Component::event(event);
}

void
CheckButton::draw(Painter& painter) {
  // how to do if statements with fallthrough
  if(mdisabled)     goto draw_normal;
  else if(mpressed) goto draw_pressed;
  else if(mchecked) goto draw_checked;
  else if(mhovered) goto draw_hovered;
  else              goto draw_normal;
  draw_checked:
    if(comp_checked().isEnabled()) {
      drawChild(comp_checked(), painter);
      goto end_draw_child;
    }
  draw_pressed:
    if(comp_clicked().isEnabled()) {
        drawChild(comp_clicked(), painter);
        goto end_draw_child;
    }
  draw_hovered:
    if(comp_hover().isEnabled()) {
      drawChild(comp_hover(), painter);
      goto end_draw_child;
    }
  draw_normal:
    drawChild(comp_normal(), painter);
  end_draw_child:

  if(lowerOnClick)   {
    if(mpressed) {
      painter.pushTransform();
      painter.translate(Vector2(3,3));
    }
    else if(mhovered) {
      painter.pushTransform();
      painter.translate(Vector2(1,1));
    }
  }
  if(comp_caption().isEnabled()) {
    if(mdisabled && comp_disabled().isEnabled())
      drawChild(comp_disabled(), painter);
    else
      drawChild(comp_caption(), painter);
  }
  if(lowerOnClick && (mpressed || mhovered))
    painter.popTransform();
}

Component *CheckButton::getCaption()
{
  return comp_caption().getComponent();
}

void CheckButton::setCaptionText(const std::string &pText)
{
  Child &c=comp_caption();
  Component *cm=c.getComponent();
  if(cm)
  {
    Paragraph *p=dynamic_cast<Paragraph*>(cm);
    if(p)
      p->setText(pText);
  }
}

void CheckButton::setTooltip(const std::string &pText)
{
    tooltip = pText;
}

const std::string&
CheckButton::getTooltip() const {
  return tooltip;
}

std::string CheckButton::getCaptionText()
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

IMPLEMENT_COMPONENT_FACTORY(CheckButton)


/** @file gui/CheckButton.cpp */
