/* ---------------------------------------------------------------------- *
 * src/lincity/resources.hpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2022-2024 David Bears <dbear4q@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
** ---------------------------------------------------------------------- */

#ifndef __LINCITY_RESOURCES_HPP__
#define __LINCITY_RESOURCES_HPP__

#include <iostream>
#include <map>
#include <SDL_mixer.h>      // for Mix_Chunk
#include <SDL.h>    // for SDL_Surface
#include <string>           // for string
#include <vector>           // for vector

#include "gui/Texture.hpp"  // for Texture

class GraphicsInfo
{
    public:
    GraphicsInfo(void){
        texture = (Texture*)'\0';
        image = (SDL_Surface*)'\0';
        x = 0;
        y = 0;
    }

    Texture* texture;
    SDL_Surface* image;
    int x, y;
};

//all instances are added to resMap
class ResourceGroup {
public:

    ResourceGroup(const std::string &tag)
    {
        graphicsInfoVector.clear();
        chunks.clear();
        resourceID = tag;
        images_loaded = false;
        sounds_loaded = false;
        is_vehicle = false;
        //std::cout << "new resourceGroup: " << tag << std::endl;
        if (resMap.count(tag))
        {   std::cout << "rejecting " << tag << " as another ResourceGroup"<< std::endl;}
        else
        {   resMap[tag] = this;}
    }
    ~ResourceGroup()
    {
        std::vector<GraphicsInfo>::iterator it;
        for(it = graphicsInfoVector.begin(); it != graphicsInfoVector.end(); ++it)
        {
            if(it->texture)
            {
                delete it->texture;
                it->texture = 0;
            }
        }
        if ( resMap.count(resourceID))
        {
            resMap.erase(resourceID);
            //std::cout << "sayonara: " << resourceID << std::endl;
        }
        else
        {   std::cout << "error: unreachable resourceGroup: " << resourceID << std::endl;}
    }
    std::string resourceID;
    bool images_loaded;
    bool sounds_loaded;
    bool is_vehicle; //vehicles are always rendered on upper left tile
    std::vector<Mix_Chunk *> chunks;
    std::vector<GraphicsInfo> graphicsInfoVector;
    void growGraphicsInfoVector(void)
    {   graphicsInfoVector.resize(graphicsInfoVector.size() + 1);}
    static std::map<std::string, ResourceGroup*> resMap;
};

struct ExtraFrame{
    ExtraFrame(void){
        move_x = 0;
        move_y = 0;
        frame = 0;
        resourceGroup = 0;
    }

    int move_x; // >0 moves frame to the right
    int move_y; // >0 moves frame downwards
    int frame; //frame >= 0 will be rendered as overlay
    ResourceGroup *resourceGroup; //overlay frame is choosen from its GraphicsInfoVector
};

#endif  // __LINCITY_RESOURCES_HPP__
