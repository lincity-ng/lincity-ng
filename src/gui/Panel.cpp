#include <config.h>

#include "Panel.hpp"

#include <sstream>
#include <stdexcept>
#include <iostream>

#include "XmlReader.hpp"
#include "TextureManager.hpp"
#include "ComponentFactory.hpp"
#include "ComponentLoader.hpp"
#include "Painter.hpp"

Panel::Panel()
{
}

Panel::~Panel()
{
}

void
Panel::parse(XmlReader& reader)
{
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttribute(attribute, value)) {
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

    Component* component = parseEmbeddedComponent(reader);
    addChild(component);
    if(component->getFlags() & FLAG_RESIZABLE) {
        component->resize(width, height);
    }
}

void
Panel::draw(Painter& painter)
{
    if(background.get())
      painter.drawTexture(background.get(), Vector2(0, 0));
    Component::draw(painter);
}

bool
Panel::opaque(const Vector2& pos) const
{
    for(Childs::const_iterator i = childs.begin(); i != childs.end(); ++i) {
        const Child& child = *i;
        if(child.getComponent() == 0 || !child.isEnabled())
            continue;

        if(child.getComponent()->opaque(pos - child.getPos()))
            return true;
    }
    
    return false;
}

IMPLEMENT_COMPONENT_FACTORY(Panel)
