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

#include "Debug.hpp"

// FIXME: is this correct ???
int monument_bul_flag=0;
int river_bul_flag=0;
int shanty_bul_flag=0;

int ask_launch_rocket_click (int x, int y)
{
  return true;
}

// FIXME: what is this for ???
void
clicked_port_cb (int x, int y)
{
/*    port_cb_flag = 1;
    mcbx = x;
    mcby = y;*/
}

void
clicked_market_cb (int x, int y)
{
/*    market_cb_flag = 1;
    mcbx = x;
    mcby = y;*/
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

  /* GCS: Free bulldozing of most recently placed item is disabled.
     Still not sure how this can be done w/ multiplayer. */
  if (g == GROUP_MONUMENT && monument_bul_flag == 0)
    {
      if (yn_dial_box (_("WARNING"),
                       _("Bulldozing a monument costs"),
                       _("a lot of money."),
                       _("Want to bulldoze?")) == 0)
        return;
      monument_bul_flag = 1;
    }
  else if (g == GROUP_RIVER && river_bul_flag == 0)
    {
      if (yn_dial_box (_("WARNING"),
                       _("Bulldozing a section of river"),
                       _("costs a lot of money."),
                       _("Want to bulldoze?")) == 0)
        return;
      river_bul_flag = 1;
    }
  else if (g == GROUP_SHANTY && shanty_bul_flag == 0)
    {
      if (yn_dial_box (_("WARNING"),
                       _("Bulldozing a shanty town costs a"),
                       _("lot of money and may cause a fire."),
                       _("Want to bulldoze?")) == 0)
        return;
      shanty_bul_flag = 1;
    }
  else if (g == GROUP_TIP)
    {
      ok_dial_box ("nobull-tip.mes", BAD, 0L);
      return;
    }
  getSound()->playwav( "Raze" );
  bulldoze_item (xx,yy);
}


void editMap (int x, int y, int button)
{
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
  /* XXX: Environmental (right click) MPS should show the clicked
     square, not the master square */
  if (button == SDL_BUTTON_RIGHT)
    {
      mps_set(mod_x, mod_y, MPS_ENV);
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
          if (ask_launch_rocket_click (mod_x,mod_y))
            {
              launch_rocket (mod_x, mod_y);
            }
        }
    }

  /* Handle multitransport */
  if (button == SDL_BUTTON_LEFT &&
      GROUP_IS_TRANSPORT(selected_module_group))
    {

      /*
        FIXME: what was this supposed to be ???

      if (mt_draw (px, py, MT_START))
        {
          // We need to set mps to current location, since the user might
          //   click on the transport to see the mps 

          mps_set(mod_x, mod_y, MPS_MAP);
          return;
        }
        */
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
      getMPS()->setView(x,y);
      return;
    }
  if(selected_module_type==CST_NONE)
  {
    TRACE;
    getMPS()->setView(x,y);

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
    if( x + size > WORLD_SIDE_LEN || y + size > WORLD_SIDE_LEN || x < 0 || y < 0 )
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
      getSound()->playwav( "Build" );
      break;
    case -1:
      /* Not enough money */
      no_credit_build_msg (selected_module_group);
      break;
    case -2:
      /* Improper port placement */
      /* TRANSLATORS: The part about the cup of tea is one of Ian's
         jokes, but the part about ports needing to be connected 
         to rivers is true.  */
      if (yn_dial_box (_("WARNING"),
                       _("Ports need to be"),
                       _("connected to rivers!"),
                       _("Want to make a cup of tea?")) != 0)
        while (yn_dial_box (_("TEA BREAK"),
                            _("Boil->pour->wait->stir"),
                            _("stir->pour->stir->wait->drink...ahhh"),
                            _("Have you finished yet?")) == 0)
          ;
      break;
    }
}
