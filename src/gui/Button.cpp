#include <config.h>

#include <stdexcept>

#include "Painter.hpp"
#include "Event.hpp"
#include "TextureManager.hpp"
#include "Button.hpp"
#include "ComponentFactory.hpp"
#include "XmlReader.hpp"

Button::Button(Component* parent, XmlReader& reader)
    : Component(parent), state(STATE_NORMAL), normal(0), hover(0), clicked(0)
{
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttributeValue(attribute, value)) {
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
        } else {
            std::cerr << "Skipping unknown attribute '"
                << attribute << "'.\n";
        }
    }
        
    int depth = reader.getDepth();
    while(reader.read() && reader.getDepth() > depth) {
        if(reader.getNodeType() == XML_READER_TYPE_ELEMENT) {
            std::string element = (const char*) reader.getName();
            if(element == "image") {
                normal.reset(
                    texture_manager->load(reader.getAttribute("src")));
            } else if(element == "image-hover") {
                hover.reset(
                    texture_manager->load(reader.getAttribute("src")));
            } else if(element == "image-clicked") {
                clicked.reset(
                    texture_manager->load(reader.getAttribute("src")));
            }
        }
    }
   
    if(width <= 0 || height <= 0) {
        width = normal->getWidth();
        height = normal->getHeight();
    }
}

Button::~Button()
{
}

void
Button::draw(Painter& painter)
{
    const Texture* current;
    switch(state) {
        case STATE_NORMAL:
            current = normal.get();
            break;
        case STATE_HOVER:
            if(hover.get() != 0)
                current = hover.get();
            else
                current = normal.get();
            break;
        case STATE_CLICKED:
            if(clicked.get() != 0)
                current = clicked.get();
            else if(hover.get() != 0)
                current = hover.get();
            else
                current = normal.get();
            break;
        default:
            current = 0;
            assert(false);
            break;
    }
    painter.drawTexture(current, Rectangle(0, 0, width, height));
}

void
Button::event(Event& event)
{
    switch(event.type) {
        case Event::MOUSEMOTION:
            if(inside(event.mousepos)) {
                if(state == STATE_NORMAL)
                    state = STATE_HOVER;
            } else {
                if(state == STATE_HOVER)
                    state = STATE_NORMAL;
            }
            break;
        case Event::MOUSEBUTTONDOWN:
            if(inside(event.mousepos))
                state = STATE_CLICKED;
            else
                state = STATE_NORMAL;
            break;
        case Event::MOUSEBUTTONUP:
            if(inside(event.mousepos) && state == STATE_CLICKED) {
                signalClicked(this);
            }
            state = inside(event.mousepos) ? STATE_HOVER : STATE_NORMAL;
            break;
        default:
            break;
    }
}

bool
Button::inside(const Vector2& pos)
{
    if(pos.x < 0 || pos.y < 0 || pos.x >= width || pos.y >= height)
        return false;

    return true;
}

IMPLEMENT_COMPONENT_FACTORY(Button)
