#ifndef __BUTTON_HPP__
#define __BUTTON_HPP__

#include "Component.hpp"
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

    virtual void draw(Painter& painter);
    virtual void event(Event& event);

    sigc::signal<void, Button*> signalClicked;

private:
    bool inside(const Vector2& pos);
    
    enum State {
        STATE_NORMAL,
        STATE_HOVER,
        STATE_CLICKED
    };
    State state;
   
    std::auto_ptr<Texture> normal;
    std::auto_ptr<Texture> hover;
    std::auto_ptr<Texture> clicked;
    std::auto_ptr<Texture> caption;
};

#endif

