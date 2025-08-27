/*
Copyright (C) 2005 David Kamphausen <david.kamphausen@web.de>
Copyright (C) 2024 David Bears <dbear4q@gmail.com>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef __CHECK_BUTTON_HPP__
#define __CHECK_BUTTON_HPP__

#include <SDL.h>              // for Uint32
#include <string>             // for string, basic_string

#include "gui/Child.hpp"      // for Childs
#include "gui/Component.hpp"  // for Component
#include "gui/Signal.hpp"     // for Signal
#include "gui/Vector2.hpp"    // for Vector2

namespace xmlpp {
class TextReader;
}  // namespace xmlpp

class CheckButton : public Component
{
public:
    CheckButton();
    virtual ~CheckButton();

    void parse(xmlpp::TextReader& reader);

    void draw(Painter& painter);
    void event(const Event& event);

    /** This signal is called when the button has been clicked.
     * Arguments are a pointer to the checkbutton and the mousebutton number
     * that has been pressed
     */
    Signal<CheckButton*, int> clicked;
    Signal<CheckButton*, int> pressed;
    Signal<CheckButton*, int> released;
    Signal<CheckButton *> checked;
    Signal<CheckButton *> unchecked;

    void setCaptionText(const std::string &pText);
    std::string getCaptionText();
    Component *getCaption();

    const std::string& getTooltip() const;
    void setTooltip(const std::string &pText);

    void check();
    void uncheck();
    void tryCheck();
    void tryUncheck();
    bool isChecked() const;
    void setAutoCheck(bool check, bool uncheck);

    void enable(bool enabled = true);
    void disable() { enable(false); }
    bool isEnabled() const;

private:
    void setChildImage(Child& child, xmlpp::TextReader& reader);
    void setChildText(Child& child, xmlpp::TextReader& reader);

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

    bool mdisabled = false;
    bool mpressed = false;
    bool mchecked = false;
    bool mhovered = false;

    bool autoCheck;
    bool autoUncheck;
    bool lowerOnClick;
    std::string tooltip;
    Uint32 mouseholdTicks = 0;
    Vector2 mouseholdPos;
};

#endif

/** @file gui/CheckButton.hpp */
