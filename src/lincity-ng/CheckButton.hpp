#ifndef __CHECK_BUTTON_HPP__
#define __CHECK_BUTTON_HPP__

#include "gui/Component.hpp"
#include "gui/callback/Signal.hpp"

class XmlReader;
class Alternate;

class CheckButton : public Component
{
public:
    CheckButton();
    virtual ~CheckButton();

    void parse(XmlReader& reader);

    void draw(Painter& painter);
    void event(const Event& event);

    Signal<CheckButton*,int> clicked; // give CheckButton and button

    void setCaptionText(const std::string &pText);
    std::string getCaptionText();

    Component *getCaption();
    std::string getMain() const;
    void uncheck();
    void check();
    
    enum State {
        STATE_NORMAL,
        STATE_HOVER,
        STATE_CLICKED,
        STATE_CHECKED
    };
    
    State state;

private:
    void setChildImage(Child& child, XmlReader& reader);
    void setChildText(Child& child, XmlReader& reader);
    
    Child& comp_normal()
    { return childs[0]; }
    Child& comp_hover()
    { return childs[1]; }
    Child& comp_clicked()
    { return childs[2]; }
    Child& comp_checked()
    { return childs[3]; }
    Child& comp_caption()
    { return childs[4]; }
    
    
    std::string mmain;
    
    bool lowerOnClick;
    bool checked;    
    bool mclicked;
};

#endif
