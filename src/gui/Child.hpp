#ifndef __CHILD_HPP__
#define __CHILD_HPP__

#include <vector>
#include "Rectangle.hpp"

class Component;

class Child
{
public:
    Child(Component* _component = 0);
    ~Child();

    void setComponent(Component* newcomponent);

    Component* getComponent() const
    {
        return component;
    }

    void enable(bool enabled)
    {
        if(!component) {
            this->enabled = false;
            return;
        }
        this->enabled = enabled;
    }

    void setPos(const Vector2& position)
    {
        this->position = position;
    }

    const Vector2& getPos() const
    {
        return position;
    }

    bool inside(const Vector2& pos) const;
    
    Vector2 position;
    bool enabled;
    bool useClipRect;
    Rect2D clipRect;
    Component* component;
};

/**
 * I create an own Child vector class here. So that components can get deleted
 * when they are removed from the vector.
 * (This can't be done in the Child destructor, because making Child the owner
 * of a component, makes big trouble when implementing copy constructors)
 */
class Childs : public std::vector<Child>
{
public:
    Childs();
    ~Childs();

    iterator erase(iterator i);
    void clear();
};

#endif
