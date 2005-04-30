#ifndef __WINDOW_HPP__
#define __WINDOW_HPP__

#include "Component.hpp"

class XmlReader;
class Button;

class Window : public Component
{
public:
    Window();
    virtual ~Window();

    void parse(XmlReader& reader);

    void draw(Painter& painter);
    void event(const Event& event);

private:
    void closeButtonClicked(Button* button);
    
    float border;
    float titlesize;
    bool dragging;
    Vector2 dragOffset;

    Child& background()
    { return childs[0]; }
    Child& title_background()
    { return childs[1]; }        
    Child& title()
    { return childs[2]; }
    Child& closeButton()
    { return childs[3]; }
    Child& contents()
    { return childs[4]; }
};

#endif

