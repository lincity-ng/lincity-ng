#ifndef __BUTTON_HPP__
#define __BUTTON_HPP__

#include "Component.hpp"
#include "ComponentHolder.hpp"
#include <sigc++/signal.h>

class XmlReader;
class Texture;

/**
 * This Component is a clickable button. You can assign images for the 3
 * differen states of the button: normal, hover (when the mouse is inside the
 * button area) and clicked (when the mouse button is pressed on the button).
 * 
 * signalClicked is fired each time the button is pressed.
 */
class Button : public Component
{
public:
    Button(Component* parent, XmlReader& reader);
    virtual ~Button();

    void draw(Painter& painter);
    void event(const Event& event);

    sigc::signal<void, Button*> signalClicked;

private:
    bool inside(const Vector2& pos);
    
    enum State {
        STATE_NORMAL,
        STATE_HOVER,
        STATE_CLICKED
    };
    State state;

    Child& normal()
    { return childs[0]; }
    Child& hover()
    { return childs[1]; }
    Child& clicked()
    { return childs[2]; }
    Child& caption()
    { return childs[3]; }
};

#endif

