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

    /** This signal is called when the button has been clicked.
     * Arguments are a pointer to the checkbutton and the mousebutton number
     * that has been pressed
     */
    Signal<CheckButton*, int> clicked;

    void setCaptionText(const std::string &pText);
    std::string getCaptionText();
    
    void setTooltip(const std::string &pText);

    Component *getCaption();
    std::string getMain() const;
    void uncheck();
    void check();

    /** enable/disable a button. A disabled button doesn't react to left-mouse
     * clicks anymore.
     */
    void enable(bool enabled = true);
    bool isEnabled() const;
    
    enum State {
        STATE_NORMAL,
        STATE_HOVER,
        STATE_CLICKED,
        STATE_CHECKED,
        STATE_DISABLED
    };
    
    State state;

private:
    void setChildImage(Child& child, XmlReader& reader);
    void setChildText(Child& child, XmlReader& reader);

    Child& comp_disabled()
    { return childs[0]; }
    Child& comp_normal()
    { return childs[1]; }
    Child& comp_hover()
    { return childs[2]; }
    Child& comp_clicked()
    { return childs[3]; }
    Child& comp_checked()
    { return childs[4]; }
    Child& comp_caption()
    { return childs[5]; }
    
    std::string mmain;
    
    bool lowerOnClick;
    bool checked;    
    bool mclicked;
    std::string tooltip;
    Uint32 mouseholdTicks;
    Vector2 mouseholdPos;
};

#endif
