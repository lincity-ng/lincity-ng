#ifndef __DESKTOP_HPP__
#define __DESKTOP_HPP__

#include <vector>
#include "Component.hpp"
#include "ComponentHolder.hpp"

class XmlReader;

class Desktop : public Component
{
public:
    Desktop(Component* parent, XmlReader& reader);
    virtual ~Desktop();

    virtual void resize(float width, float height);
    virtual void draw(Painter& painter);
    virtual void event(Event& event);

    Vector2 getPos(Component* component);
    void move(Component* component, Vector2 pos);
    void remove(Component* component);
    
private:
    void removeComponents();
    void internal_remove(Component* component);
    
    typedef std::vector<ComponentHolder<Component>* > Components;
    Components components;

    std::vector<Component*> removeQueue;
};

#endif

