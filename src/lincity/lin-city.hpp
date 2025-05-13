/* ---------------------------------------------------------------------- *
 * src/lincity/lin-city.hpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 1995-1997 I J Peters
 * Copyright (C) 1997-2005 Greg Sharp
 * Copyright (C) 2000-2004 Corey Keasling
 * Copyright (C) 2022-2025 David Bears <dbear4q@gmail.com>
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

#ifndef __lin_city_h__
#define __lin_city_h__

#define RESULTS_FILENAME "results.txt"

#if defined (_MSC_VER)
#   define snprintf _snprintf
#endif

/* AL1 : unused in 2.x
#define USE_X11_PIXMAPS
*/

#ifndef TRUE
#   define TRUE 1
#endif
#ifndef FALSE
#   define FALSE 0
#endif

// MapTile flags
#define FLAG_POWER_CABLES_0        (0x00000001) // TODO: use extraFrame instead
#define FLAG_POWER_CABLES_90       (0x00000002) // TODO: use extraFrame instead
#define FLAG_MARKET_COVER          (0x00010000)
#define FLAG_MARKET_COVER_CHECK    (0x00020000)
#define FLAG_FIRE_COVER            (0x00040000)
#define FLAG_FIRE_COVER_CHECK      (0x00080000)
#define FLAG_HEALTH_COVER          (0x00100000)
#define FLAG_HEALTH_COVER_CHECK    (0x00200000)
#define FLAG_CRICKET_COVER         (0x00400000)
#define FLAG_CRICKET_COVER_CHECK   (0x00800000)
#define FLAG_IS_LAKE               (0x01000000)
#define FLAG_IS_RIVER              (0x02000000)
#define FLAG_HAS_UNDERGROUND_WATER (0x20000000)

// Construction flags
#define FLAG_FED                   (0x00000020) // TODO: move to residence.h
#define FLAG_EMPLOYED              (0x00000040) // TODO: move to residence.h
#define FLAG_IS_TRANSPORT          (0x00000080) // TODO: volatile
#define FLAG_NEVER_EVACUATE        (0x00000100) // TODO: volatile
#define FLAG_EVACUATE              (0x00000200)
#define FLAG_HAD_POWER             (0x01000000) // TODO: move to residence.h
#define FLAG_TRANSPARENT           (0x08000000) // TODO: volatile

//volatile flags from savegames are ignorred, and will be reset at loading
#define VOLATILE_FLAGS  (FLAG_POWER_CABLES_0 |  FLAG_POWER_CABLES_90 )

//#define SHUFFLE_MAPPOINT_COUNT 4

// TODO: move SIM_DELAY_* to lincity-ng directory
#define SIM_DELAY_PAUSE -1
#define SIM_DELAY_FAST 0
#define SIM_DELAY_MED  20
#define SIM_DELAY_SLOW 60

#define ANIMATE_DELAY 100

// Used to find a threshold time that is not close to an animate target time.
// This helps animation keep a consistent speed with varying fps.
#define ANIM_THRESHOLD(millis) (((millis) + ANIMATE_DELAY / 2) \
  / ANIMATE_DELAY * ANIMATE_DELAY - ANIMATE_DELAY / 2)
/* #define ANIM_THRESHOLD(millis) \
  (millis - (millis + ANIMATE_DELAY / 2) % ANIMATE_DELAY) */

#define MIN_RES_POPULATION 10

/*
   tech-level/TECH_LEVEL_LOSS is lost every month when
   above TECH_LEVEL_LOSS_START
 */
#define MAX_TECH_LEVEL     1000000
#define TECH_LEVEL_LOSS_START 11000
#define TECH_LEVEL_LOSS    500
#define TECH_LEVEL_UNAIDED 2

#endif /* __lin_city_h__ */

/** @file lincity/lin-city.h */
