#include "Window.hpp"

#include <sstream>
#include <stdexcept>
#include <iostream>
#include <assert.h>

#include "ComponentFactory.hpp"
#include "ComponentLoader.hpp"
#include "XmlReader.hpp"
#include "Painter.hpp"
#include "Button.hpp"
#include "Event.hpp"
#include "Desktop.hpp"

Window::Window(Component* parent, XmlReader& reader)
    : Component(parent), border(1), titlesize(0), dragging(false)
{
    Desktop* desktop = dynamic_cast<Desktop*> (parent);
    if(desktop == 0) {
        throw std::runtime_error(
                "Windows have to be childs of a Desktop component.");
    }

    // parse attributes...
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* name = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(strcmp(name, "width") == 0) {
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

    childs.assign(3, Child());
    
    int depth = reader.getDepth();
    while(reader.read() && reader.getDepth() > depth) {
        if(reader.getNodeType() == XML_READER_TYPE_ELEMENT) {
            std::string element = (const char*) reader.getName();
            if(element == "title") {
                title().setComponent(parseEmbeddedComponent(this, reader));
            } else if(element == "closebutton") {
                closeButton().setComponent(new Button(this, reader));
            } else if(element == "contents") {
                contents().setComponent(parseEmbeddedComponent(this, reader));
            } else {
                std::cerr << "Skipping unknown element '"
                    << element << "'.\n";
                reader.nextNode();
            }
        }
    }

    if(closeButton().getComponent() == 0)
        throw std::runtime_error("No closeButton specified.");

    // layout
    float closeButtonHeight = closeButton().getComponent()->getHeight();
    if(titlesize < closeButtonHeight) {
        titlesize = closeButtonHeight;
    }

    float closeButtonWidth = closeButton().getComponent()->getWidth();
    float closeButtonBorder = (titlesize - closeButtonHeight) / 2;
    closeButtonWidth += 2*closeButtonBorder;
    closeButtonHeight += 2*closeButtonBorder;

    float compWidth = width - 2*border;
    float compHeight = height - 2*border;
    title().setPos(Vector2(border, border));
    title().getComponent()->resize(compWidth - closeButtonWidth, titlesize);
    closeButton().setPos(Vector2(
          border + compWidth - closeButtonWidth + closeButtonBorder,
          border + closeButtonBorder));
    contents().setPos(Vector2(border, border + titlesize));
    contents().getComponent()->resize(compWidth, compHeight - titlesize);

    // connect signals...
    Button* button = (Button*) closeButton().getComponent();
    button->signalClicked.connect(
        sigc::mem_fun(*this, &Window::closeButtonClicked));
}

Window::~Window()
{
}

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
            if(title().inside(event.mousepos)) {
                dragging = true;
                dragOffset = event.mousepos - title().getPos();
            }
            break;
            
        case Event::MOUSEBUTTONUP:
            dragging = false;
            break;

        case Event::MOUSEMOTION: {
            if(!dragging)
                break;

            Desktop* desktop = dynamic_cast<Desktop*> (getParent());
            assert(desktop != 0);                                               
            if(!desktop)
                return;

            // try to move window...
            Vector2 newpos = desktop->getPos(this) + 
                event.mousepos - dragOffset;
            desktop->move(this, newpos);
            
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

IMPLEMENT_COMPONENT_FACTORY(Window)
