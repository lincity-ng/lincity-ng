#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__

#include "Component.hpp"

class XmlReader;
class Button;

class Window : public Component
{
public:
    Window(Component* parent, XmlReader& reader);
    virtual ~Window();

    void draw(Painter& painter);
    void event(const Event& event);

private:
    void closeButtonClicked(Button* button);
    
    float border;
    float titlesize;
    bool dragging;
    Vector2 dragOffset;
   
    Child& closeButton()
    { return childs[0]; }
    Child& title()
    { return childs[1]; }
    Child& contents()
    { return childs[2]; }
};

#endif

