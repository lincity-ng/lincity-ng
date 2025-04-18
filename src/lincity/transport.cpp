/* ---------------------------------------------------------------------- *
 * src/lincity/transport.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 1995-1997 I J Peters
 * Copyright (C) 1997-2005 Greg Sharp
 * Copyright (C) 2000-2004 Corey Keasling
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

#include "transport.hpp"

#include <stdio.h>          // for NULL
#include <list>             // for _List_iterator

#include "MapPoint.hpp"     // for MapPoint
#include "commodities.hpp"  // for Commodity
#include "groups.hpp"       // for GROUP_RAIL, GROUP_ROAD, GROUP_RAIL_BRIDGE
#include "lin-city.hpp"     // for FLAG_TRANSPARENT, FLAG_INVISIBLE, FLAG_PO...
#include "lintypes.hpp"     // for Construction
#include "resources.hpp"    // for ExtraFrame
#include "world.hpp"        // for Map, MapTile

// I don't really like the use of these functions because bounds checks should
// be done by the caller, but alas this is the way connect_transport was
// originally coded and it would be too much effort to change. So this is why
// these two functions are static.
static unsigned short check_group(Map& map, MapPoint point) {
  if(!map.is_inside(point)) return 0;
  else return map(point)->getGroup();
}
static unsigned short check_topgroup(Map& map, MapPoint point) {
  if(!map.is_inside(point)) return 0;
  else return map(point)->getTopGroup();
}

void
Map::connect_transport(int originx, int originy, int lastx, int lasty) {
    Map& map = *this;
    // sets the correct TYPE depending on neighbours, => gives the correct tile to display
    static const short power_table[16] = {
        0, 1, 0, 2,
        0, 3, 0, 4,
        1, 1, 5, 6,
        7, 8, 9, 10
    };

    static const short table[16] = {
        0, 0, 1, 2,
        0, 0, 3, 4,
        1, 5, 1, 6,
        7, 8, 9, 10
    };

    Construction *cstr = 0;
    /* Adjust originx,originy,w,h to proper range */
    if (originx <= 0)
    {   originx = 1;}
    if (originy <= 0)
    {   originy = 1;}
    if (lastx >= map.len())
    {   lastx = map.len() - 1;}
    if (lasty >= map.len())
    {   lasty = map.len() - 1;}

    for(MapPoint p(originx, originy); p.y <= lasty; p.y++)
    for(p.x = originx; p.x <= lastx; p.x++) {
      // First, set up a mask according to directions
      cstr = map(p)->construction;
      int* frame = cstr ? &(cstr->frameIt->frame) : NULL;
      int mask = 0;
      switch (map(p)->getGroup())
      {
      case GROUP_POWER_LINE:
      {
          bool far = false;
          int hivolt = -1;
          map(p)->reportingConstruction->deneighborize();
          /* up -- (ThMO) */
          hivolt = map(p.n())->reportingConstruction?
          map(p.n())->reportingConstruction->tellstuff(STUFF_HIVOLT, -2):-1;
          if ((far = ((p.y > 1) && (map(p.n())->is_water() || map(p.n())->is_transport()))))
          {
              hivolt = map(p.n(2))->reportingConstruction?
              map(p.n(2))->reportingConstruction->tellstuff(STUFF_HIVOLT, -2):-1;
          }
          if(hivolt != -1)
          {
              if (far) //suspended cables
              {
                  //opposite edge
                  if (!map(p.n(2))->reportingConstruction->countPowercables(1))
                  {
                      map(p.n())->flags |= FLAG_POWER_CABLES_0;
                      cstr->link_to(map(p.n(2))->reportingConstruction);
                      mask |=8;
                  }
              }
              else
              {
                  cstr->link_to(map(p.n())->reportingConstruction);
                  mask |=8;
              }
          }
          else
          {   map(p.n())->flags &= ~FLAG_POWER_CABLES_0;}

          /* left -- (ThMO) */
          hivolt = map(p.w())->reportingConstruction?
          map(p.w())->reportingConstruction->tellstuff(STUFF_HIVOLT, -2):-1;
          if((far = ((p.x > 1) && (map(p.w())->is_water() || map(p.w())->is_transport()))))
          {
              hivolt = map(p.w(2))->reportingConstruction?
              map(p.w(2))->reportingConstruction->tellstuff(STUFF_HIVOLT, -2):-1;
          }
          if(hivolt != -1)
          {
              if (far) //suspended cables
              {
                  //opposite edge
                  if ( !map(p.w(2))->reportingConstruction->countPowercables(2) )
                  {
                      cstr->link_to(map(p.w(2))->reportingConstruction);
                      map(p.w())->flags |= FLAG_POWER_CABLES_90;
                      mask |=4;
                  }
              }
              else
              {
                  cstr->link_to(map(p.w())->reportingConstruction);
                  mask |=4;
              }
          }
          else
          {   map(p.w())->flags &= ~FLAG_POWER_CABLES_90;}

          /* right -- (ThMO) */
          hivolt = map(p.e())->reportingConstruction?
          map(p.e())->reportingConstruction->tellstuff(STUFF_HIVOLT, -2):-1;
          if ((far = ((p.x < map.len() - 2) && (map(p.e())->is_water() || map(p.e())->is_transport()))))
          {
              hivolt = map(p.e(2))->reportingConstruction?
              map(p.e(2))->reportingConstruction->tellstuff(STUFF_HIVOLT, -2):-1;
          }
          if(hivolt != -1)
          {
              if (far) //suspended cables
              {
                  //opposite edge
                  if ( !map(p.e(2))->reportingConstruction->countPowercables(4) )
                  {
                      cstr->link_to(map(p.e(2))->reportingConstruction);
                      map(p.e())->flags |= FLAG_POWER_CABLES_90;
                      mask |=2;
                  }
              }
              else
              {
                  cstr->link_to(map(p.e())->reportingConstruction);
                  mask |=2;
              }
          }
          else
          {   map(p.e())->flags &= ~FLAG_POWER_CABLES_90;}

          /* down -- (ThMO) */
          hivolt = map(p.s())->reportingConstruction?
          map(p.s())->reportingConstruction->tellstuff(STUFF_HIVOLT, -2):-1;
          if ((far = (p.y < map.len() - 2) && (map(p.s())->is_water() || map(p.s())->is_transport())))
          {
              hivolt = map(p.s(2))->reportingConstruction?
              map(p.s(2))->reportingConstruction->tellstuff(STUFF_HIVOLT, -2):-1;}
          if(hivolt != -1)
          {
              if (far) //suspended cables
              {
                  //opposite edge
                  if ( !map(p.s(2))->reportingConstruction->countPowercables(8) )
                  {
                      cstr->link_to(map(p.s(2))->reportingConstruction);
                      map(p.s())->flags |= FLAG_POWER_CABLES_0;
                      mask |=1;
                  }
              }
              else
              {
                  cstr->link_to(map(p.s())->reportingConstruction);
                  mask |=1;
              }
          }
          else
          {   map(p.s())->flags &= ~FLAG_POWER_CABLES_0;}

          *frame = power_table[mask];
          break;
      }
      case GROUP_TRACK:
          if (check_group(map, p.n()) == GROUP_TRACK
          ||  check_group(map, p.n()) == GROUP_ROAD
          || (check_group(map, p.n()) == GROUP_RAIL && //rail crossing
              check_group(map, p.n(2)) == GROUP_TRACK))
          {   mask |= 2;}
          if (check_group(map, p.w()) == GROUP_TRACK
          ||  check_group(map, p.w()) == GROUP_ROAD
          || (check_group(map, p.w()) == GROUP_RAIL && //rail crossing
              check_group(map, p.w(2)) == GROUP_TRACK))
          {   mask |= 1;}

          switch (check_topgroup(map, p.e()))
          {
              case GROUP_RAIL:
                  if(check_group(map, p.e(2)) != GROUP_TRACK)
                  {   break;}
              case GROUP_ROAD:
              case GROUP_TRACK:
              case GROUP_TRACK_BRIDGE:
              case GROUP_COMMUNE:
              case GROUP_COALMINE:
              case GROUP_OREMINE:
              case GROUP_INDUSTRY_L:
              case GROUP_INDUSTRY_H:
              case GROUP_RECYCLE:
              case GROUP_TIP:
              case GROUP_PORT:
              case GROUP_COAL_POWER:
                  mask |= 4;
                  break;
          }

          switch (check_topgroup(map, p.s()))
          {
              case GROUP_RAIL:
                  if(check_group(map, p.s(2)) != GROUP_TRACK)
                  {   break;}
              case GROUP_ROAD:
              case GROUP_TRACK:
              case GROUP_TRACK_BRIDGE:
              case GROUP_COMMUNE:
              case GROUP_COALMINE:
              case GROUP_OREMINE:
              case GROUP_INDUSTRY_L:
              case GROUP_INDUSTRY_H:
              case GROUP_RECYCLE:
              case GROUP_TIP:
              case GROUP_PORT:
              case GROUP_COAL_POWER:
                  mask |= 8;
                  break;
          }
          // A track section between 2 bridge sections
          // in this special case we use a pillar bridge section with green
          if ((check_group(map, p.n()) == GROUP_TRACK_BRIDGE && (
                  check_group(map, p.s()) == GROUP_TRACK_BRIDGE || check_group(map, p.s(2)) == GROUP_TRACK_BRIDGE))
                  || (check_group(map, p.s()) == GROUP_TRACK_BRIDGE && (
                  check_group(map, p.n()) == GROUP_TRACK_BRIDGE || check_group(map, p.n(2)) == GROUP_TRACK_BRIDGE)))
          {   *frame = 11;}
          else if ((check_group(map, p.w()) == GROUP_TRACK_BRIDGE && (
                  check_group(map, p.e()) == GROUP_TRACK_BRIDGE || check_group(map, p.e(2)) == GROUP_TRACK_BRIDGE))
                  || (check_group(map, p.e()) == GROUP_TRACK_BRIDGE && (
                  check_group(map, p.w()) == GROUP_TRACK_BRIDGE || check_group(map, p.w(2)) == GROUP_TRACK_BRIDGE)))
          {   *frame = 12;}
          // Set according bridge entrance if any
          else if (check_group(map, p.n()) == GROUP_TRACK_BRIDGE)
          {   *frame = 13;}
          else if (check_group(map, p.w()) == GROUP_TRACK_BRIDGE)
          {   *frame = 14;}
          else if (check_group(map, p.s()) == GROUP_TRACK_BRIDGE)
          {   *frame = 15;}
          else if (check_group(map, p.e()) == GROUP_TRACK_BRIDGE)
          {   *frame = 16;}
          else if (check_group(map, p.e()) == GROUP_RAIL &&
                   check_group(map, p.w()) == GROUP_RAIL &&
                   check_group(map, p.s()) == GROUP_TRACK &&
                   check_group(map, p.n()) == GROUP_TRACK)
          {
              // David: IDK why we're placing a rail, but we don't have
              //        access to the World instance from here
              // railConstructionGroup.placeItem(*this, x,y);
              // cstr = map(x,y)->construction;
              // frame = &(cstr->frameIt->frame);
              // //rewind the loops
              // x -= 2;
              // y -= 2;
              // if (x < originx)
              // {   x = originx;}
              // if (y < originy)
              // {   y = originy;}

              *frame = 21;

          }
          else if (check_group(map, p.s()) == GROUP_RAIL &&
                   check_group(map, p.n()) == GROUP_RAIL &&
                   check_group(map, p.e()) == GROUP_TRACK &&
                   check_group(map, p.w()) == GROUP_TRACK)
          {
              // David: IDK why we're placing a rail, but we don't have
              //        access to the World instance from here
              // railConstructionGroup.placeItem(*this, x,y);
              // cstr = map(x,y)->construction;
              // frame = &(cstr->frameIt->frame);
              // //rewind the loops
              // x -= 2;
              // y -= 2;
              // if (x < originx)
              // {   x = originx;}
              // if (y < originy)
              // {   y = originy;}

              *frame = 22;
          }
          else
          {   *frame = table[mask];}
          //only brige entrances (and bridges) are transparent
          if (*frame >= 11 && *frame <= 12)
          {
              cstr->flags |= FLAG_TRANSPARENT;
          }
          else
          {
             cstr->flags &= ~FLAG_TRANSPARENT;
          }
          break;

      case GROUP_TRACK_BRIDGE:
          // Bridge neighbour priority
          if (check_group(map, p.n()) == GROUP_TRACK_BRIDGE || check_group(map, p.s()) == GROUP_TRACK_BRIDGE
             || check_group(map, p.n()) == GROUP_TRACK || check_group(map, p.s()) == GROUP_TRACK)
          {
              mask |= 2;
              *frame = 0;
          }
          else if (check_group(map, p.w()) == GROUP_TRACK_BRIDGE || check_group(map, p.e()) == GROUP_TRACK_BRIDGE
              || check_group(map, p.w()) == GROUP_TRACK || check_group(map, p.e()) == GROUP_TRACK)
          {
              mask |= 1;
              *frame = 1;
          }
          else //a lonely bridge tile
          {   *frame = 1;}
          cstr->flags |= FLAG_TRANSPARENT;
          break;

      case GROUP_ROAD:
          if (check_group(map, p.n()) == GROUP_ROAD
          ||  check_group(map, p.n()) == GROUP_TRACK
          || (check_group(map, p.n()) == GROUP_RAIL && //rail crossing
              check_group(map, p.n(2)) == GROUP_ROAD))
          {   mask |= 2;}
          if (check_group(map, p.w()) == GROUP_ROAD
          ||  check_group(map, p.w()) == GROUP_TRACK
          || (check_group(map, p.w()) == GROUP_RAIL && //rail crossing
              check_group(map, p.w(2)) == GROUP_ROAD))
          {   mask |= 1;}

          switch (check_topgroup(map, p.e()))
          {
              case GROUP_RAIL:
                  if(check_group(map, p.e(2)) != GROUP_ROAD)
                  {   break;}
              case GROUP_TRACK:
              case GROUP_ROAD:
              case GROUP_COMMUNE:
              case GROUP_COALMINE:
              case GROUP_OREMINE:
              case GROUP_INDUSTRY_L:
              case GROUP_INDUSTRY_H:
              case GROUP_RECYCLE:
              case GROUP_TIP:
              case GROUP_PORT:
              case GROUP_COAL_POWER:
                  mask |= 4;
                  break;
          }
          switch (check_topgroup(map, p.s()))
          {
              case GROUP_RAIL:
                  if(check_group(map, p.s(2)) != GROUP_ROAD)
                  {   break;}
              case GROUP_TRACK:
              case GROUP_ROAD:
              case GROUP_COMMUNE:
              case GROUP_COALMINE:
              case GROUP_OREMINE:
              case GROUP_INDUSTRY_L:
              case GROUP_INDUSTRY_H:
              case GROUP_RECYCLE:
              case GROUP_TIP:
              case GROUP_PORT:
              case GROUP_COAL_POWER:
                  mask |= 8;
                  break;
          }
          // A road section between 2 bridge sections
          // in this special case we use a pillar bridge section with green
          if ((check_group(map, p.n()) == GROUP_ROAD_BRIDGE && (
                  check_group(map, p.s()) == GROUP_ROAD_BRIDGE || check_group(map, p.s(2)) == GROUP_ROAD_BRIDGE))
                  || (check_group(map, p.s()) == GROUP_ROAD_BRIDGE && (
                  check_group(map, p.n()) == GROUP_ROAD_BRIDGE || check_group(map, p.n(2)) == GROUP_ROAD_BRIDGE)))
          {   *frame = 11;}
          else if ((check_group(map, p.w()) == GROUP_ROAD_BRIDGE && (
                  check_group(map, p.e()) == GROUP_ROAD_BRIDGE || check_group(map, p.e(2)) == GROUP_ROAD_BRIDGE))
                  || (check_group(map, p.e()) == GROUP_ROAD_BRIDGE && (
                  check_group(map, p.w()) == GROUP_ROAD_BRIDGE || check_group(map, p.w(2)) == GROUP_ROAD_BRIDGE)))
          {   *frame = 12;}
          // Build bridge entrance2
          else if (check_group(map, p.n()) == GROUP_ROAD_BRIDGE)
          {   *frame = 13;}
          else if (check_group(map, p.w()) == GROUP_ROAD_BRIDGE)
          {   *frame = 14;}
          else if (check_group(map, p.s()) == GROUP_ROAD_BRIDGE)
          {   *frame = 15;}
          else if (check_group(map, p.e()) == GROUP_ROAD_BRIDGE)
          {   *frame = 16;}
          // Build bridge entrance1
          else if (check_group(map, p.n(2)) == GROUP_ROAD_BRIDGE && check_group(map, p.n()) == GROUP_ROAD)
          {   *frame = 17;}
          else if (check_group(map, p.w(2)) == GROUP_ROAD_BRIDGE && check_group(map, p.w()) == GROUP_ROAD)
          {   *frame = 18;}
          else if (check_group(map, p.s(2)) == GROUP_ROAD_BRIDGE && check_group(map, p.s()) == GROUP_ROAD)
          {   *frame = 19;}
          else if (check_group(map, p.e(2)) == GROUP_ROAD_BRIDGE && check_group(map, p.e()) == GROUP_ROAD)
          {   *frame = 20;}
          else if (check_group(map, p.e()) == GROUP_RAIL &&
                   check_group(map, p.w()) == GROUP_RAIL &&
                   check_group(map, p.s()) == GROUP_ROAD &&
                   check_group(map, p.n()) == GROUP_ROAD)
          {
              // David: IDK why we're placing a rail, but we don't have
              //        access to the World instance from here
              // railConstructionGroup.placeItem(*this, x,y);
              // cstr = map(x,y)->construction;
              // frame = &(cstr->frameIt->frame);
              // //rewind the loops
              // x -= 2;
              // y -= 2;
              // if (x < originx)
              // {   x = originx;}
              // if (y < originy)
              // {   y = originy;}

              *frame = 23;
          }
          else if (check_group(map, p.s()) == GROUP_RAIL &&
                   check_group(map, p.n()) == GROUP_RAIL &&
                   check_group(map, p.e()) == GROUP_ROAD &&
                   check_group(map, p.w()) == GROUP_ROAD)
          {
              // David: IDK why we're placing a rail, but we don't have
              //        access to the World instance from here
              // railConstructionGroup.placeItem(*this, x,y);
              // cstr = map(x,y)->construction;
              // frame = &(cstr->frameIt->frame);
              // //rewind the loops
              // x -= 2;
              // y -= 2;
              // if (x < originx)
              // {   x = originx;}
              // if (y < originy)
              // {   y = originy;}

              *frame = 24;
          }
          else
          {   *frame = table[mask];}
          if(*frame >= 11 && *frame <= 16)
          {
              cstr->flags |= FLAG_TRANSPARENT;
          }
          else
          {
             cstr->flags &= ~FLAG_TRANSPARENT;
          }
          break;

      case GROUP_ROAD_BRIDGE:
          // Bridge neighbour priority
          if (check_group(map, p.n()) == GROUP_ROAD_BRIDGE || check_group(map, p.s()) == GROUP_ROAD_BRIDGE)
          {   *frame = 0;}
          else if (check_group(map, p.w()) == GROUP_ROAD_BRIDGE || check_group(map, p.e()) == GROUP_ROAD_BRIDGE)
          {   *frame = 1;}
          else if (check_group(map, p.n()) == GROUP_ROAD || check_group(map, p.s()) == GROUP_ROAD)
          {   *frame = 0;}//2
          else if (check_group(map, p.w()) == GROUP_ROAD || check_group(map, p.e()) == GROUP_ROAD)
          {   *frame = 1;}//3
          else
          {  *frame = 1;}
          cstr->flags |= FLAG_TRANSPARENT;
          break;

      case GROUP_RAIL:
          if (check_group(map, p.n()) == GROUP_RAIL)
          {   mask |= 2;}
          if (check_group(map, p.w()) == GROUP_RAIL)
          {   mask |= 1;}

          switch (check_topgroup(map, p.e())) {
              case GROUP_RAIL:
              case GROUP_COMMUNE:
              case GROUP_COALMINE:
              case GROUP_OREMINE:
              case GROUP_INDUSTRY_L:
              case GROUP_INDUSTRY_H:
              case GROUP_RECYCLE:
              case GROUP_TIP:
              case GROUP_PORT:
              case GROUP_COAL_POWER:
                  mask |= 4;
                  break;
          }
          switch (check_topgroup(map, p.s())) {
              case GROUP_RAIL:
              case GROUP_COMMUNE:
              case GROUP_COALMINE:
              case GROUP_OREMINE:
              case GROUP_INDUSTRY_L:
              case GROUP_INDUSTRY_H:
              case GROUP_RECYCLE:
              case GROUP_TIP:
              case GROUP_PORT:
              case GROUP_COAL_POWER:
                  mask |= 8;
                  break;
          }
          // A rail section between 2 bridge sections
          // in this special case we use a pillar bridge section with green
          if ((check_group(map, p.n()) == GROUP_RAIL_BRIDGE && (
                  check_group(map, p.s()) == GROUP_RAIL_BRIDGE || check_group(map, p.s(2)) == GROUP_RAIL_BRIDGE))
                  || (check_group(map, p.s()) == GROUP_RAIL_BRIDGE && (
                  check_group(map, p.n()) == GROUP_RAIL_BRIDGE || check_group(map, p.n(2)) == GROUP_RAIL_BRIDGE)))
          {   *frame = 11;}
          else if ((check_group(map, p.w()) == GROUP_RAIL_BRIDGE && (
                  check_group(map, p.e()) == GROUP_RAIL_BRIDGE || check_group(map, p.e(2)) == GROUP_RAIL_BRIDGE))
                  || (check_group(map, p.e()) == GROUP_RAIL_BRIDGE && (
                  check_group(map, p.w()) == GROUP_RAIL_BRIDGE || check_group(map, p.w(2)) == GROUP_RAIL_BRIDGE)))
          {   *frame = 12;}
          // Build bridge entrance2
          else if (check_group(map, p.n()) == GROUP_RAIL_BRIDGE)
          {   *frame = 13;}
          else if (check_group(map, p.w()) == GROUP_RAIL_BRIDGE)
          {   *frame = 14;}
          else if (check_group(map, p.s()) == GROUP_RAIL_BRIDGE)
          {   *frame = 15;}
          else if (check_group(map, p.e()) == GROUP_RAIL_BRIDGE)
          {   *frame = 16;}
          // Build bridge entrance1
          else if (check_group(map, p.n(2)) == GROUP_RAIL_BRIDGE && check_group(map, p.n()) == GROUP_RAIL)
          {   *frame = 17;}
          else if (check_group(map, p.w(2)) == GROUP_RAIL_BRIDGE && check_group(map, p.w()) == GROUP_RAIL)
          {   *frame = 18;}
          else if (check_group(map, p.s(2)) == GROUP_RAIL_BRIDGE && check_group(map, p.s()) == GROUP_RAIL)
          {   *frame = 19;}
          else if (check_group(map, p.e(2)) == GROUP_RAIL_BRIDGE && check_group(map, p.e()) == GROUP_RAIL)
          {   *frame = 20;}
          //railroad crossings
          else if (check_group(map, p.e()) == GROUP_TRACK &&
                   check_group(map, p.w()) == GROUP_TRACK)
          {   *frame = 22; }
          else if (check_group(map, p.s()) == GROUP_TRACK &&
                   check_group(map, p.n()) == GROUP_TRACK)
          {   *frame = 21; }
          else if (check_group(map, p.e()) == GROUP_ROAD &&
                   check_group(map, p.w()) == GROUP_ROAD)
          {   *frame = 24; }
          else if (check_group(map, p.s()) == GROUP_ROAD &&
                   check_group(map, p.n()) == GROUP_ROAD)
          {   *frame = 23; }

          else
          {   *frame = table[mask];}
          if(*frame >= 11 && *frame <= 16)
          {
              cstr->flags |= FLAG_TRANSPARENT;
          }
          else
          {
                 cstr->flags &= ~FLAG_TRANSPARENT;
          }
          break;

      case GROUP_RAIL_BRIDGE:
          // Bridge neighbour priority
          if (check_group(map, p.n()) == GROUP_RAIL_BRIDGE || check_group(map, p.s()) == GROUP_RAIL_BRIDGE
             || check_group(map, p.n()) == GROUP_RAIL || check_group(map, p.s()) == GROUP_RAIL)
          {   *frame = 0;}
          else if (check_group(map, p.w()) == GROUP_RAIL_BRIDGE || check_group(map, p.e()) == GROUP_RAIL_BRIDGE
              || check_group(map, p.w()) == GROUP_RAIL || check_group(map, p.e()) == GROUP_RAIL)
          {   *frame = 1;}
          else
          {   *frame = 1;}
          cstr->flags |= FLAG_TRANSPARENT;
          break;
      }
    }
}

/** @file lincity/transport.cpp */
