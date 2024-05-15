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
#include <iostream>               // for char_traits, basic_ostream, operator<<
#include <memory>                 // for allocator, unique_ptr
#include <sstream>                // for basic_stringstream
#include <stdexcept>              // for runtime_error
#include <string>                 // for basic_string, operator==, operator<<

#include "Button.hpp"             // for Button
#include "Color.hpp"              // for Color
#include "ComponentFactory.hpp"   // for IMPLEMENT_COMPONENT_FACTORY
#include "ComponentLoader.hpp"    // for parseEmbeddedComponent
#include "Desktop.hpp"            // for Desktop
#include "Event.hpp"              // for Event
#include "Painter.hpp"            // for Painter
#include "Rect2D.hpp"             // for Rect2D
#include "XmlReader.hpp"          // for XmlReader
#include "callback/Callback.hpp"  // for makeCallback, Callback
#include "callback/Signal.hpp"    // for Signal

Window::Window()
    : border(1), titlesize(0), dragging(false), resizeEdge(0)
{
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

    float compWidth = width - 2*border;
    float compHeight = height - 2*border;
    title().setPos(Vector2(border, border));
    title().getComponent()->resize(compWidth - closeButtonWidth, titlesize);
    if(title_background().getComponent() != 0) {
        title_background().setPos(title().getPos());
        title_background().getComponent()->resize(
                compWidth - closeButtonWidth, titlesize);
    }
    if(closeButton().getComponent() != 0) {
        closeButton().setPos(Vector2(
                    border + compWidth - closeButtonWidth + closeButtonBorder,
                    border + closeButtonBorder));
    }
    contents().setPos(Vector2(border, border + titlesize));
    contents().getComponent()->resize(compWidth, compHeight - titlesize);
    if(background().getComponent() != 0) {
        background().setPos(Vector2(0, 0));
        background().getComponent()->resize(width, height);
    }

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
    switch(event.type) {
        case Event::MOUSEBUTTONDOWN:
            if(event.inside && event.button == SDL_BUTTON_LEFT
                && title().inside(event.mousepos)
            ) {
                dragging = true;
                dragOffset = desktop->getPos(this) - event.mousepos;
                desktop->setSystemCursor(this, SDL_SYSTEM_CURSOR_SIZEALL);
            }
            else if(resizeEdge && event.button == SDL_BUTTON_LEFT) {
                dragOffset = Vector2(getWidth(), getHeight());
                if(resizeEdge & Window::Edge::N)
                    dragOffset.x = -dragOffset.x;
                if(resizeEdge & Window::Edge::W)
                    dragOffset.y = -dragOffset.y;
                dragOffset -= event.mousepos;
            }
            else if(resizeEdge) {
                // cancel window resize because a wrong mouse button is pressed
                resizeEdge = Window::Edge::NONE;
                desktop->tryClearCursor(this);
            }
            break;

        case Event::MOUSEBUTTONUP:
            if(dragging) {
                dragging = false;
                desktop->tryClearCursor(this);
            }
            break;

        case Event::MOUSEMOTION: {
            if(dragging) {
                assert(desktop != 0);
                if(!desktop)
                    return;

                // try to move window...
                desktop->move(this, event.mousepos + dragOffset);
            }
            else if((event.inside || resizeEdge) && !event.mousebuttonstate
                && resizeEdge != (resizeEdge = edgeAt(event.mousepos))
            ) {
                SDL_SystemCursor cursorId = SDL_SYSTEM_CURSOR_SIZENWSE;
                switch(resizeEdge) {
                case Window::Edge::N:
                case Window::Edge::S:
                    cursorId++;
                case Window::Edge::E:
                case Window::Edge::W:
                    cursorId++;
                case Window::Edge::NE:
                case Window::Edge::SW:
                    cursorId++;
                case Window::Edge::NW:
                case Window::Edge::SE:
                    desktop->setSystemCursor(this, cursorId);
                    break;
                default:
                    assert(false);
                case Window::Edge::NONE:
                    desktop->tryClearCursor(this);
                }
            }
            else if(resizeEdge && event.mousebuttonstate) {
                assert(event.mousebuttonstate == SDL_BUTTON_LMASK);

                // if move, then
                //   new size = -(mousepos + offset)
                //   new size > old size => move neg (expand)
                //   new size < old size => move pos (shrink)
                // if no move, then do the resize
                //   new size = mousepos + offset
                // if move neg (expand), then move then resize
                //   new pos  = old pos + old size + mousepos + offset
                //   new size = old pos + old size - new pos
                // if move pos (shrink), then resize then move
                //   new size = -(mousepos + offset)
                //   new pos  = old pos + old size - new size

                #define DIM(P, X, Y) (ISX?(P##X):(P##Y))
                #define XY(V) DIM(V.,x,y)
                #define DIMDUP(CODE) {bool ISX=1;CODE ISX=0;CODE}

                Vector2 oldSize(getWidth(), getHeight());
                Vector2 newSize = oldSize;
                Vector2 minSize = getMinSize();
                Vector2 oldPos = desktop->getPos(this);
                Vector2 newPos = oldPos;
                Vector2 oldSE = oldPos + oldSize;

                DIMDUP({
                  if(resizeEdge & DIM(Window::Edge::EW,NS)) {
                    XY(newSize) = XY(event.mousepos) + XY(dragOffset);
                    if(resizeEdge & DIM(Window::Edge::,W,N))
                      XY(newSize) = -XY(newSize);
                    if(XY(newSize) < XY(minSize))
                      XY(newSize) = XY(minSize);
                    if(XY(newsize) < XY(oldSize))
                      DIM(,width,height) = XY(newSize);
                    if(resizeEdge & DIM(Window::Edge::,W,N))
                      XY(newPos) = XY(oldSE) - XY(newSize);
                  }
                })

                if(resizeEdge & Window::Edge::NW) {
                  desktop->move(this, newPos);

                  DIMDUP({
                    if(resizeEdge & DIM(Window::Edge::,W,N))
                      XY(newSize) = XY(oldSE) - XY(newPos);
                  })
                }

                desktop->resize(this, newSize);
            }
            break;
        }

        default:
            break;
    }

    // distribute event to child components...
    Component::event(event);
}

void
Window::closeButtonClicked(Button* )
{
    Desktop* desktop = dynamic_cast<Desktop*> (getParent());
    assert(desktop != 0);
    if(!desktop)
        return;

    desktop->remove(this);
}

Window::Edge
Window::edgeAt(const Vector2 &pos) {
  Edge edge = Edge::NONE;
  if(pos.y >= -1 && pos.y < 1)
    edge |= Edge::N;
  else if(height - 1 - pos.y >= -1 && height - 1 - pos.y < 1)
    edge |= Edge::S;
  if(pos.x >= -1 && pos.x < 1)
    edge |= Edge::W;
  else if(width - 1 - pos.x >= -1 && width - 1 - pos.x < 1)
    edge |= Edge::E;
  return edge;
}

void
Window::setResizeCursor() {
    SDL_SystemCursor cursorId = SDL_SYSTEM_CURSOR_SIZENWSE;
    switch(resizeEdge) {
    case Window::Edge::N:
    case Window::Edge::S:
        cursorId++;
    case Window::Edge::E:
    case Window::Edge::W:
        cursorId++;
    case Window::Edge::NE:
    case Window::Edge::SW:
        cursorId++;
    case Window::Edge::NW:
    case Window::Edge::SE:
        desktop->setSystemCursor(this, cursorId);
        break;
    default:
        assert(false);
    case Window::Edge::NONE:
        desktop->tryClearCursor(this);
    }
}


IMPLEMENT_COMPONENT_FACTORY(Window)

/** @file gui/Window.cpp */
