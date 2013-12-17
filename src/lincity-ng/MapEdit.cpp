/*
Copyright (C) 2005 David Kamphausen <david.kamphausen@web.de>

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
#include "Mps.hpp"
#include "SDL.h"
#include "lincity/engglobs.h"
#include "lincity/lctypes.h"
#include "gui_interface/mps.h"
#include "gui_interface/screen_interface.h"
#include "gui_interface/shared_globals.h"
#include "tinygettext/gettext.hpp"
#include "lincity/engine.h"
#include "lincity/lin-city.h"
#include "lincity/modules/all_modules.h" //for knowing the individual constructions

#include "Sound.hpp"
#include "MapEdit.hpp"
#include "Dialog.hpp"
#include "Config.hpp"
#include "Debug.hpp"
#include "GameView.hpp"

extern void ok_dial_box(const char *, int, const char *);

int monument_bul_flag = 0;
int river_bul_flag = 0;
int shanty_bul_flag = 0;
int build_bridge_flag = 0;
int last_message_group = 0;

/* #ifdef DEBUG
    void DBG_TileInfo(int x, int y) {
    fprintf(stderr, "%u,%u:Type=%d, Group=%s(%d), Flags= %08X, Alt.=%d\n", x, y,
        world(x, y)->type, _(main_groups[world(x, y)->group].name), world(x, y)->group,
        world(x, y)->flags, world(x, y)->ground.altitude);
    }
    #endif
*/

void resetLastMessage(){
    last_message_group = 0;
}

void check_bulldoze_area (int x, int y)
{
    int xx, yy;
    unsigned short g = world(x,y)->getGroup();
    //no need to bulldoze desert
    if( g == GROUP_DESERT )
    {    return;}
    if (world(x,y)->reportingConstruction)
    {
        xx = world(x,y)->reportingConstruction->x;
        yy = world(x,y)->reportingConstruction->y;
    }
    else
    {
        xx = x;
        yy = y;
    }

    if (g == GROUP_MONUMENT && monument_bul_flag == 0)
    {
        if( (world(x,y)->reportingConstruction->flags & FLAG_EVACUATE)
        && (last_message_group != GROUP_MONUMENT) )
        {
            new Dialog( BULLDOZE_MONUMENT, xx, yy ); // deletes itself
            last_message_group = GROUP_MONUMENT;
        }
        return;
    }
    else if (g == GROUP_RIVER && river_bul_flag == 0)
    {
        if(last_message_group != GROUP_RIVER ){
            new Dialog( BULLDOZE_RIVER, xx, yy ); // deletes itself
            last_message_group = GROUP_RIVER;
        }
        return;
    }
    else if (g == GROUP_SHANTY && shanty_bul_flag == 0)
    {
        if( last_message_group != GROUP_SHANTY ){
            new Dialog( BULLDOZE_SHANTY, xx, yy ); // deletes itself
            last_message_group = GROUP_SHANTY;
        }
        return;
    }
    // only empty landfills may be bulldozed
    else if ( g == GROUP_TIP
    && static_cast<Tip *>(world(x,y)->reportingConstruction)->total_waste > 0 )
    {
      if( last_message_group != GROUP_TIP )
    {
        ok_dial_box ("nobull-tip.mes", BAD, 0L);
        last_message_group = GROUP_TIP;
      }
      return;
    }
    resetLastMessage();
    getSound()->playSound( "Raze" );
    bulldoze_item(xx,yy);
}


void editMap (MapPoint point, int button)
{
    if( !getGameView()->inCity( point ) ){
        return;
    }

    int x = point.x;
    int y = point.y;



    int mod_x, mod_y; // upper left coords of module clicked on
    int mps_result;

    if(world(x,y)->reportingConstruction)
    {
        mod_x = world(x,y)->reportingConstruction->x;
        mod_y = world(x,y)->reportingConstruction->y;
    }
    else
    {
        mod_x = x;
        mod_y = y;
    }

    // Handle bulldozing
    if (userOperation->action == UserOperation::ACTION_BULLDOZE && button != SDL_BUTTON_RIGHT)
    {
        check_bulldoze_area (mod_x, mod_y);
        mps_result = mps_set( mod_x, mod_y, MPS_MAP ); // Update mps on bulldoze
#ifdef DEBUG
        //DBG_TileInfo(x, y);
#endif
        return;
    }
    // show info on any click, but dont do double for query
    if ((!world(mod_x,mod_y)->reportingConstruction) || (userOperation->action != UserOperation::ACTION_QUERY))
    {
        mps_set(mod_x, mod_y, MPS_MAP); //fake Query action
        if(userOperation->action == UserOperation::ACTION_QUERY)
        {
            mapMPS->playBuildingSound( mod_x, mod_y );
            return;
        }
    }

    if(!userOperation->is_allowed_here(mod_x, mod_y, true))
    {   return;}
    //from here on everything should be allowed
    if (userOperation->action == UserOperation::ACTION_FLOOD && button != SDL_BUTTON_RIGHT)
    {
        world(x, y)->setTerrain(GROUP_WATER);
        world(x, y)->flags |= FLAG_ALTERED;
        adjust_money(-selected_module_cost);
        desert_frontier(x - 1, y - 1, 1 + 2, 1 + 2);
        connect_rivers(x, y);
        connect_transport(x - 2, y - 2, x + 1 + 1, y + 1 + 1);
        return;
    }

    // Check market and port double-clicks here
    // Check rocket launches
    // Hold d pressed to send load/save info details to console
    if(userOperation->action == UserOperation::ACTION_QUERY)
    {
#ifdef DEBUG
        Uint8 *keystate = SDL_GetKeyState(NULL);
        if ( !binary_mode && keystate[SDLK_d] && world(mod_x,mod_y)->reportingConstruction)
        {
            world(mod_x,mod_y)->reportingConstruction->saveMembers(&std::cout);
        }
#endif
        mps_result = mps_set( mod_x, mod_y, MPS_MAP );
        mapMPS->playBuildingSound( mod_x, mod_y );

        //DBG_TileInfo(x, y);

        if( mps_result >= 1 )
        {
            if( world(mod_x,mod_y)->getGroup() == GROUP_MARKET)
            {
                new Dialog( EDIT_MARKET, mod_x, mod_y );
                return;
            } else if (world(mod_x,mod_y)->getGroup() == GROUP_PORT)
            {
                new Dialog( EDIT_PORT, mod_x, mod_y );
                return;
            } else if(world(mod_x,mod_y)->getGroup() == GROUP_ROCKET)
            {
                if (world(mod_x,mod_y)->getType() >= 4 &&
                         world(mod_x,mod_y)->getType() < 7)
                {
                    new Dialog( ASK_LAUNCH_ROCKET, mod_x,mod_y );
                    return;
                }
            }
        }// end mps_result>1

        return;
    }

    //Handle Evacuation of Commodities
    if (userOperation->action == UserOperation::ACTION_EVACUATE && button != SDL_BUTTON_RIGHT)
    {
        if(world(x,y)->reportingConstruction->constructionGroup->group == GROUP_MARKET)
        {
            (dynamic_cast<Market*>(world(x,y)->reportingConstruction))->toggleEvacuation();
            return;
        }
        if(world(x,y)->reportingConstruction->flags & FLAG_EVACUATE)
        {   world(x,y)->reportingConstruction->flags &= ~FLAG_EVACUATE;}
        else
        {   world(x,y)->reportingConstruction->flags |= FLAG_EVACUATE;}
        mps_result = mps_set( mod_x, mod_y, MPS_MAP ); // Update mps on evacuate
        return;
    }

    if(userOperation->action == UserOperation::ACTION_BUILD) //MUST BE TRUE
    {
        //double check windmill tech
        //int selected_module_group = userOperation->constructionGroup?userOperation->constructionGroup->group:0;
        if ((userOperation->constructionGroup == &windmillConstructionGroup) && (tech_level >= MODERN_WINDMILL_TECH))
        {   userOperation->constructionGroup = &windpowerConstructionGroup;}
        else if (( userOperation->constructionGroup == &windpowerConstructionGroup) && (tech_level < MODERN_WINDMILL_TECH))
        {   userOperation->constructionGroup = &windmillConstructionGroup;}
        //how to build a lake in the park?
        //just hold 'W' key on build ;-)
        if( userOperation->constructionGroup == &parklandConstructionGroup ||
            userOperation->constructionGroup == &parkpondConstructionGroup )
        {
            Uint8 *keystate = SDL_GetKeyState(NULL);
            if ( keystate[SDLK_w] )
            {   userOperation->constructionGroup = &parkpondConstructionGroup;}
            else
            {   userOperation->constructionGroup = &parklandConstructionGroup;}
        }
/*
        //how to build a shanty?
        //just hold 'S' key on building a water tower ;-)

        if( userOperation->constructionGroup == &waterwellConstructionGroup
         || userOperation->constructionGroup == &shantyConstructionGroup)
        {
            Uint8 *keystate = SDL_GetKeyState(NULL);
            if ( keystate[SDLK_s] )
            {   userOperation->constructionGroup = &shantyConstructionGroup;}
            else
            {   userOperation->constructionGroup = &waterwellConstructionGroup;}
        }
*/
        // place the selected item.
        last_message_group = place_item(mod_x, mod_y);
        switch (last_message_group)
        {
            case 0:
                /* Success */
                getSound()->playSound( "Build" );
                mps_result = mps_set( mod_x, mod_y, MPS_MAP ); // Update mps on well-built
#ifdef DEBUG
                //DBG_TileInfo(x, y);
#endif
                break;
            case -1000:
                /* ouch group does not exist */
            case -1:
                /* Not enough money */
            case -2:
                /* Improper port placement */
            case -3:
                /* too many windmills/substations */
            case -4:
                /* too many market */
            case -5:
                /* previous tip here, cannot build tip here */
            case -6:
                /* previous tip here, cannot build oremine */
            case -7:
                /* no ore reserve. cannot build oremine here */
            default:
                /* warning messages are managed by place item */
               resetLastMessage();
        }
    }
    else
    {   std::cout << "unexpected UserOperation in MapEdit" << std::endl;}
}

/** @file lincity-ng/MapEdit.cpp */

