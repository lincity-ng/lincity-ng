#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__

#include "Component.hpp"
#include "ComponentHolder.hpp"

class XmlReader;
class Button;

class Window : public Component
{
public:
    Window(Component* parent, XmlReader& reader);
    virtual ~Window();

    virtual void draw(Painter& painter);
    virtual void event(Event& event);

private:
    void closeButtonClicked(Button* button);
    
    float border;
    float titlesize;
    bool dragging;
    Vector2 dragOffset;
    
    ComponentHolder<Button> closeButton;
    ComponentHolder<Component> title;
    ComponentHolder<Component> contents;
};

#endif

