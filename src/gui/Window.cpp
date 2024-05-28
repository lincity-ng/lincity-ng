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
 * @file Window.cpp
 * @author Matthias Braun
 */

#include "Window.hpp"

#include <assert.h>               // for assert
#include <libxml/xmlreader.h>     // for XML_READER_TYPE_ELEMENT
#include <stdio.h>                // for sscanf
#include <string.h>               // for strcmp
#include <iostream>               // for basic_ostream, operator<<, stringst...
#include <memory>                 // for unique_ptr
#include <sstream>                // for basic_stringstream
#include <stdexcept>              // for runtime_error
#include <string>                 // for char_traits, basic_string, operator==

#include "Button.hpp"             // for Button
#include "Color.hpp"              // for Color
#include "ComponentFactory.hpp"   // for IMPLEMENT_COMPONENT_FACTORY
#include "ComponentLoader.hpp"    // for parseEmbeddedComponent
#include "Painter.hpp"            // for Painter
#include "Rect2D.hpp"             // for Rect2D
#include "Vector2.hpp"            // for Vector2
#include "WindowManager.hpp"      // for WindowManager
#include "XmlReader.hpp"          // for XmlReader
#include "callback/Callback.hpp"  // for makeCallback, Callback
#include "callback/Signal.hpp"    // for Signal

Window::Window()
    : border(1), titlesize(0)
{
    setFlags(FLAG_RESIZABLE);
}

Window::~Window()
{
}

/**
 * Parse a given XmlReader objest that represents the map.
 *
 * @param reader Reference to a XmlReader object representing a given XML file.
 */
void
Window::parse(XmlReader& reader)
{
    // parse attributes...
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* name = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttribute(name, value)) {
            continue;
        } else if(strcmp(name, "width") == 0) {
            if(sscanf(value, "%f", &width) != 1) {
                std::stringstream msg;
                msg << "Couldn't parse width attribute (" << value << ").";
                throw std::runtime_error(msg.str());
            }
        } else if(strcmp(name, "height") == 0) {
            if(sscanf(value, "%f", &height) != 1) {
                std::stringstream msg;
                msg << "Couldn't parse height attribute (" << value << ").";
                throw std::runtime_error(msg.str());
            }
        } else if(strcmp(name, "border") == 0) {
            if(sscanf(value, "%f", &border) != 1) {
                std::stringstream msg;
                msg << "Couldn't parse border attribute (" << value << ").";
                throw std::runtime_error(msg.str());
            }
        } else if(strcmp(name, "titlesize") == 0) {
            if(sscanf(value, "%f", &titlesize) != 1) {
                std::stringstream msg;
                msg << "Couldn't parse titlesize attribute (" << value << ").";
                throw std::runtime_error(msg.str());
            }
        } else {
            std::cerr << "Unknown attribute '" << name << "' skipped.\n";
        }
    }
    if(width <= 0 || height <= 0)
        throw std::runtime_error("Width or Height invalid");

    childs.assign(5, Child());

    int depth = reader.getDepth();
    while(reader.read() && reader.getDepth() > depth) {
        if(reader.getNodeType() == XML_READER_TYPE_ELEMENT) {
            std::string element = (const char*) reader.getName();
            if(element == "title") {
                resetChild(title(), parseEmbeddedComponent(reader));
            } else if(element == "closebutton") {
                std::unique_ptr<Button> button (new Button());
                button->parse(reader);
                resetChild(closeButton(), button.release());
            } else if(element == "contents") {
                resetChild(contents(), parseEmbeddedComponent(reader));
            } else if(element == "background") {
                resetChild(background(), parseEmbeddedComponent(reader));
            } else if(element == "title-background") {
                resetChild(title_background(), parseEmbeddedComponent(reader));
            } else {
                std::cerr << "Skipping unknown element '"
                    << element << "'.\n";
                reader.nextNode();
            }
        }
    }

    reLayout();

    // connect signals...
    if(closeButton().getComponent() != 0) {
        Button* button = (Button*) closeButton().getComponent();
        button->clicked.connect(
                makeCallback(*this, &Window::closeButtonClicked));
    }
}

/**
 * Draw the map.
 *
 * @param painter Reference to a Painter object.
 */
void
Window::draw(Painter& painter)
{
    Component::draw(painter);

    painter.setLineColor(Color(0, 0, 0, 0xff));
    painter.drawRectangle(Rect2D(0, 0, width, height));
}

void
Window::event(const Event& event)
{
    // distribute event to child components...
    Component::event(event);
}

void
Window::resize(float width, float height) {
  size.x = width;
  size.y = height;

  retry:

  if(size.x < 10) size.x = 10;
  if(size.y < 10) size.y = 10;

  // layout
  float closeButtonHeight = 0;
  float closeButtonWidth = 0;
  float closeButtonBorder = 0;
  if(closeButton().getComponent() != 0) {
      closeButtonHeight = closeButton().getComponent()->getHeight();
      if(titlesize < closeButtonHeight) {
          titlesize = closeButtonHeight;
      }

      closeButtonWidth = closeButton().getComponent()->getWidth();
      closeButtonBorder = (titlesize - closeButtonHeight) / 2;
      closeButtonWidth += 2*closeButtonBorder;
      closeButtonHeight += 2*closeButtonBorder;
  }
  Vector2 compSize = size - Vector2(border,border)*2;

  Vector2 targetSize;
  Vector2 actualSize;
  title().setPos(Vector2(border, border));
  targetSize = Vector2(compSize.x - closeButtonWidth, titlesize);
  title().getComponent()->resize(targetSize);
  actualSize = title().getComponent()->getSize();
  if(actualSize.x != targetSize.x) {
    size.x = actualSize.x + closeButtonWidth + 2*border;
    goto retry;
  }

  if(title_background().getComponent() != 0) {
    title_background().setPos(title().getPos());
    targetSize = Vector2(compSize.x - closeButtonWidth, titlesize);
    title_background().getComponent()->resize(targetSize);
    actualSize = title_background().getComponent()->getSize();
    if(actualSize.x != targetSize.x) {
      size.x = actualSize.x + closeButtonWidth + 2*border;
      goto retry;
    }
  }

  if(closeButton().getComponent() != 0) {
    closeButton().setPos(Vector2(
      border + compSize.x - closeButtonWidth + closeButtonBorder,
      border + closeButtonBorder));
  }

  contents().setPos(Vector2(border, border + titlesize));
  targetSize = actualSize = Vector2(compSize.x, compSize.y - titlesize);
  if(actualSize.x < 1) actualSize.x = 1;
  if(actualSize.y < 1) actualSize.y = 1;
  contents().getComponent()->resize(actualSize);
  actualSize = contents().getComponent()->getSize();
  if(actualSize != targetSize) {
    size += actualSize - targetSize;
    goto retry;
  }
  contents().setClipRect(
    Rect2D(Vector2(),targetSize).move(contents().getPos()));

  if(background().getComponent() != 0) {
    background().setPos(Vector2(0, 0));
    targetSize = size;
    background().getComponent()->resize(targetSize);
    actualSize = background().getComponent()->getSize();
    if(actualSize != targetSize) {
      size = actualSize;
      goto retry;
    }
  }
}

void
Window::closeButtonClicked(Button* )
{
    WindowManager* windowManager = dynamic_cast<WindowManager *>(getParent());
    assert(windowManager != 0);
    if(!windowManager)
        return;

    windowManager->removeWindow(this);
}


IMPLEMENT_COMPONENT_FACTORY(Window)

/** @file gui/Window.cpp */
