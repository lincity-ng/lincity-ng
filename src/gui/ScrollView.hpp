#ifndef __SCROLLVIEW_HPP__
#define __SCROLLVIEW_HPP__

#include "Component.hpp"

class XmlReader;
class ScrollBar;

class ScrollView : public Component
{
public:
    ScrollView();
    virtual ~ScrollView();

    void parse(XmlReader& reader);

    void resize(float width, float height);

private:
    void scrollBarChanged(ScrollBar* bar, float newvalue);

    Child& contents()
    { return childs[0]; }
    Child& scrollBar()
    { return childs[1]; }
};

#endif

