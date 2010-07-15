/* ---------------------------------------------------------------------- *
 * all_buildings.h
 * This file is part of lincity-ng
 * Lincity is copyright (c) see CREDITS for more informations
 *
 * See COPYING COPYING-data.txt and COPYING-fonts.txt for licenses informations
 * ---------------------------------------------------------------------- */

#ifndef __all_buildings_h__
#define __all_buildings_h__

/************* Buildings stuff *********************/

#define POWER_USE_PER_PERSON 3
#define POWER_RES_OVERHEAD 30
#define POWERS_COAL_OUTPUT 22000
#define MAX_COAL_AT_POWER_STATION 100000

#define DAYS_PER_POLLUTION      14
#define POWERS_COAL_POLLUTION   20
#define INDUSTRY_H_POLLUTION    10
#define COALMINE_POLLUTION      3
#define PORT_POLLUTION          1
#define RAIL_POLLUTION          1
#define DAYS_PER_RAIL_POLLUTION 30
#define ROAD_POLLUTION          1
#define DAYS_PER_ROAD_POLLUTION 20
#define UNNAT_DEATHS_COST       500
#define POL_DIV                 64      /* GCS -- from engine.c */


#define INCOME_TAX_RATE 8
#define COAL_TAX_RATE 20
#define GOODS_TAX_RATE 1
#define DOLE_RATE 15
#define TRANSPORT_COST_RATE 14
#define IM_PORT_COST_RATE 1

#define DAYS_BETWEEN_COVER (NUMOF_DAYS_IN_MONTH*3)
#define DAYS_BETWEEN_FIRES (NUMOF_DAYS_IN_YEAR*2)
#define FIRE_ANIMATION_SPEED 350
#define FIRE_DAYS_PER_SPREAD (NUMOF_DAYS_IN_YEAR/8)
#define FIRE_LENGTH (NUMOF_DAYS_IN_YEAR*5)
#define AFTER_FIRE_LENGTH (NUMOF_DAYS_IN_YEAR*10)

#define TIP_DEGRADE_TIME 200 * NUMOF_DAYS_IN_YEAR

#define MARKET_ANIM_SPEED 750
#define WORKING_POP_PERCENT 45
#define JOB_SWING 15
#define HC_JOB_SWING 10

#define DAYS_PER_STARVE 20

#define COAL_RESERVE_SIZE 10000
#define ORE_RESERVE       1000
#define MIN_ORE_RESERVE_FOR_MINE (ORE_RESERVE)
#define MAX_COAL_AT_MINE 100000
#define MAX_ORE_AT_MINE 100000
#define JOBS_DIG_COAL 900
#define JOBS_DIG_ORE  200
#define JOBS_COALPS_GENERATE 100
#define MAX_JOBS_AT_COALPS 2000
#define JOBS_LOAD_COAL 18
#define JOBS_LOAD_ORE  9
#define JOBS_LOAD_STEEL 15

#define MAX_STEEL_AT_INDUSTRY_H (MAX_STEEL_ON_RAIL*10)

#define MAX_WASTE_AT_TIP  10000000


/*
  JOBS_MAKE_STEEL is the steel made per job at the steel works
  what's it doing here?
 */
#define JOBS_MAKE_STEEL 70
/*
  ORE_MAKE_STEEL is the ore used per unit of STEEL at a steel works
  what's it doing here?
 */
#define ORE_MAKE_STEEL 17
#define MAX_STEEL_IN_MARKET (MAX_STEEL_ON_RAIL*2)

/* interest rate *10  ie 10 is 1% */
#define INTEREST_RATE 15


/******* Buildings // GROUPS ************/
#define red(x) (32 + x)
#define green(x) (64 + x)
#define yellow(x) (96 + x)
#define blue(x) (128 + x)
#define magenta(x) (160 + x)
#define cyan(x) (192 + x)
#define white(x) (224 + x)

#define GROUP_NAME_LEN   20

#define GROUP_BARE_COLOUR  (green(12))
#define GROUP_BARE_COST    0
#define GROUP_BARE_COST_MUL 1
#define GROUP_BARE_BUL_COST 1
#define GROUP_BARE_TECH    0
#define GROUP_BARE_FIREC   0

#define GROUP_TIP_COLOUR (white(16))
#define GROUP_TIP_COST 10000
#define GROUP_TIP_COST_MUL 25
#define GROUP_TIP_BUL_COST 1000000
#define GROUP_TIP_TECH 0
#define GROUP_TIP_FIREC 50

#define GROUP_WATER_COLOUR    (blue(31))
#define GROUP_WATER_COST      1000000
#define GROUP_WATER_COST_MUL 2
#define GROUP_WATER_BUL_COST      1000000
#define GROUP_WATER_TECH      0
#define GROUP_WATER_FIREC 0

#define GROUP_BURNT_COLOUR    (red(30))
#define GROUP_BURNT_COST   0    /* Unused */
#define GROUP_BURNT_COST_MUL   1        /* Unused */
#define GROUP_BURNT_BUL_COST  1000
#define GROUP_BURNT_TECH   0    /* Unused */
#define GROUP_BURNT_FIREC  0

#define GROUP_USED_COLOUR (green(12))   /* Unused */
#define GROUP_USED_COST   0     /* Unused */
#define GROUP_USED_COST_MUL  1  /* Unused */
#define GROUP_USED_BUL_COST  0  /* Unused */
#define GROUP_USED_TECH   0     /* Unused */
#define GROUP_USED_FIREC  0     /* Unused */

#define GROUP_DESERT_COLOUR  (yellow(18))
#define GROUP_DESERT_COST    0
#define GROUP_DESERT_COST_MUL 1
#define GROUP_DESERT_BUL_COST 1
#define GROUP_DESERT_TECH    0
#define GROUP_DESERT_FIREC   0

#define GROUP_TREE_COLOUR  (green(12))
#define GROUP_TREE_COST    0
#define GROUP_TREE_COST_MUL 1
#define GROUP_TREE_BUL_COST 1
#define GROUP_TREE_TECH    0
#define GROUP_TREE_FIREC   0

#define GROUP_TREE2_COLOUR  (green(12))
#define GROUP_TREE2_COST    0
#define GROUP_TREE2_COST_MUL 1
#define GROUP_TREE2_BUL_COST 1
#define GROUP_TREE2_TECH    0
#define GROUP_TREE2_FIREC   0

#define GROUP_TREE3_COLOUR  (green(12))
#define GROUP_TREE3_COST    0
#define GROUP_TREE3_COST_MUL 1
#define GROUP_TREE3_BUL_COST 1
#define GROUP_TREE3_TECH    0
#define GROUP_TREE3_FIREC   0

/********   end of buildings // groups   ************/

#endif // __all_buildings_h__
