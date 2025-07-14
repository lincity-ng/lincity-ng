/*
Copyright (C) 2005 Matthias Braun <matze@braunis.de>
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
#ifndef __EVENT_HPP__
#define __EVENT_HPP__

#include <SDL.h>        // for SDL_Event, SDL_Keysym

#include "Vector2.hpp"  // for Vector2

/**
 * @file Event.hpp
 * @brief This class contains informations about events (such as keypresses or
 * mouseclicks)
 * @author Matthias Braun.
**/

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
        MOUSEBUTTONUP,
        /// a mouse wheel has been turned
        MOUSEWHEEL,
        /// window gained mouse focus
        WINDOWENTER,
        /// window lost mouse focus
        WINDOWLEAVE,
    };
    /// Create an update Event
    Event(float elapsedTime);
    /// Create an arbitrary event
    Event(Type type);

    /// type of the event
    Type type;
    /// position of the mouse (relative to component origin)
    Vector2 mousepos;
    /// relative mouse movement
    Vector2 mousemove;
    /// amount scrolled (vertically) by mouse
    int scrolly;
    /// amount scrolled (vertically) by mouse; can be non-integer
    float scrolly_precise;
    /// number of the mousebutton that has been pressed
    int mousebutton;
    /// mouse button state (can be decoded with SDL_BUTTON macros)
    Uint32 mousebuttonstate;
    /// symbol of the key that has been pressed (see SDL_keysym)
    SDL_Keysym keysym;
    /** set to true if the position where the mouse was clicked/released is
     * inside the component and the component is not occupied by another
     * component at this position
     */
    bool inside;
    /** For update events this is the time that has elapsed since the last frame
     */
    float elapsedTime;
};

// TODO: Some day, I want to implement a decent mouse-/keyboard-focus system.
// This would change the WINDOWLEAVE event type into a MOUSELEAVE type and void
// the need for the 'inside' member. Such a system would direct events only
// where they need to go based on which components have focus, and unrelated
// components would not be bothered with the event. This would help solve issues
// where otherwise a component does not realize that it lost focus e.g. for
// ButtonPanel where hacky methods are used to inform buttons that they lose
// focus when a menu is hidden.

#endif


/** @file gui/Event.hpp */
