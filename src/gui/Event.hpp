#ifndef __EVENT_HPP__
#define __EVENT_HPP__

#include <SDL_events.h>
#include "Vector2.hpp"

class Event
{
public:
    Event(SDL_Event& event);

    enum Type {
        KEYUP,
        KEYDOWN,
        MOUSEMOTION,
        MOUSEBUTTONDOWN,
        MOUSEBUTTONUP
    };
    
    Type type;
    Vector2 mousepos;
    int mousebutton;
    SDL_keysym keysym;
};

#endif

