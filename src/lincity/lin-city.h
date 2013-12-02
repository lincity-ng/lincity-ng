/* ---------------------------------------------------------------------- *
 * lin-city.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */

#ifndef __lin_city_h__
#define __lin_city_h__

#include <stdio.h>

#define LC_X11

/*
  When your money reaches 1 million or more, to make it easier to
  read, it is split into two parts; a millions part, and the rest.
   For example  12,355232    You can choose any character you like,
   but I have given some other examples. Just comment out the one
   above, and uncomment one below. IT MUST NOT BE NULL.
  #define MONEY_SEPARATOR '.'
  #define MONEY_SEPARATOR ' '
  #define MONEY_SEPARATOR ':'
*/
#define MONEY_SEPARATOR ',' // AL1 unsused in 2.x , but it would be very good to use it => TODO

/*
       ************************************************
        NO USER CONFIGURABLE OPTIONS BEYOND THIS POINT
       ************************************************
*/
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

/* Used by HandleError () */
#define FATAL   -1
#define WARNING -2

/* comment out the the next three lines for _slightly_ faster X drawing. */
/* AL1 : unused in 2.x
#if defined LC_X11 || defined WIN32
#   define ALLOW_PIX_DOUBLING
#endif
*/

//#define FLAG_LEFT               (1)             // Used for transports, power lines, rivers, and desert borders.
//#define FLAG_UP                 (2)             // Algorithm relies on the numerical values of
#define FLAG_POWER_CABLES_0       (1)
#define FLAG_POWER_CABLES_90      (2)
//#define FLAG_RIGHT              (4)             // the flags. (a check is done at compile time)
//#define FLAG_DOWN               (8)             //
#define FLAG_POWERED            (0x10)
#define FLAG_FED                (0x20)
#define FLAG_EMPLOYED           (0x40)
#define FLAG_IS_TRANSPORT       (0x80)
//#define FLAG_MB_FOOD          (0x100)
#define FLAG_MARKET_COVER       (0x100) //mapTile only
#define FLAG_NEVER_EVACUATE     (0x100) //contruction only
#define FLAG_ALTERED            (0x200) //mapTile only
#define FLAG_EVACUATE           (0x200) //construction only
#define FLAG_IS_GHOST           (0x400) //temporary construction like fireplace
#define FLAG_ROCKET_READY       (0x800) //construction (Rocket Pad) only
//#define FLAG_MB_COAL            (0x1000)
//#define FLAG_MS_COAL            (0x2000)
//#define FLAG_MB_ORE             (0x4000)
//#define FLAG_MS_ORE             (0x8000)
//#define FLAG_MB_GOODS           (0x10000)
//#define FLAG_MS_GOODS           (0x20000)
//#define FLAG_MB_STEEL           (0x40000)
//#define FLAG_MS_STEEL           (0x80000)
#define FLAG_FIRE_COVER         (0x100000)
#define FLAG_HEALTH_COVER       (0x200000)
#define FLAG_CRICKET_COVER      (0x400000)
#define FLAG_IS_RIVER           (0x800000)
#define FLAG_HAD_POWER          (0x1000000)
#define FLAG_MULTI_TRANSPORT    (0x2000000)   /* Is it a multitransport? */     /* AL1: unused in NG 1.1 */
#define FLAG_LACK_JOBS          (0x4000000)     /* 1.92 svn , replace previous unused one. */
#define FLAG_POWER_LINE         (0x8000000)
//#define FLAG_WATERWELL_COVER    (0x10000000) //unused since now STUFF_WATER is used instead
#define FLAG_HAS_UNDERGROUND_WATER (0x20000000)
#define FLAG_ASKED_FOR_POWER       (0x40000000)
#define FLAG_GOT_POWER             (0x80000000) /* 2^31 = last flag */  //nearly duplicate of FLAG_POWERED 0x10

#define VOLATILE_FLAGS  (FLAG_POWER_CABLES_0 |  FLAG_POWER_CABLES_90)
#define GOOD 1
#define BAD (-1)
#define RESULTS 0

//#define SHUFFLE_MAPPOINT_COUNT 4

#define FAST_TIME_FOR_YEAR 9
#define MED_TIME_FOR_YEAR  20
#define SLOW_TIME_FOR_YEAR 60

#define MIN_RES_POPULATION 10

/*
   tech-level/TECH_LEVEL_LOSS is lost every month when
   above TECH_LEVEL_LOSS_START
 */
#define MAX_TECH_LEVEL     1000000
#define TECH_LEVEL_LOSS_START 11000
#define TECH_LEVEL_LOSS    500
#define TECH_LEVEL_UNAIDED 2


#define EX_DISCOUNT_TRIGGER_1   25000
#define EX_DISCOUNT_TRIGGER_2   50000
#define EX_DISCOUNT_TRIGGER_3  100000
#define EX_DISCOUNT_TRIGGER_4  200000
#define EX_DISCOUNT_TRIGGER_5  400000
#define EX_DISCOUNT_TRIGGER_6  800000


#include "lintypes.h"

#endif /* __lin_city_h__ */

/** @file lincity/lin-city.h */

