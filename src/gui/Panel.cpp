#include "Panel.hpp"

#include <sstream>
#include <stdexcept>
#include <iostream>

#include "XmlReader.hpp"
#include "TextureManager.hpp"
#include "ComponentFactory.hpp"
#include "ComponentLoader.hpp"
#include "Painter.hpp"

Panel::Panel(Component* parent, XmlReader& reader)
    : Component(parent)
{
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttributeValue(attribute, value)) {
            continue;
        } else if(strcmp(attribute, "background") == 0) {
            background.reset(texture_manager->load(value));
        } else if(strcmp(attribute, "width") == 0) {
            if(sscanf(value, "%f", &width) != 1) {
                std::stringstream msg;
                msg << "Parse error when parsing width (" << value << ")";
                throw std::runtime_error(msg.str());
           }
        } else if(strcmp(attribute, "height") == 0) {
            if(sscanf(value, "%f", &height) != 1) {
                std::stringstream msg;
                msg << "Parse error when parsing height (" << value << ")";
                throw std::runtime_error(msg.str());
            }
        } else {
            std::cerr << "Skipping unknown attribute '" << attribute << "'.\n";
        }
    }

    if(width <= 0 || height <= 0) {
        throw std::runtime_error("invalid width/height");
    }

    Component* component = parseEmbeddedComponent(this, reader);
    addChild(component);
    if(component->getFlags() & FLAG_RESIZABLE) {
        component->resize(width, height);
    }
}

Panel::~Panel()
{
}

void
Panel::draw(Painter& painter)
{
    painter.drawTexture(background.get(), Rectangle(0, 0, width, height));
    Component::draw(painter);
}

IMPLEMENT_COMPONENT_FACTORY(Panel)
