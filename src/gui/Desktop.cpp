#include "Desktop.hpp"

#include <iostream>
#include <stdexcept>

#include "XmlReader.hpp"
#include "ComponentFactory.hpp"
#include "ComponentLoader.hpp"

Desktop::Desktop(Component* parent, XmlReader& reader)
    : Component(parent)
{
    XmlReader::AttributeIterator iter(reader);
    while(iter.next()) {
        const char* attribute = (const char*) iter.getName();
        const char* value = (const char*) iter.getValue();

        if(parseAttributeValue(attribute, value)) {
            continue;
        } else {
            std::cerr << "Skipping unknown attribute '" << attribute << "'.\n";
        }
    }

    try {
        int depth = reader.getDepth();
        while(reader.read() && reader.getDepth() > depth) {
            if(reader.getNodeType() == XML_READER_TYPE_ELEMENT) {
                std::string element = (const char*) reader.getName();

                Component* component = createComponent(element, this, reader);
                components.push_back(new ComponentHolder<Component> (component));
            }
        }
    } catch(...) {
        removeComponents();
        throw;
    }
}

Desktop::~Desktop()
{
    removeComponents();
}

void
Desktop::removeComponents()
{
    for(Components::iterator i = components.begin(); i != components.end(); ++i)
        delete *i;
    components.clear();
}

void
Desktop::draw(Painter& painter)
{
    for(Components::iterator i = components.begin();
            i != components.end(); ++i) {
        ComponentHolder<Component>* holder = *i;
        holder->draw(painter);
    }
}

void
Desktop::event(Event& event)
{
    for(Components::iterator i = components.begin();
            i != components.end(); ++i) {
        ComponentHolder<Component>* holder = *i;
        holder->event(event);
    }

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
    for(Components::iterator i = components.begin();
            i != components.end(); ++i) {
        ComponentHolder<Component>* holder = *i;
        if(holder->getComponent()->getFlags() & FLAG_RESIZABLE)
            holder->resize(width, height);      
    }
    this->width = width;
    this->height = height;
}

Vector2
Desktop::getPos(Component* component)
{
    // find componentholder...
    ComponentHolder<Component>* holder = 0;
    for(Components::iterator i = components.begin();
            i != components.end(); ++i) {
        if((*i)->getComponent() == component) {
            holder = *i;
            break;
        }
    }
    if(holder == 0)
        throw std::runtime_error(
                "Trying to getPos a component that is not a direct child");

    return holder->getPos();
}

void
Desktop::move(Component* component, Vector2 newpos)
{
    if(component->getFlags() & FLAG_RESIZABLE)
        throw std::runtime_error("Can't move resizable components around");

    // find componentholder...
    ComponentHolder<Component>* holder = 0;
    for(Components::iterator i = components.begin();
            i != components.end(); ++i) {
        if((*i)->getComponent() == component) {
            holder = *i;
            break;
        }
    }
    if(holder == 0)
        throw std::runtime_error(
                "Trying to move a component that is not a direct child");

    // keep component in bounds...
    if(newpos.x + component->getWidth() > width)
        newpos.x = width - component->getWidth();
    if(newpos.y + component->getHeight() > height)
        newpos.y = height - component->getHeight();
    if(newpos.x < 0)
        newpos.x = 0;
    if(newpos.y < 0)
        newpos.y = 0;

    holder->setPos(newpos);
}

void
Desktop::remove(Component* component)
{
    removeQueue.push_back(component);
}

void
Desktop::internal_remove(Component* component)
{
    // find componentholder...
    for(Components::iterator i = components.begin();
            i != components.end(); ++i) {
        if((*i)->getComponent() == component) {
            components.erase(i);
            delete component;
            return;
        }
    }
    throw std::runtime_error(
            "Trying to remove a component that is not a direct child");
}
