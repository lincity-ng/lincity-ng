#ifndef __EVENT_HPP__
#define __EVENT_HPP__

#include <SDL_events.h>
#include "Vector2.hpp"

/**
 * This class contains informations about events (such as keypresses or
 * mouseclicks)
 */
class Event
{
public:
    Event(SDL_Event& event);

    enum Type {
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
    
    /// type of the event
    Type type;
    /// position of the mouse (relative to component origin)
    Vector2 mousepos;
    /// number of the mousebutton that has been pressed
    int mousebutton;
    /// symbol of the key that has been pressed (see SDL_keysym)
    SDL_keysym keysym;
    /** set to true if the position where the mouse was clicked/released is 
     * inside the component and the component is not occupied by another
     * component at this position
     */
    bool inside;
};

#endif

