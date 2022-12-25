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
#include <config.h>

#include "Event.hpp"

#include <assert.h>

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
            // We may want to set mousepos with event.wheel.mouseX and
            // event.wheel.mouseY. I didn't see this field in the docs
            // (https://wiki.libsdl.org/SDL2/SDL_MouseWheelEvent), but I
            // stumbled across it in the SDL2 source code
            // (https://github.com/libsdl-org/SDL/blob/0a3262e819edc695a764702e8127bbd1b09944ef/src/events/SDL_mouse.c#L864).
            // Also here: https://github.com/libsdl-org/SDL/blob/fcafe40948fe308cc9552df5a3d625ee2725de5a/include/SDL3/SDL_events.h#L320
            // This would help with determining the `inside` field so a scroll
            // event goes to only one component. Evidently, this feature was
            // added in SDL 2.26.0.
            #ifdef HAVE_SDL_MOUSEWHEELEVENT_MOUSEX
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
