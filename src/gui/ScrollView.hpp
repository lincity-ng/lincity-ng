#ifndef __SCROLLVIEW_HPP__
#define __SCROLLVIEW_HPP__

#include "Component.hpp"
#include "ComponentHolder.hpp"

class XmlReader;
class ScrollBar;

class ScrollView : public Component
{
public:
    ScrollView(Component* parent, XmlReader& reader);
    virtual ~ScrollView();

    void resize(float width, float height);
    void draw(Painter& painter);
    void event(Event& event);

private:
    void scrollBarChanged(ScrollBar* bar, float newvalue);
    
    ComponentHolder<ScrollBar> scrollBar;
    ComponentHolder<Component> contents;
};

#endif

