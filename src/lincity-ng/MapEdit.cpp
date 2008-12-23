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

#include "Sound.hpp"
#include "MapEdit.hpp"
#include "Dialog.hpp"
#include "Config.hpp"
#include "Debug.hpp"
#include "GameView.hpp"

extern void ok_dial_box(const char *, int, const char *);

int monument_bul_flag=0;
int river_bul_flag=0;
int shanty_bul_flag=0;
int build_bridge_flag=0;
int last_message_group = 0;

#ifdef DEBUG
void DBG_TileInfo(int x, int y) {
    fprintf(stderr, "%u,%u:Type=%d, Group=%s(%d), Flags= %08X, Alt.=%d\n", x, y,
        MP_TYPE(x, y), _(main_groups[MP_GROUP(x, y)].name), MP_GROUP(x, y),
        MP_INFO(x, y).flags, ALT(x, y));
}
#endif

void resetLastMessage(){
    last_message_group = 0;
}

// Open Dialog for selected Port 
void
clicked_port_cb (int x, int y)
{
    new Dialog( EDIT_PORT, x, y );
}

// Open Dialog for selected Market 
void
clicked_market_cb (int x, int y)
{
    new Dialog( EDIT_MARKET, x, y );
}

void
check_bulldoze_area (int x, int y)
{
    //no need to bulldoze desert
    if( MP_GROUP( x, y ) == GROUP_DESERT ) 
        return;
    
  int xx, yy, g;
  if (MP_TYPE(x,y) == CST_USED)
    {
      xx = MP_INFO(x,y).int_1;
      yy = MP_INFO(x,y).int_2;
    }
  else
    {
      xx = x;
      yy = y;
    }
  g = MP_GROUP(xx,yy);

  if (g == GROUP_MONUMENT && monument_bul_flag == 0)
    {
        if( last_message_group != GROUP_MONUMENT ){
            new Dialog( BULLDOZE_MONUMENT, xx, yy ); // deletes itself
            last_message_group = GROUP_MONUMENT;
        }
        return;
    }
  else if (g == GROUP_RIVER && river_bul_flag == 0)
    {
        if( last_message_group != GROUP_RIVER ){
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
  else if (g == GROUP_TIP)
    {
      if( last_message_group != GROUP_TIP ){
        ok_dial_box ("nobull-tip.mes", BAD, 0L);
        last_message_group = GROUP_TIP;
      }
      return;
    }
  last_message_group = 0;
  getSound()->playSound( "Raze" );
  bulldoze_item (xx,yy);
}


void editMap (MapPoint point, int button)
{
    if( !getGameView()->inCity( point ) ){
        return;
    }
   
    int x = point.x;
    int y = point.y;
    int selected_module_group = get_group_of_type(selected_module_type);
    
    int size;
    //  int x, y; /* mappoint */
    int mod_x, mod_y; /* upper left coords of module clicked on */
    int mps_result;
    
    if (MP_TYPE(x,y) == CST_USED) {
        mod_x = MP_INFO(x,y).int_1;
        mod_y = MP_INFO(x,y).int_2;
    } else {
        mod_x = x;
        mod_y = y;
    }

    /* Bring up mappoint_stats for any right mouse click */
    if (button == SDL_BUTTON_RIGHT) {
        mps_set( x, y, MPS_ENV);
        return;
    }

    /* Handle bulldozing */
    if (selected_module_type == CST_GREEN && button != SDL_BUTTON_RIGHT) {
        check_bulldoze_area (x, y);
        mps_result = mps_set( mod_x, mod_y, MPS_MAP ); // Update mps on bulldoze
#ifdef DEBUG
        DBG_TileInfo(x, y);
#endif
        return;
    }

    /* Bring up mappoint_stats for certain left mouse clicks */
    /* Check market and port double-clicks here */
    /* Check rocket launches */
    if( !GROUP_IS_BARE(MP_GROUP( x,y )) ) {
        if(mapMPS)
            mapMPS->playBuildingSound( mod_x, mod_y );
        mps_result = mps_set( mod_x, mod_y, MPS_MAP ); //query Tool
#ifdef DEBUG
        DBG_TileInfo(x, y);
#endif
        if( mps_result >= 1 ){
            if( MP_GROUP( mod_x,mod_y ) == GROUP_MARKET ){
                clicked_market_cb (mod_x, mod_y);
                return;
            } else if (MP_GROUP(mod_x,mod_y) == GROUP_PORT) {
                clicked_port_cb (mod_x, mod_y);
                return;
            } else if (MP_TYPE(mod_x,mod_y) >= CST_ROCKET_5 &&
                         MP_TYPE(mod_x,mod_y) <= CST_ROCKET_7){
                //Dialogs delete themself
                new Dialog( ASK_LAUNCH_ROCKET, mod_x,mod_y );
                return;
            }
        }
        //to be here we are not in bulldoze-mode and the tile
        //under the cursor is not empty. 
        //to allow up/downgrading of Tracks,Roads,Rails and bridges we can't always return.
        if( !getConfig()->upgradeTransport ){
            return;
        }

        if( ( selected_module_type != CST_TRACK_LR ) && 
            ( selected_module_type != CST_ROAD_LR ) && 
            ( selected_module_type != CST_RAIL_LR ) ) {
            return; //not building a transport
        }
        
        if( ( MP_GROUP(x,y) != GROUP_WATER ) && ( !( MP_INFO(x,y).flags & FLAG_IS_TRANSPORT ))){
            return; //target area is neither water not a transport
	}    

        if( selected_module_type == CST_TRACK_LR ) {
            if( MP_GROUP( x, y ) == GROUP_TRACK || MP_GROUP( x, y ) == GROUP_TRACK_BRIDGE ||
                    MP_GROUP( x, y ) == GROUP_ROAD || MP_GROUP( x, y ) == GROUP_ROAD_BRIDGE ||
                    MP_GROUP( x, y ) == GROUP_RAIL || MP_GROUP( x, y ) == GROUP_RAIL_BRIDGE )
           return;
        } else if( selected_module_type == CST_ROAD_LR ) {
            if ( MP_GROUP( x, y ) == GROUP_ROAD || MP_GROUP( x, y ) == GROUP_ROAD_BRIDGE ||
                    MP_GROUP( x, y ) == GROUP_RAIL || MP_GROUP( x, y ) == GROUP_RAIL_BRIDGE )
                return;
        } else if( selected_module_type == CST_RAIL_LR ) {
            if( MP_GROUP( x, y ) == GROUP_RAIL || MP_GROUP( x, y ) == GROUP_RAIL_BRIDGE )
                return;
        } 
    }

    //query Tool 
    if(selected_module_type==CST_NONE) {
        if (mapMPS) {
            mapMPS->playBuildingSound( mod_x, mod_y );
            mapMPS->setView(MapPoint( mod_x, mod_y ));
        }
        mps_result = mps_set( mod_x, mod_y, MPS_MAP ); //query Tool on CST_NONE
#ifdef DEBUG
        DBG_TileInfo(x, y);
#endif
        return;
    }

    /* OK, by now we are certain that the user wants to place the item.
       Set the origin based on the size of the selected_module_type, and 
       see if the selected item will fit. */
    size = main_groups[selected_module_group].size;
    /*  if (px > (mw->x + mw->w) - size*16)
        px = (mw->x + mw->w) - size*16;
        if (py > (mw->y + mw->h) - size*16)
        py = (mw->y + mw->h) - size*16;
        pixel_to_mappoint(px, py, &x, &y);
    */
    //Check if we are too close to the border
    if( x + size > WORLD_SIDE_LEN - 1 || y + size > WORLD_SIDE_LEN - 1 || x < 1 || y < 1 )
        return;
    
    if (size >= 2) {
        if (!GROUP_IS_BARE(MP_GROUP(x + 1,y))
            || !GROUP_IS_BARE(MP_GROUP(x,y + 1))
            || !GROUP_IS_BARE(MP_GROUP(x + 1,y + 1)))
            return;
    }
    if (size >= 3) {
        if (!GROUP_IS_BARE(MP_GROUP(x + 2,y))
            || !GROUP_IS_BARE(MP_GROUP(x + 2,y + 1))
            || !GROUP_IS_BARE(MP_GROUP(x + 2,y + 2))
            || !GROUP_IS_BARE(MP_GROUP(x + 1,y + 2))
            || !GROUP_IS_BARE(MP_GROUP(x,y + 2)))
            return;
    }
    if (size == 4) {
        if (!GROUP_IS_BARE(MP_GROUP(x + 3,y))
            || !GROUP_IS_BARE(MP_GROUP(x + 3,y + 1))
            || !GROUP_IS_BARE(MP_GROUP(x + 3,y + 2))
            || !GROUP_IS_BARE(MP_GROUP(x + 3,y + 3))
            || !GROUP_IS_BARE(MP_GROUP(x + 2,y + 3))
            || !GROUP_IS_BARE(MP_GROUP(x + 1,y + 3))
            || !GROUP_IS_BARE(MP_GROUP(x,y + 3)))
            return;
    }
    
    //how to build a lake in the park?
    //just hold 'W' key on build ;-)
    if( selected_module_group == GROUP_PARKLAND ){
        Uint8 *keystate = SDL_GetKeyState(NULL);
        if ( keystate[SDLK_w] )
            selected_module_type = CST_PARKLAND_LAKE;
        else
            selected_module_type = CST_PARKLAND_PLANE;
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
            DBG_TileInfo(x, y);
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
