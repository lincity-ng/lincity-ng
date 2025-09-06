/*
Copyright (C) 2005 Matthias Braun <matze@braunis.de>
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

/**
 * @file Window.cpp
 * @author Matthias Braun
 */

#include "Window.hpp"

#include <assert.h>                       // for assert
#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml++/ustring.h>             // for ustring
#include <functional>                     // for bind, _1, function
#include <memory>                         // for allocator, unique_ptr
#include <stdexcept>                      // for runtime_error
#include <string>                         // for operator==, basic_string
#include <utility>                        // for move

#include "Button.hpp"                     // for Button
#include "Color.hpp"                      // for Color
#include "ComponentFactory.hpp"           // for IMPLEMENT_COMPONENT_FACTORY
#include "ComponentLoader.hpp"            // for parseEmbeddedComponent
#include "Painter.hpp"                    // for Painter
#include "Rect2D.hpp"                     // for Rect2D
#include "Signal.hpp"                     // for Signal
#include "Vector2.hpp"                    // for Vector2
#include "WindowManager.hpp"              // for WindowManager
#include "util/xmlutil.hpp"               // for xmlParse, unexpectedXmlAttr...

using namespace std::placeholders;

Window::Window()
    : border(1), titlesize(0)
{
    setFlags(FLAG_RESIZABLE);
}

Window::~Window()
{
}

void
Window::parse(xmlpp::TextReader& reader) {
  while(reader.move_to_next_attribute()) {
    xmlpp::ustring name = reader.get_name();
    xmlpp::ustring value = reader.get_value();
    if(parseAttribute(reader));
    else if(name == "width")
      width = xmlParse<float>(value);
    else if(name == "height")
      height = xmlParse<float>(value);
    else if(name == "border")
      border = xmlParse<float>(value);
    else if(name == "titlesize")
      titlesize = xmlParse<float>(value);
    else if(name == "resizable")
      (this->*(xmlParse<bool>(value) ? &Window::setFlags : &Window::clearFlags))(FLAG_RESIZABLE);
    else
      unexpectedXmlAttribute(reader);
  }
  reader.move_to_element();

  if(width <= 0 || height <= 0)
    throw std::runtime_error("invalid width / height");

  childs.assign(5, Child());

  if(!reader.is_empty_element() && reader.read())
  while(reader.get_node_type() != xmlpp::TextReader::NodeType::EndElement) {
    if(reader.get_node_type() != xmlpp::TextReader::NodeType::Element) {
      reader.next();
      continue;
    }
    xmlpp::ustring element = reader.get_name();
    if(element == "title") {
      resetChild(title(), parseEmbeddedComponent(reader));
    } else if(element == "closebutton") {
      std::unique_ptr<Button> button (new Button());
      button->parse(reader);
      resetChild(closeButton(), std::move(button));
    } else if(element == "contents") {
      resetChild(contents(), parseEmbeddedComponent(reader));
    } else if(element == "background") {
      resetChild(background(), parseEmbeddedComponent(reader));
    } else if(element == "title-background") {
      resetChild(title_background(), parseEmbeddedComponent(reader));
    } else {
      unexpectedXmlElement(reader);
    }
    reader.next();
  }

  Component::resize(getSize());

  // connect signals...
  if(closeButton().getComponent()) {
    Button* button = (Button*) closeButton().getComponent();
    button->clicked.connect(std::bind(&Window::closeButtonClicked, this, _1));
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
