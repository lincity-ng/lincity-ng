#include <config.h>

#include "Desktop.hpp"

#include <iostream>
#include <stdexcept>

#include "XmlReader.hpp"
#include "ComponentFactory.hpp"
#include "ComponentLoader.hpp"
#include "Style.hpp"

Desktop::Desktop(Component* parent, XmlReader& reader)
    : Component(parent)
{
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttribute(attribute, value)) {
            continue;
        } else {
            std::cerr << "Skipping unknown attribute '" << attribute << "'.\n";
        }
    }

    int depth = reader.getDepth();
    while(reader.read() && reader.getDepth() > depth) {
        if(reader.getNodeType() == XML_READER_TYPE_ELEMENT) {
            std::string element = (const char*) reader.getName();

            if(element == "DefineStyle") {
                parseStyleDef(reader);
            } else {
                Component* component = createComponent(element, this, reader);
                addChild(component);
            }
        }
    }

    setFlags(FLAG_RESIZABLE);
}

Desktop::~Desktop()
{
}

void
Desktop::event(const Event& event)
{
    Component::event(event);

    // process pending remove events...
    for(std::vector<Component*>::iterator i = removeQueue.begin();
            i != removeQueue.end(); ++i) {
        internal_remove(*i);
    }
    removeQueue.clear();
}

void
Desktop::resize(float width, float height)
{
    for(Childs::iterator i = childs.begin(); i != childs.end(); ++i) {
        Component* component = i->getComponent();
        if(component->getFlags() & FLAG_RESIZABLE)
            component->resize(width, height);
    }
    this->width = width;
    this->height = height;
}

Vector2
Desktop::getPos(Component* component)
{
    // find child
    Child* child = 0;
    for(Childs::iterator i = childs.begin(); i != childs.end(); ++i) {
        if(i->getComponent() == component) {
            child = &(*i);
            break;
        }
    }
    if(child == 0)
        throw std::runtime_error(
                "Trying to getPos a component that is not a direct child");

    return child->position;
}

void
Desktop::move(Component* component, Vector2 newpos)
{
    if(component->getFlags() & FLAG_RESIZABLE)
        throw std::runtime_error("Can't move resizable components around");

    // find child
    Child* child = 0;
    for(Childs::iterator i = childs.begin(); i != childs.end(); ++i) {
        if(i->getComponent() == component) {
            child = &(*i);
            break;
        }
    }
    if(child == 0)
        throw std::runtime_error(
                "Trying to getPos a component that is not a direct child");
    
    // keep component in bounds...
    if(newpos.x + component->getWidth() > width)
        newpos.x = width - component->getWidth();
    if(newpos.y + component->getHeight() > height)
        newpos.y = height - component->getHeight();
    if(newpos.x < 0)
        newpos.x = 0;
    if(newpos.y < 0)
        newpos.y = 0;

    child->position = newpos;
}

void
Desktop::remove(Component* component)
{
    removeQueue.push_back(component);
}

void
Desktop::internal_remove(Component* component)
{
    // find child
    for(Childs::iterator i = childs.begin(); i != childs.end(); ++i) {
        if(i->getComponent() == component) {
            childs.erase(i);
            return;
        }
    }
    throw std::runtime_error(
            "Trying to remove a component that is not a direct child");
}
