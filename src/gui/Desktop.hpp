#ifndef __DESKTOP_HPP__
#define __DESKTOP_HPP__

#include <vector>
#include "Component.hpp"

class XmlReader;

class Desktop : public Component
{
public:
    Desktop(Component* parent, XmlReader& reader);
    virtual ~Desktop();

    void resize(float width, float height);
    void event(const Event& event);
    bool opaque(const Vector2& pos) const;

    Vector2 getPos(Component* component);
    void move(Component* component, Vector2 pos);
    void remove(Component* component);
    
private:
    void internal_remove(Component* component);
    
    std::vector<Component*> removeQueue;
};

#endif

