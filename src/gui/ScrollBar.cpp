#include "ScrollBar.hpp"

#include <sstream>
#include <stdexcept>
#include <iostream>

#include "ComponentFactory.hpp"
#include "XmlReader.hpp"
#include "Button.hpp"
#include "Painter.hpp"

ScrollBar::ScrollBar(Component* parent, XmlReader& reader)
    : Component(parent), minVal(0), maxVal(0), currentVal(0), scrolling(false)
{
    width = 30; // default width...
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();
        
        if(parseAttributeValue(attribute, value)) {
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

    int depth = reader.getDepth();
    while(reader.read() && reader.getDepth() > depth) {
        if(reader.getNodeType() == XML_READER_TYPE_ELEMENT) {
            std::string element = (const char*) reader.getName();
            if(element == "button1") {
                button1.setComponent(new Button(this, reader));
            } else if(element == "button2") {
                button2.setComponent(new Button(this, reader));
            } else if(element == "scroller") {
                scroller.setComponent(new Button(this, reader));
            } else {
                std::cerr << "Skipping unknown element '"
                    << element << "'.\n";
                reader.nextNode();
            }
        }
    }

    setFlags(getFlags() | FLAG_RESIZABLE);
}

ScrollBar::~ScrollBar()
{
}

void
ScrollBar::resize(float newwidth, float newheight)
{
    // ensure a minimum height...
    if(newheight < width*2 + scroller.getComponent()->getHeight())
        newheight = width*2 + scroller.getComponent()->getHeight();
    
    this->height = newheight;

    button1.setPos(Vector2(0, 0));
    button2.setPos(Vector2(0, height - button2.getComponent()->getHeight()));
    // TODO correctly set scroller pos...
    scroller.setPos(Vector2(0, button1.getComponent()->getHeight()));
}

void
ScrollBar::setScrollPos()
{
}

void
ScrollBar::draw(Painter& painter)
{
    // TODO draw rectangle around scrollbar...
    button1.draw(painter);
    button2.draw(painter);
    scroller.draw(painter);
}

void
ScrollBar::event(Event& event)
{
    switch(event.type) {
        case Event::MOUSEBUTTONDOWN:
            if(scroller.inside(event.mousepos)) {
                scrolling = true;
                scrollOffset = event.mousepos.y - scroller.getPos().y;
            }
            break;
            
        case Event::MOUSEBUTTONUP:
            scrolling = false;
            break;

        case Event::MOUSEMOTION: {
            if(!scrolling)
                break;
            
            float val = event.mousepos.y - scrollOffset;
            if(val < button1.getComponent()->getHeight())
                val = button1.getComponent()->getHeight();
            if(val > button2.getPos().y - scroller.getComponent()->getHeight())
                val = button2.getPos().y - scroller.getComponent()->getHeight();
            scroller.setPos(Vector2(0, val));

            // map val to scrollrange...
            float scrollScreenRange = height 
                - button1.getComponent()->getHeight()
                - button2.getComponent()->getHeight()
                - scroller.getComponent()->getHeight();
            float scrollScreenRatio 
                = (val - button1.getComponent()->getHeight()) /
                        scrollScreenRange;
            float newScrollVal = minVal + 
                ((maxVal - minVal) * scrollScreenRatio);
            assert(newScrollVal >= minVal && newScrollVal <= maxVal);
            signalValueChanged(this, newScrollVal);
            break;
        }
        default:
            break;
    }
    
    button1.event(event);
    button2.event(event);
    scroller.event(event);
}

void
ScrollBar::setRange(float min, float max)
{
    if(max < min) {
        throw std::runtime_error("Invalid scroll range specified (max < min)");
    }
    minVal = min;
    maxVal = max;
    printf("NewRange: %f %f.\n", min, max);
}

IMPLEMENT_COMPONENT_FACTORY(ScrollBar);
