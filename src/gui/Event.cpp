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
#include "Event.hpp"

#include <assert.h>
#include <SDL_version.h>

Event::Event(SDL_Event& event)
    : inside(true)
{
    switch(event.type) {
        case SDL_KEYUP:
            type = KEYUP;
            keysym = event.key.keysym;
            break;
        case SDL_KEYDOWN:
            type = KEYDOWN;
            keysym = event.key.keysym;
            break;
        case SDL_MOUSEMOTION:
            type = MOUSEMOTION;
            mousepos = Vector2(event.motion.x, event.motion.y);
            mousemove = Vector2(event.motion.xrel, event.motion.yrel);
            break;
        case SDL_MOUSEBUTTONUP:
            type = MOUSEBUTTONUP;
            mousepos = Vector2(event.button.x, event.button.y);
            mousebutton = event.button.button;
            break;
        case SDL_MOUSEBUTTONDOWN:
            type = MOUSEBUTTONDOWN;
            mousepos = Vector2(event.button.x, event.button.y);
            mousebutton = event.button.button;
            break;
        case SDL_MOUSEWHEEL:
            type = MOUSEWHEEL;
            scrolly = event.wheel.y;
            #if SDL_VERSION_ATLEAST(2,26,0)
            mousepos = Vector2(event.wheel.mouseX, event.wheel.mouseY);
            #else
            int x, y;
            SDL_GetMouseState(&x, &y);
            mousepos = Vector2(x, y);
            #endif
            break;
        case SDL_WINDOWEVENT:
            switch(event.window.event) {
            case SDL_WINDOWEVENT_ENTER:
                type = WINDOWENTER;
                break;
            case SDL_WINDOWEVENT_LEAVE:
                type = WINDOWLEAVE;
                break;
            default:
                type = WINDOWOTHER;
            }
            break;
        default:
            assert(false);
    }
}

Event::Event(float _elapsedTime)
    : type(UPDATE), inside(false), elapsedTime(_elapsedTime)
{
}

/** @file gui/Event.cpp */
