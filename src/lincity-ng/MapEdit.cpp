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

// Open Dialog for selected Port
void clicked_port_cb (int x, int y)
{
    new Dialog( EDIT_PORT, x, y );
}

// Open Dialog for selected Market
void clicked_market_cb (int x, int y)
{
    new Dialog( EDIT_MARKET, x, y );
}

void check_bulldoze_area (int x, int y)
{
    //no need to bulldoze desert


    int xx, yy, g;

    g = world(x,y)->getGroup();

    if( g == GROUP_DESERT )
        return;

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
  last_message_group = 0;
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

    int selected_module_group = get_group_of_type(selected_module_type);

    int size, i, j;
    //  int x, y; /* mappoint */
    int mod_x, mod_y; /* upper left coords of module clicked on */
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

    /* Handle bulldozing */
    if (selected_module_type == CST_GREEN && button != SDL_BUTTON_RIGHT)
    {
        check_bulldoze_area (mod_x, mod_y);
        mps_result = mps_set( mod_x, mod_y, MPS_MAP ); // Update mps on bulldoze
#ifdef DEBUG
        //DBG_TileInfo(x, y);
#endif
        return;
    }
    /*Handle Evacuation of Commodities*/
    if (selected_module_type == CST_DESERT && button != SDL_BUTTON_RIGHT)
    {
        if (world(x,y)->reportingConstruction)
        {
            if(world(x,y)->reportingConstruction->flags & FLAG_NEVER_EVACUATE)
            {   return;}
            if(world(x,y)->reportingConstruction->constructionGroup->group == GROUP_MARKET)
            {
                (dynamic_cast<Market*>(world(x,y)->reportingConstruction))->toggleEvacuation();
                return;
            }

            if(world(x,y)->reportingConstruction->flags & FLAG_EVACUATE)
            {
                world(x,y)->reportingConstruction->flags &= ~FLAG_EVACUATE;
            }
            else
            {
                world(x,y)->reportingConstruction->flags |= FLAG_EVACUATE;
            }
            mps_result = mps_set( mod_x, mod_y, MPS_MAP ); // Update mps on evacuate
        }
        return;
    }


    /* Bring up mappoint_stats for certain left mouse clicks */
    /* Check market and port double-clicks here */
    /* Check rocket launches */
    /* Hold d pressed to send load/save info details to console*/
    if( !world(mod_x,mod_y)->is_bare() )
    {
        Uint8 *keystate = SDL_GetKeyState(NULL);
        if ( !binary_mode && keystate[SDLK_d] && world(mod_x,mod_y)->reportingConstruction)
        {
            world(mod_x,mod_y)->reportingConstruction->saveMembers(&std::cout);
        }
        if(mapMPS)
            mapMPS->playBuildingSound( mod_x, mod_y );
        mps_result = mps_set( mod_x, mod_y, MPS_MAP ); //query Tool
#ifdef DEBUG
        //DBG_TileInfo(x, y);
#endif
        if( mps_result >= 1 )
        {
            if( world(mod_x,mod_y)->getGroup() == GROUP_MARKET)
            {
                clicked_market_cb (mod_x, mod_y);
                return;
            } else if (world(mod_x,mod_y)->getGroup() == GROUP_PORT)
            {
                clicked_port_cb (mod_x, mod_y);
                return;
            } else if (world(mod_x,mod_y)->getType() >= CST_ROCKET_5 &&
                         world(mod_x,mod_y)->getType() <= CST_ROCKET_7)
            {
                //Dialogs delete themself
                new Dialog( ASK_LAUNCH_ROCKET, mod_x,mod_y );
                return;
            }
        }// end mps_result>1
        //to be here we are not in bulldoze-mode and the tile
        //under the cursor is not empty.
        //to allow up/downgrading of Buildings and exchanging TransportTilesTracks we can't always return.
        if( ( selected_module_type != CST_TRACK_LR ) &&
            ( selected_module_type != CST_ROAD_LR ) &&
            ( selected_module_type != CST_RAIL_LR ) )
        {
            return; //not building a transport or renewing a building
        }

        if( ( ( selected_module_type == CST_TRACK_LR ) ||
              ( selected_module_type == CST_ROAD_LR  ) ||
              ( selected_module_type == CST_RAIL_LR  )
            ) && !(( world(x,y)->is_transport()
                ||   world(x,y)->is_water()
                ||   world(x,y)->is_powerline() ) ))
        {
            return; //TransportTiles may only overbuild previous TransportTiles or Water
        }
        // TransporstTiles dont overbuild their own kind
        if (selected_module_group == world(x,y)->getTransportGroup())
        {   return;}
    }//end is_not_bare

    //query Tool
    if(selected_module_type==CST_NONE) {
        if (mapMPS) {
            mapMPS->playBuildingSound( mod_x, mod_y );
            mapMPS->setView(MapPoint( mod_x, mod_y ));
        }

        mps_result = mps_set( mod_x, mod_y, MPS_MAP ); //query Tool on CST_NONE
#ifdef DEBUG
        //DBG_TileInfo(x, y);
#endif
        return;
    }

    /* OK, by now we are certain that the user wants to place the item.
       Set the origin based on the size of the selected_module_type, and
       see if the selected item will fit. */
    if ((selected_module_group == GROUP_WINDMILL) && (tech_level >= MODERN_WINDMILL_TECH))
    {
        selected_module_type = CST_WINDMILL_1_R;
        selected_module_group = get_group_of_type(selected_module_type);
    }
    else if ((selected_module_group == GROUP_WIND_POWER) && (tech_level < MODERN_WINDMILL_TECH))
    {
        selected_module_type = CST_WINDMILL_1_W;
        selected_module_group = get_group_of_type(selected_module_type);
    }
    if(ConstructionGroup::countConstructionGroup(selected_module_group))
    {
        size = ConstructionGroup::getConstructionGroup(selected_module_group)->size;
    }
    else
    {
        size = main_groups[selected_module_group].size;
    }
    //Only Check bare space if we are not renewing
    if (!( ( selected_module_type == CST_TRACK_LR ) ||
           ( selected_module_type == CST_ROAD_LR  ) ||
           ( selected_module_type == CST_RAIL_LR  )
         )
        )
    {
        for (i = 0; i < size; i++)
        {
            for (j = 0; j < size; j++)
            {
                if (!world(x+j,y+i)->is_bare() )
                    return;
            }
        }
    }

    //how to build a lake in the park?
    //just hold 'W' key on build ;-)
    if( selected_module_group == GROUP_PARKLAND )
    {
        Uint8 *keystate = SDL_GetKeyState(NULL);
        if ( keystate[SDLK_w] )
            selected_module_type = CST_PARKLAND_LAKE;
        else
            selected_module_type = CST_PARKLAND_PLANE;
    }
    //how to build a shanty?
    //just hold 'S' key on building a Waterwell ;-)
    if( selected_module_group == GROUP_WATERWELL )
    {
        Uint8 *keystate = SDL_GetKeyState(NULL);
        if ( keystate[SDLK_s] )
            selected_module_type = CST_SHANTY;
        else
            selected_module_type = CST_WATERWELL;
    }
    /* Place the selected item . Warning messages are managed by place_item(...) */
    last_message_group = place_item (x, y, selected_module_type);
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
            last_message_group = 0;
    }
}

/** @file lincity-ng/MapEdit.cpp */

