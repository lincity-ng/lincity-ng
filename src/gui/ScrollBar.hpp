#ifndef __SCROLLBAR_HPP__
#define __SCROLLBAR_HPP__

#include "Component.hpp"
#include "ComponentHolder.hpp"
#include <sigc++/signal.h>

class XmlReader;
class Button;

class ScrollBar : public Component
{
public:
    ScrollBar(Component* parent, XmlReader& reader);
    virtual ~ScrollBar();

    virtual void resize(float width, float height);
    virtual void draw(Painter& painter);
    virtual void event(Event& event);

    void setRange(float min, float max);

    sigc::signal<void, ScrollBar*, float> signalValueChanged;

private:
    void setScrollPos();
    
    ComponentHolder<Button> button1;
    ComponentHolder<Button> button2;
    ComponentHolder<Button> scroller;

    float minVal;
    float maxVal;
    float currentVal;

    bool scrolling;
    float scrollOffset;
};

#endif

