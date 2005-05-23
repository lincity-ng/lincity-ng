#ifndef __SCROLLBAR_HPP__
#define __SCROLLBAR_HPP__

#include "Component.hpp"
#include "callback/Signal.hpp"

class XmlReader;
class Button;

class ScrollBar : public Component
{
public:
    ScrollBar();
    virtual ~ScrollBar();

    void parse(XmlReader& reader);

    void resize(float width, float height);
    void draw(Painter& painter);
    void event(const Event& event);

    void setRange(float min, float max);
    
    float getRangeMin() const
    {
        return minVal;
    }
    float getRangeMax() const
    {
        return maxVal;
    }
    float getValue() const
    {
        return currentVal;
    }
    void setValue(float value);

    Signal<ScrollBar*, float> valueChanged;

private:
    void buttonPressed(Button* button);
    void buttonReleased(Button* button);
    
    Child& button1()
    { return childs[0]; }
    Child& button2()
    { return childs[1]; }
    Child& scroller()
    { return childs[2]; }
    
    float minVal;
    float maxVal;
    float currentVal;

    float scrollspeed;

    bool scrolling;
    float scrollOffset;
};

#endif

