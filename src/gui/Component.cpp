#include <config.h>

#include <typeinfo>

#include "Component.hpp"
#include "Painter.hpp"
#include "Event.hpp"

Component::Component(Component* _parent)
    : parent(_parent), flags(0), width(0), height(0)
{
}

Component::~Component()
{
}

bool
Component::parseAttributeValue(const char* attribute, const char* value)
{
    if(strcmp(attribute, "name") == 0) {
        name = value;
        return true;
    }

    return false;
}

void
Component::drawChild(Child& child, Painter& painter)
{
    assert(child.getComponent() != 0);
    
    if(child.position != Vector2(0, 0)) {
        painter.pushTransform();
        painter.translate(child.position);
    }
    if(child.useClipRect) {
        painter.setClipRectangle(child.clipRect);
    }
    child.component->draw(painter);
    if(child.useClipRect) {
        painter.clearClipRectangle();
    }
    if(child.position != Vector2(0, 0)) {
        painter.popTransform();
    }
}

void
Component::draw(Painter& painter)
{
    for(Childs::iterator i = childs.begin(); i != childs.end(); ++i) {
        Child& child = *i;
        if(!child.enabled)
            return;
    
        drawChild(child, painter);
    }
}

void
Component::eventChild(Child& child, const Event& event)
{
    assert(child.getComponent() != 0);
    
    Event ev = event;
    if(event.type == Event::MOUSEMOTION 
        || event.type == Event::MOUSEBUTTONDOWN
        || event.type == Event::MOUSEBUTTONUP) {
        // TODO handle clip rectangles?
        ev.mousepos = event.mousepos - child.position;
    }
    child.component->event(ev);
}

void
Component::event(const Event& event)
{
    for(Childs::iterator i = childs.begin(); i != childs.end(); ++i) {
        Child& child = *i;
        if(!child.enabled)
          continue;
    
        eventChild(child, event);
    }
}

Component*
Component::findComponent(const std::string& name)
{
    if(getName() == name)
        return this;

    for(Childs::const_iterator i = childs.begin(); i != childs.end(); ++i) {
        const Child& child = *i;
        Component* component = child.component->findComponent(name);
        if(component)
            return component;
    }

    return 0;
}

Child&
Component::addChild(Component* component)
{
    childs.push_back(Child(component));
    return childs.back();
}

void
Component::resize(float , float )
{
}

