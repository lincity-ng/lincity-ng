#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include <SDL.h>
#include <string>
#include "Rectangle.hpp"

class Painter;
class Event;

class Component 
{
public:
    enum {
        FLAG_RESIZABLE = 0x00000001
    };
        
    Component(Component* parent);
    virtual ~Component();

    virtual void draw(Painter& painter) = 0;
    virtual void event(Event& event) = 0;
    virtual void resize(float width, float height)
    {}

    virtual float getWidth() const
    {
        return width;
    }

    const std::string& getName() const
    {
        return name;
    }

    void setName(const std::string& name)
    {
        this->name = name;
    }
    
    virtual float getHeight() const
    {
        return height;
    }

    int getFlags() const
    {
        return flags;
    }

    Component* getParent()
    {
        return parent;
    }

protected:
    virtual bool parseAttributeValue(const char* attribute, const char* value);
    
    void setFlags(int flags)
    {
        this->flags = flags;
    }     

    Component* parent;
    int flags;
    float width, height;
    std::string name;
};

#endif
