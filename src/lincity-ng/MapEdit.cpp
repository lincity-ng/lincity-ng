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
#include "lincity/lcintl.h"
#include "lincity/engine.h"
#include "lincity/lin-city.h"

#include "Sound.hpp"
#include "MapEdit.hpp"
#include "Dialog.hpp"
#include "Config.hpp"
#include "Debug.hpp"

int monument_bul_flag=0;
int river_bul_flag=0;
int shanty_bul_flag=0;
int last_message_group = 0;

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
no_credit_build_msg (int selected_group)
{
    if( last_message_group == selected_group ){
        return;
    }
    last_message_group = selected_group;
#ifdef GROUP_POWER_SOURCE_NO_CREDIT
  if (selected_group == GROUP_POWER_SOURCE) {
    ok_dial_box ("no-credit-solar-power.mes", BAD, 0L);
    return;
  }
#endif
#ifdef GROUP_UNIVERSITY_NO_CREDIT
  if (selected_group == GROUP_UNIVERSITY) {
    ok_dial_box ("no-credit-university.mes", BAD, 0L);
    return;
  }
#endif
#ifdef GROUP_PARKLAND_NO_CREDIT
  if (selected_group == GROUP_PARKLAND) {
    ok_dial_box ("no-credit-parkland.mes", BAD, 0L);
    return;
  }
#endif
#ifdef GROUP_RECYCLE_NO_CREDIT
  if (selected_group == GROUP_RECYCLE) {
    ok_dial_box ("no-credit-recycle.mes", BAD, 0L);
    return;
  }
#endif
#ifdef GROUP_ROCKET
  if (selected_group == GROUP_ROCKET) {
    ok_dial_box ("no-credit-rocket.mes", BAD, 0L);
    return;
  }
#endif
  return;
}

void
check_bulldoze_area (int x, int y)
{
    //no need to bulldoze grass
    if( MP_TYPE( x, y ) == CST_GREEN ) 
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
        return;
    }

    /* Bring up mappoint_stats for certain left mouse clicks */
    /* Check market and port double-clicks here */
    /* Check rocket launches */
    if( MP_TYPE( x,y ) != CST_GREEN ) {
        if(mapMPS)
            mapMPS->playBuildingSound( mod_x, mod_y );
        mps_result = mps_set( mod_x, mod_y, MPS_MAP ); //query Tool
        
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
        //to allow up/downgrading of Tracks,Roads and Rails we can't allways return.
        if( !getConfig()->upgradeTransport ){
            return;
        }

        if( ( selected_module_type != CST_TRACK_LR ) && 
            ( selected_module_type != CST_ROAD_LR ) && 
            ( selected_module_type != CST_RAIL_LR ) ) {
            return; //not building a transport
        }
        if( !( MP_INFO(x,y).flags & FLAG_IS_TRANSPORT ) ){
            return;//not on transport
        }
        //don't replace anything by itself.
        if( ( selected_module_type == CST_TRACK_LR ) && ( MP_GROUP( x, y ) == GROUP_TRACK ) ){
           return;
        } 
        if( ( selected_module_type == CST_ROAD_LR ) && ( MP_GROUP( x, y ) == GROUP_ROAD ) ){ 
           return;
        } 
        if( ( selected_module_type == CST_RAIL_LR ) && ( MP_GROUP( x, y ) == GROUP_RAIL ) ){ 
           return;
        } 
    }

    //query Tool 
    if(selected_module_type==CST_NONE) {
        if(mapMPS)
            mapMPS->setView(MapPoint(x, y));
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
        if (MP_TYPE(x + 1,y) != CST_GREEN
            || MP_TYPE(x,y + 1) != CST_GREEN
            || MP_TYPE(x + 1,y + 1) != CST_GREEN)
            return;
    }
    if (size >= 3) {
        if (MP_TYPE(x + 2,y) != CST_GREEN
            || MP_TYPE(x + 2,y + 1) != CST_GREEN
            || MP_TYPE(x + 2,y + 2) != CST_GREEN
            || MP_TYPE(x + 1,y + 2) != CST_GREEN
            || MP_TYPE(x,y + 2) != CST_GREEN)
            return;
    }
    if (size == 4) {
        if (MP_TYPE(x + 3,y) != CST_GREEN
            || MP_TYPE(x + 3,y + 1) != CST_GREEN
            || MP_TYPE(x + 3,y + 2) != CST_GREEN
            || MP_TYPE(x + 3,y + 3) != CST_GREEN
            || MP_TYPE(x + 2,y + 3) != CST_GREEN
            || MP_TYPE(x + 1,y + 3) != CST_GREEN
            || MP_TYPE(x,y + 3) != CST_GREEN)
            return;
    }
    
    //how to build a lake in the park? Seems to be impossible
    //in the original game :-)
    if( selected_module_group == GROUP_PARKLAND ){
        Uint8 *keystate = SDL_GetKeyState(NULL);
        if ( keystate[SDLK_w] )
            selected_module_type = CST_PARKLAND_LAKE;
        else
            selected_module_type = CST_PARKLAND_PLANE;
    }

    /* Place the selected item */
    switch (place_item (x, y, selected_module_type))
    {
        case 0:
            /* Success */
            getSound()->playSound( "Build" );
            last_message_group = 0;
            break;
        case -1:
            /* Not enough money */
            no_credit_build_msg (selected_module_group);
            break;
        case -2:
            /* Improper port placement */
            // WolfgangB: The correct placement for the port (river to the east)
            // is shown by red/blue cursor in GameView. So we don't need    
            // a dialog here.
            last_message_group = 0;
            break;
    }
}
