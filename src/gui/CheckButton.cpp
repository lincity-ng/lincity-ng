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

#include <SDL.h>                     // for SDL_GetTicks, SDL_BUTTON_LEFT
#include <ctype.h>                   // for isspace
#include <stdio.h>                   // for sscanf
#include <string.h>                  // for strcmp
#include <iostream>                  // for operator<<, basic_ostream, cerr
#include <memory>                    // for unique_ptr
#include <sstream>                   // for basic_stringstream
#include <stdexcept>                 // for runtime_error

#include "Child.hpp"                 // for Child, Childs
#include "Component.hpp"             // for Component, Component::FLAG_RESIZ...
#include "Signal.hpp"                // for Signal
#include "Vector2.hpp"               // for Vector2
#include "gui/ComponentFactory.hpp"  // for GUI_TRANSLATE, IMPLEMENT_COMPONE...
#include "gui/Event.hpp"             // for Event
#include "gui/Image.hpp"             // for Image
#include "gui/Painter.hpp"           // for Painter
#include "gui/Paragraph.hpp"         // for Paragraph
#include "gui/TooltipManager.hpp"    // for tooltipManager, TOOLTIP_TIME
#include "gui/XmlReader.hpp"         // for XmlReader
#include "libxml/xmlreader.h"        // for XML_READER_TYPE_ELEMENT, XML_REA...

CheckButton::CheckButton() :
  autoCheck(true),
  autoUncheck(true),
  lowerOnClick(true)
{}

CheckButton::~CheckButton() {}

static void
setBoolAttribute(const char *attr, const char *value, bool *dest) {
  if(strcmp(value, "true") == 0) {
    *dest = true;
  } else if(strcmp(value, "false") == 0) {
    *dest = false;
  } else {
    std::cerr << "Unknown value '" << value
      << "' for attribute '" << attr
      << "'. Should be 'true' or 'false'.\n";
  }
}

void
CheckButton::parse(XmlReader& reader)
{
    // parse xml attributes
    //std::cout << "parsing Checkbutton ...";
    //std::cout.flush();
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttribute(attribute, value)) {
            continue;
        } else if(strcmp(attribute, "width") == 0) {
            if(sscanf(value, "%f", &width) != 1) {
                std::stringstream msg;
                msg << "Couldn't parse width '" << value << "'.";
                throw std::runtime_error(msg.str());
            }
        } else if(strcmp(attribute, "height") == 0) {
            if(sscanf(value, "%f", &height) != 1) {
                std::stringstream msg;
                msg << "Couldn't parse height '" << value << "'.";
                throw std::runtime_error(msg.str());
            }
        } else if(strcmp(attribute, "auto-check") == 0) {
          setBoolAttribute(attribute, value, &autoCheck);
        } else if(strcmp(attribute, "auto-uncheck") == 0) {
          setBoolAttribute(attribute, value, &autoUncheck);
        } else if(strcmp(attribute, "lower") == 0) {
          setBoolAttribute(attribute, value, &lowerOnClick);
        } else if(strcmp(attribute, "direction") == 0) {
            // skip
        } else if(strcmp(attribute, "checked") == 0) {
          setBoolAttribute(attribute, value, &mchecked);
        } else {
            std::cerr << "Skipping unknown attribute '"
                << attribute << "'.\n";
        }
    }

    // we need 6 child components
    childs.assign(6, Child());

    // parse contents of the xml-element
    bool parseTooltip = false;
    int depth = reader.getDepth();
    while(reader.read() && reader.getDepth() > depth) {
        if(reader.getNodeType() == XML_READER_TYPE_ELEMENT) {
            std::string element = (const char*) reader.getName();
            if(element == "image") {
                if(comp_normal().getComponent() != 0)
                    std::cerr << "Warning: more than 1 component for state "
                        "comp_normal defined.\n";
                setChildImage(comp_normal(), reader);
            } else if(element == "text") {
                if(comp_normal().getComponent() != 0)
                    std::cerr << "Warning: more than 1 component for state "
                        "comp_normal defined.\n";
                setChildText(comp_normal(), reader);
            } else if(element == "image-hover") {
                if(comp_hover().getComponent() != 0)
                    std::cerr << "Warning: more than 1 component for state "
                        "comp_hover defined.\n";
                setChildImage(comp_hover(), reader);
            } else if(element == "image-checked") {
                if(comp_checked().getComponent() != 0)
                    std::cerr << "Warning: more than 1 component for state "
                        "comp_hover defined.\n";
                setChildImage(comp_checked(), reader);
            } else if(element == "text-hover") {
                if(comp_hover().getComponent() != 0)
                    std::cerr << "Warning: more than 1 component for state "
                        "comp_hover defined.\n";
                setChildText(comp_hover(), reader);
            } else if(element == "image-clicked") {
                if(comp_clicked().getComponent() != 0)
                    std::cerr << "Warning: more than 1 component for state "
                        "comp_clicked defined.\n";
                setChildImage(comp_clicked(), reader);
            } else if(element == "text-clicked") {
                if(comp_clicked().getComponent() != 0)
                    std::cerr << "Warning: more than 1 component for state "
                        "comp_clicked defined.\n";
                setChildText(comp_clicked(), reader);
            } else if(element == "image-caption") {
                if(comp_caption().getComponent() != 0)
                    std::cerr << "Warning: more than 1 component for comp_caption "
                        "defined.\n";
                setChildImage(comp_caption(), reader);
            } else if(element == "image-disabled") {
                if(comp_disabled().getComponent() != 0)
                    std::cerr << "Warning: More than 1 component for "
                        "comp_disabled defined.\n";
                setChildImage(comp_disabled(), reader);
            } else if(element == "text-caption") {
                if(comp_caption().getComponent() != 0)
                    std::cerr << "Warning: more than 1 component for comp_caption "
                        "defined.\n";
                setChildText(comp_caption(), reader);
            } else if (element == "tooltip") {
                parseTooltip = true;
            } else {
                std::cerr << "Skipping unknown element '" << element << "'.\n";
            }
        } else if(reader.getNodeType() == XML_READER_TYPE_END_ELEMENT) {
             std::string element = (const char*) reader.getName();
             if(element == "tooltip")
                 parseTooltip = false;
        } else if(reader.getNodeType() == XML_READER_TYPE_TEXT) {
            if(!parseTooltip)
                continue;

            const char* p = GUI_TRANSLATE((const char*) reader.getValue());

            // skip trailing spaces
            while(*p != 0 && isspace(static_cast<unsigned char>(*p)))
                ++p;

            bool lastspace = tooltip != "";
            for( ; *p != 0; ++p) {
                if(isspace(static_cast<unsigned char>(*p))) {
                    if(!lastspace) {
                        lastspace = true;
                        tooltip += ' ';
                    }
                } else {
                    lastspace = false;
                    tooltip += *p;
                }
            }
        }
    }

    if(comp_normal().getComponent() == 0)
        throw std::runtime_error("No component for state comp_normal defined.");

    // if no width/height was specified we use the one from the biggest image
    if(width <= 0 || height <= 0) {
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
    //std::cout << " done" << std::endl;
}

void
CheckButton::setChildImage(Child& child, XmlReader& reader)
{
    std::unique_ptr<Image> image(new Image());
    image->parse(reader);
    resetChild(child, image.release());
}

void
CheckButton::setChildText(Child& child, XmlReader& reader)
{
    std::unique_ptr<Paragraph> paragraph(new Paragraph());
    paragraph->parse(reader);
    resetChild(child, paragraph.release());
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
    if(mhovered = event.inside) {
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
