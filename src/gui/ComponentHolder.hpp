#ifndef __COMPONENT_HOLDER_HPP__
#define __COMPONENT_HOLDER_HPP__

#include "Vector2.hpp"
#include "Component.hpp"
#include "Painter.hpp"
#include "Event.hpp"

template<class T>
class ComponentHolder
{
public:
    ComponentHolder(T* _component = 0)
        : component(_component)
    { }
    ~ComponentHolder()
    {
        delete component;
    }

    void setComponent(T* component)
    {
        delete this->component;
        this->component = component;
    }
    T* getComponent()
    {
        return component;
    }
    void setPos(Vector2 pos)
    {
        this->pos = pos;
    }
    const Vector2& getPos() const
    {
        return pos;
    }

    void draw(Painter& painter)
    {
        painter.pushTransform();
        painter.translate(Vector2(pos));
        component->draw(painter);
        painter.popTransform();
    }
    void event(Event& event)
    {
        switch(event.type) {
            case Event::MOUSEMOTION:
            case Event::MOUSEBUTTONDOWN:
            case Event::MOUSEBUTTONUP:
                event.mousepos -= pos;
                component->event(event);
                event.mousepos += pos;
                break;
            default:
                component->event(event);
                break;
        }
    }
    void resize(float width, float height)
    {
        component->resize(width, height);
    }
    bool inside(Vector2 testpos) const
    {
        if(testpos.x >= pos.x && testpos.x <= pos.x + component->getWidth()
                && testpos.y >= pos.y
                && testpos.y <= pos.y + component->getHeight())
            return true;

        return false;
    }

private:
    Vector2 pos;
    T* component;
};

#endif

