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
#ifndef __EVENT_HPP__
#define __EVENT_HPP__

#include <SDL_events.h>
#include "Vector2.hpp"

/**
 * @file Event.hpp
 * @brief This class contains informations about events (such as keypresses or
 * mouseclicks)
 * @author Matthias Braun.
 */

class Event
{
public:
    Event(SDL_Event& event);

    enum Type {
        /// update event, sent out once per frame
        UPDATE,
        /// a key was pressed
        KEYDOWN,
        /// a key was released
        KEYUP,
        /// the mouse has been moved
        MOUSEMOTION,
        /// a mouse button has been pressed
        MOUSEBUTTONDOWN,
        /// a mouse button has been released
        MOUSEBUTTONUP
    };
    /// Create an update Event
    Event(float elapsedTime);
    
    /// type of the event
    Type type;
    /// position of the mouse (relative to component origin)
    Vector2 mousepos;
    /// relative mouse movement
    Vector2 mousemove;
    /// number of the mousebutton that has been pressed
    int mousebutton;
    /// symbol of the key that has been pressed (see SDL_keysym)
    SDL_keysym keysym;
    /** set to true if the position where the mouse was clicked/released is 
     * inside the component and the component is not occupied by another
     * component at this position
     */
    bool inside;
    /** For update events this is the time that has elapsed since the last frame
     */
    float elapsedTime;
};

#endif


/** @file gui/Event.hpp */

