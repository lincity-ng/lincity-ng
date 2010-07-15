/*
Copyright (C) 2005 Matthias Braun <matze@braunis.de>

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

/**
 * @author Matthias Braun
 * @file Button.hpp
 */

#ifndef __BUTTON_HPP__
#define __BUTTON_HPP__

#include <SDL.h>
#include "Component.hpp"
#include "callback/Signal.hpp"

class XmlReader;
class Texture;

/**
 * @class Button
 * @brief This Component is a clickable button.
 *
 * You can assign images for the 3 differen states of the button: normal, hover
 * (when the mouse is inside the button area) and clicked (when the mouse button
 * is pressed on the button).
 *
 * signalClicked is fired each time the button is pressed.
 */
class Button : public Component
{
public:
    Button();
    virtual ~Button();

    void parse(XmlReader& reader);

    void draw(Painter& painter);
    void event(const Event& event);
    void reLayout();
    
    void setCaptionText(const std::string &pText);
    std::string getCaptionText();

    Signal<Button*> pressed;
    Signal<Button*> released;
    Signal<Button*> clicked;

    enum State {
        STATE_NORMAL,
        STATE_HOVER,
        STATE_CLICKED
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
    Child& comp_caption()
    { return childs[3]; }
    
    bool lowerOnClick;
    std::string tooltip;
    Uint32 mouseholdTicks;
    Vector2 mouseholdPos;
    float fixWidth, fixHeight;
};

#endif

/** @file gui/Button.hpp */

