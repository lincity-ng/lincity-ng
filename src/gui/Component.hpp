#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include <SDL.h>
#include <string>
#include "Rectangle.hpp"

class Painter;
class Event;

/**
 * The Component is the basic building block of the GUI. It represents a
 * rectangular area on screen which receives events and is able to draw itself.
 * This class has to be used as a base class to implement specific
 * gui-components like buttons or windows.
 *
 * Currently there are 2 sorts of components: resizable ones and fixed-size
 * ones. A fixed size component should have it's size set after creation. (You
 * should set the width and height fields in the constructor).
 * Resizable components should set the resize flag in the constructor and
 * override the resize function. If the resize function is called the component
 * should decide on the size it needs (it might only need a fraction of the size
 * passed in in the resize call), and then set the width and height members
 * accordingly.
 *
 * Components are typically put in a tree. (For example you can put
 * child-components in a TableLayout or in a Window). However components
 * typically don't have to take care of this. The corrdinates in the mouse
 * events are transformed into corrdinates relative to the component origin.
 * Also the painter passed to the draw function will have a transformation
 * set, so that you can pass in coordinates ralative to component origin.
 */
class Component 
{
public:
    /** values for the flags bitfield */
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

    /** returns the component flags (this is a bitfield) */
    int getFlags() const
    {
        return flags;
    }

    /** returns the parent component or 0 if the component has no parent */
    Component* getParent()
    {
        return parent;
    }

protected:
    /**
     * used to parse attributes (from an xml stream for example). Currently
     * parses only the name attribute
     */
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
