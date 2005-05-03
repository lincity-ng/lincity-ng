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

#include "Debug.hpp"

int monument_bul_flag=0;
int river_bul_flag=0;
int shanty_bul_flag=0;

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
        new Dialog( BULLDOZE_MONUMENT, xx, yy ); // deletes itself
        return;
    }
  else if (g == GROUP_RIVER && river_bul_flag == 0)
    {
        new Dialog( BULLDOZE_RIVER, xx, yy ); // deletes itself
        return;
    }
  else if (g == GROUP_SHANTY && shanty_bul_flag == 0)
    {
        new Dialog( BULLDOZE_SHANTY, xx, yy ); // deletes itself
        return;
    }
  else if (g == GROUP_TIP)
    {
      ok_dial_box ("nobull-tip.mes", BAD, 0L);
      return;
    }
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

  if (MP_TYPE(x,y) == CST_USED)
    {
      mod_x = MP_INFO(x,y).int_1;
      mod_y = MP_INFO(x,y).int_2;
    }
  else
    {
      mod_x = x;
      mod_y = y;
    }

  /* Bring up mappoint_stats for any right mouse click */
  if (button == SDL_BUTTON_RIGHT)
    {
      mps_set( x, y, MPS_ENV);
      return;
    }

  /* Check rocket launches */
  /* XXX: put this in modules/rocket.c */
  /* XXX: wait for second click to ask for launch */
  if (button == SDL_BUTTON_LEFT)
    {
      if (MP_TYPE(mod_x,mod_y) >= CST_ROCKET_5 &&
          MP_TYPE(mod_x,mod_y) <= CST_ROCKET_7)
        {
          new Dialog( ASK_LAUNCH_ROCKET, mod_x,mod_y );
        }
    }

  /* Handle bulldozing */
  if (selected_module_type == CST_GREEN && button != SDL_BUTTON_RIGHT)
    {
      check_bulldoze_area (x, y);
      return;
    }

  /* Bring up mappoint_stats for certain left mouse clicks */
  /* XXX: Need to check market and port double-clicks here */
  if (MP_TYPE(x,y) != CST_GREEN)
    {
      mps_result = mps_set(mod_x, mod_y, MPS_MAP);

      if (mps_result >= 1)
        {
          if (MP_GROUP(mod_x,mod_y) == GROUP_MARKET)
            {
              clicked_market_cb (mod_x, mod_y);
              return;
            }
          else if (MP_GROUP(mod_x,mod_y) == GROUP_PORT)
            {
              clicked_port_cb (mod_x, mod_y);
              return;
            }
        }
      return;
    }
  if(selected_module_type==CST_NONE)
  {
    getMPS()->setView(MapPoint(x, y));

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
    
  if (size >= 2)
    {
      if (MP_TYPE(x + 1,y) != CST_GREEN
          || MP_TYPE(x,y + 1) != CST_GREEN
          || MP_TYPE(x + 1,y + 1) != CST_GREEN)
        return;
    }
  if (size >= 3)
    {
      if (MP_TYPE(x + 2,y) != CST_GREEN
          || MP_TYPE(x + 2,y + 1) != CST_GREEN
          || MP_TYPE(x + 2,y + 2) != CST_GREEN
          || MP_TYPE(x + 1,y + 2) != CST_GREEN
          || MP_TYPE(x,y + 2) != CST_GREEN)
        return;
    }
  if (size == 4)
    {
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
      break;
    }
}
