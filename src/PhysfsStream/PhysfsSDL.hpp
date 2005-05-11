#ifndef __PHYSFSSDL_HPP__
#define __PHYSFSSDL_HPP__

#include <SDL.h>
#include <string>

SDL_RWops* getPhysfsSDLRWops(const std::string& filename);

#endif

