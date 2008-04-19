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
#define MONEY_SEPARATOR ','

/*
       ************************************************
        NO USER CONFIGURABLE OPTIONS BEYOND THIS POINT
       ************************************************
*/
#if defined (_MSC_VER)
#   define snprintf _snprintf
#endif
#define USE_X11_PIXMAPS

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
#if defined LC_X11 || defined WIN32
#   define ALLOW_PIX_DOUBLING
#endif

#if defined (LC_X11) || defined (WIN32)
#    define BORDERX 30
#    define BORDERY 30
#endif

#define TEXT_FG_COLOUR  (white(24))
#define TEXT_BG_COLOUR  105
#define YN_DIALBOX_BG_COLOUR (red(10))
#define LOAD_BG_COLOUR       (cyan(10))
#define SAVE_BG_COLOUR       (magenta(10))
#define NW_BG_COLOUR         (green(10))

/* GCS: We are about to run out of bits on the flags, so in the future we
   might need to implement compression.  For example, FLAG_IS_RIVER is 
   only used for GROUP_WATER, FLAG_MB_COAL for GROUP_MARKET, and so on. */
/* WCK: Compression would work, but that's complicated.   I would rather move
   FLAG_M?_* into an int_? variable.  And add more int_? variables.  That keeps
   it simple and squeezes some life out of 32 bits.  I don't think we need to
   hurry.  I'd rather replace the whole scheme. */

#define FLAG_LEFT               (1)             // Used for transports, power lines, rivers, and desert borders.
#define FLAG_UP                 (2)             // Algorithm relies on the numerical values of 
#define FLAG_RIGHT              (4)             // the flags. (a check is done at compile time)
#define FLAG_DOWN               (8)             // 
#define FLAG_POWERED            (0x10)
#define FLAG_FED                (0x20)
#define FLAG_EMPLOYED           (0x40)
#define FLAG_IS_TRANSPORT       (0x80)
#define FLAG_MB_FOOD            (0x100)
#define FLAG_MS_FOOD            (0x200)
#define FLAG_MB_JOBS            (0x400)
#define FLAG_MS_JOBS            (0x800)
#define FLAG_MB_COAL            (0x1000)
#define FLAG_MS_COAL            (0x2000)
#define FLAG_MB_ORE             (0x4000)
#define FLAG_MS_ORE             (0x8000)
#define FLAG_MB_GOODS           (0x10000)
#define FLAG_MS_GOODS           (0x20000)
#define FLAG_MB_STEEL           (0x40000)
#define FLAG_MS_STEEL           (0x80000)
#define FLAG_FIRE_COVER         (0x100000)
#define FLAG_HEALTH_COVER       (0x200000)
#define FLAG_CRICKET_COVER      (0x400000)
#define FLAG_IS_RIVER           (0x800000)
#define FLAG_HAD_POWER          (0x1000000)
#define FLAG_MULTI_TRANSPORT    (0x2000000)   /* Is it a multitransport? */     /* AL1: unused in NG 1.1 */
#define FLAG_LACK_JOBS          (0x4000000)     /* 1.92 svn , replace previous unused one. */
#define FLAG_POWER_LINE         (0x8000000)
#define FLAG_WATERWELL_COVER    (0x10000000)
#define FLAG_HAS_UNDERGROUND_WATER (0x20000000)
#define FLAG_ASKED_FOR_POWER       (0x40000000)
#define FLAG_GOT_POWER             (0x80000000) /* 2^31 = last flag */  //nearly duplicate of FLAG_POWERED 0x10

/* XXX: It would appear that the following T_ are used exactly two times each,
   in market.c.  */
#define T_FOOD  0
#define T_JOBS  1
#define T_COAL  2
#define T_GOODS 3
#define T_ORE   4
#define T_STEEL 5
#define T_WASTE 6

/* XXX: screen.h? */

#define MINI_SCREEN_NORMAL_FLAG    (0)
#define MINI_SCREEN_POL_FLAG       (1)
#define MINI_SCREEN_UB40_FLAG      (2)
#define MINI_SCREEN_STARVE_FLAG    (3)
#define MINI_SCREEN_PORT_FLAG      (4)
#define MINI_SCREEN_POWER_FLAG     (5)
#define MINI_SCREEN_FIRE_COVER     (6)
#define MINI_SCREEN_CRICKET_COVER  (7)
#define MINI_SCREEN_HEALTH_COVER   (8)
#define MINI_SCREEN_COAL_FLAG      (9)

#define MAIN_SCREEN_NORMAL_FLAG    (1)
#define MAIN_SCREEN_EQUALS_MINI    (2)

#define WORLD_SIDE_LEN 100      /* Minimap size is hardcoded 200 pixel => some job to do ... */
#define NUMOF_DAYS_IN_MONTH 100
#define NUMOF_DAYS_IN_YEAR (NUMOF_DAYS_IN_MONTH*12)
#define FAST_TIME_FOR_YEAR 9
#define MED_TIME_FOR_YEAR  20
#define SLOW_TIME_FOR_YEAR 60

/* interest rate *10  ie 10 is 1% */
#define INTEREST_RATE 15

#define HELPERRORPAGE "error.hlp"
#define HELPBACKGROUNDCOLOUR (white(8))
#define HELPBUTTON_COLOUR (white(25))
#define MAX_NUMOF_HELP_BUTTONS 40
#define MAX_LENOF_HELP_FILENAME 40
#define MAX_HELP_HISTORY 20
#define CS_MOUSE_BUTTON_DELAY 5
#define RIGHT_MOUSE_MOVE_VAL 5

#define GOOD 1
#define BAD (-1)
#define RESULTS 0

/************* Buildings stuff *********************/

#define WINDMILL_POWER      450
#define WINDMILL_JOBS       10
/* WINDMILL_RCOST is days per quid */
#define WINDMILL_RCOST      3
#define ANTIQUE_WINDMILL_ANIM_SPEED 160
#define MODERN_WINDMILL_ANIM_SPEED 120

#define OLD_MAX_NUMOF_SUBSTATIONS 100
#define MAX_NUMOF_SUBSTATIONS 512
#define SUBSTATION_RANGE 10
#define POWER_USE_PER_PERSON 3
#define POWER_RES_OVERHEAD 30
#define POWERS_COAL_OUTPUT 22000
#define MAX_COAL_AT_POWER_STATION 100000

#define MAX_ORE_AT_INDUSTRY_L 20000
#define MAX_JOBS_AT_INDUSTRY_L 500
#define MAX_GOODS_AT_INDUSTRY_L 65000
#define INDUSTRY_L_GET_JOBS    120
#define INDUSTRY_L_GET_ORE     600
#define INDUSTRY_L_GET_STEEL   60
#define INDUSTRY_L_JOBS_LOAD_ORE 1
#define INDUSTRY_L_JOBS_LOAD_STEEL 4
#define INDUSTRY_L_ORE_USED  125
#define INDUSTRY_L_STEEL_USED  12
#define INDUSTRY_L_JOBS_USED       30
#define INDUSTRY_L_MAKE_GOODS 225
#define MAX_STEEL_AT_INDUSTRY_L 2000
#define MIN_JOBS_AT_INDUSTRY_L (INDUSTRY_L_JOBS_LOAD_ORE\
+INDUSTRY_L_JOBS_LOAD_STEEL+INDUSTRY_L_JOBS_USED)
#define INDUSTRY_L_ANIM_SPEED 290
#define INDUSTRY_L_POL_PER_GOOD 0.05
#define INDUSTRY_L_POLLUTION    10

#define MAX_ORE_AT_INDUSTRY_H 10000
#define MAX_COAL_AT_INDUSTRY_H 1000
#define MAX_MADE_AT_INDUSTRY_H MAX_ORE_AT_INDUSTRY_H/16
#define POWER_MAKE_STEEL 200
#define INDUSTRY_H_ANIM_SPEED  290

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

#define FINANCE_X 91
#define FINANCE_W 120
#define FINANCE_Y 200
#define FINANCE_H 56
#define INCOME_TAX_RATE 8
#define COAL_TAX_RATE 20
#define GOODS_TAX_RATE 1
#define DOLE_RATE 15
#define TRANSPORT_COST_RATE 14
#define IM_PORT_COST_RATE 1
#define PORT_FOOD_RATE    1
#define PORT_JOBS_RATE    5
#define PORT_COAL_RATE    50
#define PORT_ORE_RATE     1
#define PORT_GOODS_RATE   3
#define PORT_STEEL_RATE   100
/*
  These next two control the stuff bought or sold as a % of what's on the
  transport.  1000=100%  500=50% etc.
*/
#define PORT_EXPORT_RATE  500
#define PORT_IMPORT_RATE  500

#define NUMOF_DISCOUNT_TRIGGERS 6
#define EX_DISCOUNT_TRIGGER_1   25000
#define EX_DISCOUNT_TRIGGER_2   50000
#define EX_DISCOUNT_TRIGGER_3  100000
#define EX_DISCOUNT_TRIGGER_4  200000
#define EX_DISCOUNT_TRIGGER_5  400000
#define EX_DISCOUNT_TRIGGER_6  800000

#define MIN_RES_POPULATION 10
#define MAX_TECH_LEVEL	   1000000
#define TECH_LEVEL_LOSS_START 11000
/*
   tech-level/TECH_LEVEL_LOSS is lost every month when
   above TECH_LEVEL_LOSS_START
 */
#define TECH_LEVEL_LOSS    500
#define TECH_LEVEL_UNAIDED 2
#define MODERN_WINDMILL_TECH 450000

#define BUILD_MONUMENT_JOBS     350000
#define MONUMENT_GET_JOBS       100
#define MONUMENT_DAYS_PER_TECH  3
#define MONUMENT_TECH_EXPIRE    400
#define MONUMENT_ANIM_SPEED     300

#define BLACKSMITH_JOBS    35
#define BLACKSMITH_GET_COAL 6
#define MAX_GOODS_AT_BLACKSMITH 200
#define MAX_COAL_AT_BLACKSMITH  100
#define BLACKSMITH_STEEL_USED 1
#define BLACKSMITH_COAL_USED  1
#define GOODS_MADE_BY_BLACKSMITH 50
#define BLACKSMITH_BATCH (GOODS_MADE_BY_BLACKSMITH*100)
#define BLACKSMITH_ANIM_THRESHOLD 10
#define BLACKSMITH_ANIM_SPEED    200

#define MILL_JOBS          35
#define MAX_GOODS_AT_MILL  200
#define MAX_FOOD_AT_MILL   500
#define MAX_COAL_AT_MILL   50
#define GOODS_MADE_BY_MILL 75
#define MILL_GET_FOOD      51
#define MILL_GET_COAL      4
#define MILL_POWER_PER_COAL 60
#define FOOD_USED_BY_MILL  (GOODS_MADE_BY_MILL/2)
#define COAL_USED_BY_MILL  1
#define MILL_ANIM_THRESHOLD 10
#define MILL_ANIM_SPEED    300

#define SCHOOL_JOBS        50
#define SCHOOL_GOODS	   75
#define JOBS_MAKE_TECH_SCHOOL  200
#define GOODS_MAKE_TECH_SCHOOL  75
#define TECH_MADE_BY_SCHOOL    2
#define MAX_JOBS_AT_SCHOOL     400
#define MAX_GOODS_AT_SCHOOL    200
#define SCHOOL_RUNNING_COST    2

#define DAYS_BETWEEN_COVER (NUMOF_DAYS_IN_MONTH*3)
#define DAYS_BETWEEN_FIRES (NUMOF_DAYS_IN_YEAR*2)
#define FIRESTATION_JOBS   6
#define FIRESTATION_GET_JOBS 7
#define MAX_JOBS_AT_FIRESTATION (FIRESTATION_JOBS*NUMOF_DAYS_IN_YEAR)
#define FIRESTATION_GOODS  2
#define FIRESTATION_GET_GOODS 3
#define MAX_GOODS_AT_FIRESTATION (FIRESTATION_GOODS*NUMOF_DAYS_IN_YEAR)
#define FIRESTATION_RANGE  18
#define FIRESTATION_RUNNING_COST 1
#define FIRESTATION_RUNNING_COST_MUL 6
#define FIRESTATION_ANIMATION_SPEED 250
#define FIRE_ANIMATION_SPEED 350
#define FIRE_DAYS_PER_SPREAD (NUMOF_DAYS_IN_YEAR/8)
#define FIRE_LENGTH (NUMOF_DAYS_IN_YEAR*5)
#define AFTER_FIRE_LENGTH (NUMOF_DAYS_IN_YEAR*10)

#define CRICKET_JOBS   8
#define CRICKET_GET_JOBS 9
#define MAX_JOBS_AT_CRICKET (CRICKET_JOBS*NUMOF_DAYS_IN_YEAR)
#define CRICKET_GOODS  2
#define CRICKET_GET_GOODS 3
#define MAX_GOODS_AT_CRICKET (CRICKET_GOODS*NUMOF_DAYS_IN_YEAR)
#define CRICKET_RANGE  10
#define CRICKET_RUNNING_COST 1
#define CRICKET_ANIMATION_SPEED 750

#define MAX_GOODS_AT_POTTERY     200
#define POTTERY_ORE_MAKE_GOODS    11
#define POTTERY_COAL_MAKE_GOODS    2
#define POTTERY_JOBS              35
#define MAX_ORE_AT_POTTERY        (POTTERY_ORE_MAKE_GOODS*60)
#define POTTERY_GET_ORE           (POTTERY_ORE_MAKE_GOODS*20)
#define MAX_COAL_AT_POTTERY       (POTTERY_COAL_MAKE_GOODS*60)
#define POTTERY_GET_COAL          (POTTERY_COAL_MAKE_GOODS*21)
#define MAX_JOBS_AT_POTTERY       (POTTERY_JOBS*20)
#define POTTERY_GET_JOBS          (POTTERY_JOBS*2)
#define POTTERY_MADE_GOODS        35
#define POTTERY_ANIM_SPEED        280
#define POTTERY_CLOSE_TIME        20

#define UNIVERSITY_JOBS   250
#define UNIVERSITY_JOBS_STORE 5000
#define UNIVERSITY_GOODS  750
#define UNIVERSITY_GOODS_STORE 7000
#define UNIVERSITY_RUNNING_COST 23
#define UNIVERSITY_TECH_MADE    4

#define ROCKET_PAD_JOBS         1000
#define ROCKET_PAD_JOBS_STORE   10000
#define ROCKET_PAD_GOODS        10000
#define ROCKET_PAD_GOODS_STORE  500000
#define ROCKET_PAD_STEEL	500
#define ROCKET_PAD_STEEL_STORE  12000
#define ROCKET_PAD_RUNNING_COST 200
#define ROCKET_PAD_LAUNCH       100
#define ROCKET_ANIMATION_SPEED  450
#define ROCKET_LAUNCH_BAD       1
#define ROCKET_LAUNCH_GOOD      2
#define ROCKET_LAUNCH_EVAC      3

#define TIP_DEGRADE_TIME 200 * NUMOF_DAYS_IN_YEAR

#define MAX_WASTE_AT_RECYCLE 20000
#define BURN_WASTE_AT_RECYCLE (MAX_WASTE_AT_RECYCLE/200)
#define MAX_ORE_AT_RECYCLE   (MAX_ORE_ON_RAIL*2)
#define GOODS_RECYCLED       500
#define RECYCLE_GOODS_JOBS   (GOODS_RECYCLED/50)
#define RECYCLE_RUNNING_COST 3

#define HEALTH_CENTRE_JOBS   6
#define HEALTH_CENTRE_GET_JOBS 7
#define MAX_JOBS_AT_HEALTH_CENTRE (HEALTH_CENTRE_JOBS*NUMOF_DAYS_IN_YEAR)
#define HEALTH_CENTRE_GOODS  40
#define HEALTH_CENTRE_GET_GOODS 41
#define MAX_GOODS_AT_HEALTH_CENTRE (HEALTH_CENTRE_GOODS*NUMOF_DAYS_IN_YEAR)
#define HEALTH_RUNNING_COST  2
#define HEALTH_RUNNING_COST_MUL 9
#define HEALTH_CENTRE_RANGE  15

#define OREMINE_ANIMATION_SPEED 200

#define OLD_MAX_NUMOF_MARKETS 100
#define MAX_NUMOF_MARKETS 512
#define MARKET_RANGE      10
#define MAX_FOOD_ON_TRACK 2048
#define MAX_FOOD_ON_ROAD (MAX_FOOD_ON_TRACK*4)
#define MAX_FOOD_ON_RAIL (MAX_FOOD_ON_TRACK*16)
#define MAX_FOOD_IN_MARKET (MAX_FOOD_ON_RAIL*8)
#define MARKET_FOOD_SEARCH_TRIGGER (MAX_FOOD_IN_MARKET/5)
#define MAX_JOBS_ON_TRACK 1024
#define MAX_JOBS_ON_ROAD (MAX_JOBS_ON_TRACK*5)
#define MAX_JOBS_ON_RAIL (MAX_JOBS_ON_TRACK*25)
#define MAX_JOBS_IN_MARKET (MAX_JOBS_ON_RAIL*3)
#define MARKET_JOBS_SEARCH_TRIGGER (MAX_JOBS_IN_MARKET/5)
#define MARKET_ANIM_SPEED 750
#define EMPLOYER_RANGE 10
#define WORKING_POP_PERCENT 45
#define JOB_SWING 15
#define HC_JOB_SWING 10
#define CRICKET_JOB_SWING 4

#define ORGANIC_FARM_FOOD_OUTPUT 550
#define ORGANIC_FARM_ANIM_SPEED 3000
#define FARM_JOBS_USED 13
#define ORG_FARM_RANGE 10
#define ORG_FARM_POWER_REC 50
/* gets waste only when powered */
#define ORG_FARM_WASTE_GET 6
#define MIN_FOOD_SOLD_FOR_ANIM 200
#define DAYS_PER_STARVE 20

#define NUMOF_COAL_RESERVES 100
#define COAL_RESERVE_SIZE 10000
#define ORE_RESERVE       1000
#define MIN_ORE_RESERVE_FOR_MINE (ORE_RESERVE)
#define MAX_COAL_AT_MINE 100000
#define MAX_ORE_AT_MINE 100000
/* COAL_RESERVE_SEARCH_LEN acts in both directions, so 5 is 10*10 */
#define COAL_RESERVE_SEARCH_LEN 8
#define JOBS_DIG_COAL 900
#define JOBS_DIG_ORE  200
#define JOBS_COALPS_GENERATE 100
#define MAX_JOBS_AT_COALPS 2000
#define JOBS_LOAD_COAL 18
#define JOBS_LOAD_ORE  9
#define JOBS_LOAD_STEEL 15
#define JOBS_AT_COMMUNE_GATE 4
#define COMMUNE_ANIM_SPEED 750
#define COMMUNE_POP  5

#define MAX_COAL_ON_TRACK 64
#define MAX_COAL_ON_ROAD (MAX_COAL_ON_TRACK*8)
#define MAX_COAL_ON_RAIL (MAX_COAL_ON_TRACK*64)
#define MAX_COAL_IN_MARKET (MAX_COAL_ON_RAIL*2)
#define MARKET_COAL_SEARCH_TRIGGER (MAX_COAL_IN_MARKET/5)

#define MAX_GOODS_ON_TRACK 2048
#define MAX_GOODS_ON_ROAD (MAX_GOODS_ON_TRACK*5)
#define ROAD_GOODS_USED_MASK 0x1f
#define MAX_GOODS_ON_RAIL (MAX_GOODS_ON_TRACK*25)
#define RAIL_GOODS_USED_MASK 0xf
#define MAX_GOODS_IN_MARKET (MAX_GOODS_ON_RAIL*4)
#define MARKET_GOODS_SEARCH_TRIGGER (MAX_GOODS_IN_MARKET/5)
#define MAX_ORE_ON_TRACK 4096
#define MAX_ORE_ON_ROAD (MAX_ORE_ON_TRACK*4)
#define MAX_ORE_ON_RAIL (MAX_ORE_ON_TRACK*16)
#define MAX_ORE_IN_MARKET (MAX_ORE_ON_RAIL*2)
#define MARKET_ORE_SEARCH_TRIGGER (MAX_ORE_IN_MARKET/5)

#define MAX_STEEL_ON_TRACK 128
#define MAX_STEEL_ON_ROAD (MAX_STEEL_ON_TRACK*4)
#define MAX_STEEL_ON_RAIL (MAX_STEEL_ON_TRACK*16)
#define RAIL_STEEL_USED_MASK 0x7f
#define MAX_STEEL_AT_INDUSTRY_H (MAX_STEEL_ON_RAIL*10)

#define MAX_WASTE_ON_TRACK 1024
#define MAX_WASTE_ON_ROAD (MAX_WASTE_ON_TRACK*5)
#define MAX_WASTE_ON_RAIL (MAX_WASTE_ON_TRACK*25)
#define MAX_WASTE_IN_MARKET (MAX_WASTE_ON_RAIL*3)
#define MARKET_WASTE_SEARCH_TRIGGER (MAX_WASTE_IN_MARKET/5)
#define MAX_WASTE_AT_TIP  10000000
#define WASTE_BURN_ON_TRANSPORT 20
#define TRANSPORT_BURN_WASTE_COUNT 75000

#define WATERWELL_RANGE 20

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
#define MARKET_STEEL_SEARCH_TRIGGER (MAX_STEEL_IN_MARKET/5)

#define SUST_ORE_COAL_COL 34
#define SUST_PORT_COL     white(24)
#define SUST_MONEY_COL    green(26)
#define SUST_POP_COL      cyan(24)
#define SUST_TECH_COL     yellow(26)
#define SUST_FIRE_COL     red(26)
#define SUST_ORE_COAL_YEARS_NEEDED 180
#define SUST_PORT_YEARS_NEEDED     180
#define SUST_MONEY_YEARS_NEEDED    60
#define SUST_POP_YEARS_NEEDED      60
#define SUST_TECH_YEARS_NEEDED     120
#define SUST_FIRE_YEARS_NEEDED     60

#define SUST_MIN_POPULATION 5000
#define SUST_MIN_TECH_LEVEL (MAX_TECH_LEVEL/2)

#define STATS_X 232
#define STATS_Y 428
#define STATS_W 304
#define STATS_H 32

#define GRAPHS_B_COLOUR (white(6))

//#define MARKET_CB_W (18*8)
#define MARKET_CB_W (17*8 - 2)
#define MARKET_CB_H (23*8)

#define SHUFFLE_MAPPOINT_COUNT 4

/******* Buildings // GROUPS ************/ 
#define NUM_OF_TYPES    404
#define NUM_OF_GROUPS    50
#define GROUP_NAME_LEN   20

#define GROUP_BARE 	   0
#define GROUP_BARE_COLOUR  (green(12))
#define GROUP_BARE_COST    0
#define GROUP_BARE_COST_MUL 1
#define GROUP_BARE_BUL_COST 1
#define GROUP_BARE_TECH    0
#define GROUP_BARE_FIREC   0

#define GROUP_POWER_LINE   1
#define GROUP_POWER_LINE_COLOUR (yellow(26))
#define GROUP_POWER_LINE_COST 100
#define GROUP_POWER_LINE_COST_MUL 2
#define GROUP_POWER_LINE_BUL_COST 100
#define GROUP_POWER_LINE_TECH 200
#define GROUP_POWER_LINE_FIREC 0

#define GROUP_SOLAR_POWER 2
#define GROUP_SOLAR_POWER_COLOUR (yellow(22))
#define GROUP_SOLAR_POWER_COST 500000
#define GROUP_SOLAR_POWER_COST_MUL 5
#define GROUP_SOLAR_POWER_BUL_COST 100000
#define GROUP_SOLAR_POWER_TECH 500
#define GROUP_SOLAR_POWER_FIREC 33
#define SOLAR_POWER_JOBS 50
#define POWERS_SOLAR_OUTPUT 1800

#define GROUP_SUBSTATION   3
#define GROUP_SUBSTATION_COLOUR (yellow(18))
#define GROUP_SUBSTATION_COST 500
#define GROUP_SUBSTATION_COST_MUL 2
#define GROUP_SUBSTATION_BUL_COST 100
#define GROUP_SUBSTATION_TECH 200
#define GROUP_SUBSTATION_FIREC 50

#define GROUP_RESIDENCE_LL 4
#define GROUP_RESIDENCE_LL_COLOUR (cyan(24))
#define GROUP_RESIDENCE_LL_COST 1000
#define GROUP_RESIDENCE_LL_COST_MUL 25
#define GROUP_RESIDENCE_LL_BUL_COST 1000
#define GROUP_RESIDENCE_LL_TECH 0
#define GROUP_RESIDENCE_LL_FIREC 75

/* RESIDENCE?_BRM is the birth rate modifier */
/* RESIDENCE?_DRM is the death rate modifier */
#define RESIDENCE_BASE_BR     100
#define RESIDENCE_BASE_DR     (68*12)
#define RESIDENCE_BRM_HEALTH  5
#define RESIDENCE1_BRM (RESIDENCE_BASE_BR*12)
#define RESIDENCE1_DRM ((RESIDENCE_BASE_DR*3)/4)
#define RESIDENCE2_BRM 0
#define RESIDENCE2_DRM 0
#define RESIDENCE3_BRM (RESIDENCE_BASE_BR+RESIDENCE_BASE_BR/4)
#define RESIDENCE3_DRM ((RESIDENCE_BASE_DR)/2)
#define RESIDENCE4_BRM (RESIDENCE_BASE_BR*18)
#define RESIDENCE4_DRM ((RESIDENCE_BASE_DR*3)/4)
#define RESIDENCE5_BRM (RESIDENCE_BASE_BR/2)
#define RESIDENCE5_DRM ((RESIDENCE_BASE_DR)/4)
#define RESIDENCE6_BRM 0
#define RESIDENCE6_DRM ((RESIDENCE_BASE_DR)/4)
/* RESIDENCE_PPM is the people_pool mobitily. Higher number=less mobile. */
#define RESIDENCE_PPM   20

#define GROUP_ORGANIC_FARM 5
#define GROUP_ORGANIC_FARM_COLOUR (green(30))
#define GROUP_ORGANIC_FARM_COST 1000
#define GROUP_ORGANIC_FARM_COST_MUL 20
#define GROUP_ORGANIC_FARM_BUL_COST 100
#define GROUP_ORGANIC_FARM_TECH 0
#define GROUP_ORGANIC_FARM_FIREC 20

#define GROUP_MARKET       6
#define GROUP_MARKET_COLOUR (blue(28))
#define GROUP_MARKET_COST 100
#define GROUP_MARKET_COST_MUL 25
#define GROUP_MARKET_BUL_COST 100
#define GROUP_MARKET_TECH 0
#define GROUP_MARKET_FIREC 80

#define GROUP_TRACK	   7
#define GROUP_TRACK_COLOUR 32   /* 32 is a brown */
#define GROUP_TRACK_COST   1
#define GROUP_TRACK_COST_MUL 25
#define GROUP_TRACK_BUL_COST   10
#define GROUP_TRACK_TECH   0
#define GROUP_TRACK_FIREC  4

#define GROUP_COALMINE     8
#define GROUP_COALMINE_COLOUR 0
#define GROUP_COALMINE_COST   10000
#define GROUP_COALMINE_COST_MUL 25
#define GROUP_COALMINE_BUL_COST   10000
#define GROUP_COALMINE_TECH   85
#define GROUP_COALMINE_FIREC  85

#define GROUP_RAIL         9
#define GROUP_RAIL_COLOUR  (magenta(18))
#define GROUP_RAIL_COST    500
#define GROUP_RAIL_COST_MUL 10
#define GROUP_RAIL_BUL_COST    1000
#define GROUP_RAIL_TECH    180
#define GROUP_RAIL_FIREC   6

#define GROUP_COAL_POWER 10
#define GROUP_COAL_POWER_COLOUR 0
#define GROUP_COAL_POWER_COST   100000
#define GROUP_COAL_POWER_COST_MUL 5
#define GROUP_COAL_POWER_BUL_COST   200000
#define GROUP_COAL_POWER_TECH   200
#define GROUP_COAL_POWER_FIREC  80

#define GROUP_ROAD         11
#define GROUP_ROAD_COLOUR  (white(18))
#define GROUP_ROAD_COST    100
#define GROUP_ROAD_COST_MUL 25
#define GROUP_ROAD_BUL_COST    50
#define GROUP_ROAD_TECH    50
#define GROUP_ROAD_FIREC   4

#define GROUP_INDUSTRY_L   12
#define GROUP_INDUSTRY_L_COLOUR (cyan(18))
#define GROUP_INDUSTRY_L_COST 20000
#define GROUP_INDUSTRY_L_COST_MUL 25
#define GROUP_INDUSTRY_L_BUL_COST 20000
#define GROUP_INDUSTRY_L_TECH 160
#define GROUP_INDUSTRY_L_FIREC 70

#define GROUP_UNIVERSITY   13
#define GROUP_UNIVERSITY_COLOUR (blue(22))
#define GROUP_UNIVERSITY_COST 20000
#define GROUP_UNIVERSITY_COST_MUL 25
#define GROUP_UNIVERSITY_BUL_COST 20000
#define GROUP_UNIVERSITY_TECH 150
#define GROUP_UNIVERSITY_FIREC 40

#define GROUP_COMMUNE      14
#define GROUP_COMMUNE_COLOUR (green(30))
#define GROUP_COMMUNE_COST  1
#define GROUP_COMMUNE_COST_MUL 2
#define GROUP_COMMUNE_BUL_COST  1000
#define GROUP_COMMUNE_TECH  0
#define GROUP_COMMUNE_FIREC 30

#define GROUP_OREMINE      15
#define GROUP_OREMINE_COLOUR (red(18))
#define GROUP_OREMINE_COST 500
#define GROUP_OREMINE_COST_MUL 10
#define GROUP_OREMINE_BUL_COST 500000
#define GROUP_OREMINE_TECH 0
#define GROUP_OREMINE_FIREC 0

#define GROUP_TIP      16
#define GROUP_TIP_COLOUR (white(16))
#define GROUP_TIP_COST 10000
#define GROUP_TIP_COST_MUL 25
#define GROUP_TIP_BUL_COST 1000000
#define GROUP_TIP_TECH 0
#define GROUP_TIP_FIREC 50

#define GROUP_PORT      17
#define GROUP_PORT_COLOUR (blue(28))
#define GROUP_PORT_COST 100000
#define GROUP_PORT_COST_MUL 2
#define GROUP_PORT_BUL_COST 1000
#define GROUP_PORT_TECH 35
#define GROUP_PORT_FIREC 50

#define GROUP_INDUSTRY_H   18
#define GROUP_INDUSTRY_H_COLOUR (7)     /* colour 7 is an orange, sort of */
#define GROUP_INDUSTRY_H_COST 50000
#define GROUP_INDUSTRY_H_COST_MUL 20
#define GROUP_INDUSTRY_H_BUL_COST 70000
#define GROUP_INDUSTRY_H_TECH 170
#define GROUP_INDUSTRY_H_FIREC 80

#define GROUP_PARKLAND     19
#define GROUP_PARKLAND_COLOUR (green(31))
#define GROUP_PARKLAND_COST   1000
#define GROUP_PARKLAND_COST_MUL 25
#define GROUP_PARKLAND_BUL_COST   1000
#define GROUP_PARKLAND_TECH   2
#define GROUP_PARKLAND_FIREC 1

#define GROUP_RECYCLE      20
#define GROUP_RECYCLE_COLOUR   (green(28))
#define GROUP_RECYCLE_COST    100000
#define GROUP_RECYCLE_COST_MUL 5
#define GROUP_RECYCLE_BUL_COST    1000
#define GROUP_RECYCLE_TECH    232
#define GROUP_RECYCLE_FIREC 10

#define GROUP_WATER	   21
#define GROUP_WATER_COLOUR    (blue(31))
#define GROUP_WATER_COST      1000000
#define GROUP_WATER_COST_MUL 2
#define GROUP_WATER_BUL_COST      1000000
#define GROUP_WATER_TECH      0
#define GROUP_RIVER (GROUP_WATER)       /* oops -- GCS: Why oops? */
#define GROUP_WATER_FIREC 0

#define GROUP_HEALTH       22
#define GROUP_HEALTH_COLOUR   (green(24))
#define GROUP_HEALTH_COST     100000
#define GROUP_HEALTH_COST_MUL 2
#define GROUP_HEALTH_BUL_COST     1000
#define GROUP_HEALTH_TECH     110
#define GROUP_HEALTH_FIREC 2

#define GROUP_ROCKET	   23
#define GROUP_ROCKET_COLOUR   (magenta(20))
#define GROUP_ROCKET_COST     700000
#define GROUP_ROCKET_COST_MUL 2
#define GROUP_ROCKET_BUL_COST     1
#define GROUP_ROCKET_TECH     750
#define GROUP_ROCKET_FIREC 0

#define GROUP_WINDMILL     24
#define GROUP_WINDMILL_COLOUR (green(25))
#define GROUP_WINDMILL_COST   20000
#define GROUP_WINDMILL_COST_MUL 25
#define GROUP_WINDMILL_BUL_COST   1000
#define GROUP_WINDMILL_TECH   30
#define GROUP_WINDMILL_FIREC  10

#define GROUP_MONUMENT     25
#define GROUP_MONUMENT_COLOUR (white(15))
#define GROUP_MONUMENT_COST   10000
#define GROUP_MONUMENT_COST_MUL 25
#define GROUP_MONUMENT_BUL_COST   1000000
#define GROUP_MONUMENT_TECH   0
#define GROUP_MONUMENT_FIREC  0

#define GROUP_SCHOOL     26
#define GROUP_SCHOOL_COLOUR (white(15))
#define GROUP_SCHOOL_COST   10000
#define GROUP_SCHOOL_COST_MUL 25
#define GROUP_SCHOOL_BUL_COST   10000
#define GROUP_SCHOOL_TECH   1
#define GROUP_SCHOOL_FIREC 40

#define GROUP_BLACKSMITH     27
#define GROUP_BLACKSMITH_COLOUR (white(15))
#define GROUP_BLACKSMITH_COST   5000
#define GROUP_BLACKSMITH_COST_MUL 25
#define GROUP_BLACKSMITH_BUL_COST   1000
#define GROUP_BLACKSMITH_TECH   3
#define GROUP_BLACKSMITH_FIREC 60

#define GROUP_MILL     28
#define GROUP_MILL_COLOUR (white(15))
#define GROUP_MILL_COST   10000
#define GROUP_MILL_COST_MUL 25
#define GROUP_MILL_BUL_COST   1000
#define GROUP_MILL_TECH   25
#define GROUP_MILL_FIREC  60

#define GROUP_POTTERY  29
#define GROUP_POTTERY_COLOUR (red(12))
#define GROUP_POTTERY_COST   1000
#define GROUP_POTTERY_COST_MUL 25
#define GROUP_POTTERY_BUL_COST 1000
#define GROUP_POTTERY_TECH  0
#define GROUP_POTTERY_FIREC 50

#define GROUP_FIRESTATION 30
#define GROUP_FIRESTATION_COLOUR (red(20))
#define GROUP_FIRESTATION_COST   20000
#define GROUP_FIRESTATION_COST_MUL 10
#define GROUP_FIRESTATION_BUL_COST 1000
#define GROUP_FIRESTATION_TECH     22
#define GROUP_FIRESTATION_FIREC 0

#define GROUP_CRICKET 31
#define GROUP_CRICKET_COLOUR (white(20))
#define GROUP_CRICKET_COST   2000
#define GROUP_CRICKET_COST_MUL 3
#define GROUP_CRICKET_BUL_COST 1000
#define GROUP_CRICKET_TECH     12
#define GROUP_CRICKET_FIREC 20

#define GROUP_BURNT        32
#define GROUP_BURNT_COLOUR    (red(30))
#define GROUP_BURNT_COST   0    /* Unused */
#define GROUP_BURNT_COST_MUL   1        /* Unused */
#define GROUP_BURNT_BUL_COST  1000
#define GROUP_BURNT_TECH   0    /* Unused */
#define GROUP_BURNT_FIREC  0

#define GROUP_SHANTY        33
#define GROUP_SHANTY_COLOUR    (red(22))
#define GROUP_SHANTY_COST   0   /* Unused */
#define GROUP_SHANTY_COST_MUL   1       /* Unused */
#define GROUP_SHANTY_BUL_COST  100000
#define GROUP_SHANTY_TECH   0   /* Unused */
#define GROUP_SHANTY_FIREC     25

#define SHANTY_MIN_PP     150
#define SHANTY_POP        50
#define DAYS_BETWEEN_SHANTY (NUMOF_DAYS_IN_MONTH*5)
#define SHANTY_GET_FOOD   50
#define SHANTY_GET_JOBS   5
#define SHANTY_GET_GOODS  50
#define SHANTY_GET_COAL   1
#define SHANTY_GET_ORE    10
#define SHANTY_GET_STEEL  1

#define GROUP_FIRE        34
#define GROUP_FIRE_COLOUR    (red(30))
#define GROUP_FIRE_COST   0     /* Unused */
#define GROUP_FIRE_COST_MUL   1 /* Unused */
#define GROUP_FIRE_BUL_COST  1000
#define GROUP_FIRE_TECH   0     /* Unused */
#define GROUP_FIRE_FIREC  0

#define GROUP_USED        35
#define GROUP_USED_COLOUR (green(12))   /* Unused */
#define GROUP_USED_COST   0     /* Unused */
#define GROUP_USED_COST_MUL  1  /* Unused */
#define GROUP_USED_BUL_COST  0  /* Unused */
#define GROUP_USED_TECH   0     /* Unused */
#define GROUP_USED_FIREC  0     /* Unused */

#define GROUP_RESIDENCE_ML 36
#define GROUP_RESIDENCE_ML_COLOUR (cyan(24))
#define GROUP_RESIDENCE_ML_COST 2000
#define GROUP_RESIDENCE_ML_COST_MUL 25
#define GROUP_RESIDENCE_ML_BUL_COST 1000
#define GROUP_RESIDENCE_ML_TECH 0
#define GROUP_RESIDENCE_ML_FIREC 75

#define GROUP_RESIDENCE_HL 37
#define GROUP_RESIDENCE_HL_COLOUR (cyan(24))
#define GROUP_RESIDENCE_HL_COST 4000
#define GROUP_RESIDENCE_HL_COST_MUL 25
#define GROUP_RESIDENCE_HL_BUL_COST 1000
#define GROUP_RESIDENCE_HL_TECH 0
#define GROUP_RESIDENCE_HL_FIREC 75

#define GROUP_RESIDENCE_LH 38
#define GROUP_RESIDENCE_LH_COLOUR (cyan(24))
#define GROUP_RESIDENCE_LH_COST 800
#define GROUP_RESIDENCE_LH_COST_MUL 25
#define GROUP_RESIDENCE_LH_BUL_COST 1000
#define GROUP_RESIDENCE_LH_TECH 0       /* ?? */
#define GROUP_RESIDENCE_LH_FIREC 75

#define GROUP_RESIDENCE_MH 39
#define GROUP_RESIDENCE_MH_COLOUR (cyan(24))
#define GROUP_RESIDENCE_MH_COST 1600
#define GROUP_RESIDENCE_MH_COST_MUL 25
#define GROUP_RESIDENCE_MH_BUL_COST 1000
#define GROUP_RESIDENCE_MH_TECH 0
#define GROUP_RESIDENCE_MH_FIREC 75

#define GROUP_RESIDENCE_HH 40
#define GROUP_RESIDENCE_HH_COLOUR (cyan(24))
#define GROUP_RESIDENCE_HH_COST 3200
#define GROUP_RESIDENCE_HH_COST_MUL 25
#define GROUP_RESIDENCE_HH_BUL_COST 1000
#define GROUP_RESIDENCE_HH_TECH 0
#define GROUP_RESIDENCE_HH_FIREC 75

#define GROUP_WATERWELL	   41
#define GROUP_WATERWELL_COLOUR    (blue(31))
#define GROUP_WATERWELL_COST      1
#define GROUP_WATERWELL_COST_MUL 2
#define GROUP_WATERWELL_BUL_COST      1
#define GROUP_WATERWELL_TECH      0
#define GROUP_WATERWELL_FIREC 0

#define GROUP_DESERT 	  42
#define GROUP_DESERT_COLOUR  (yellow(18))
#define GROUP_DESERT_COST    0
#define GROUP_DESERT_COST_MUL 1
#define GROUP_DESERT_BUL_COST 1
#define GROUP_DESERT_TECH    0
#define GROUP_DESERT_FIREC   0

#define GROUP_TREE 	   43
#define GROUP_TREE_COLOUR  (green(12))
#define GROUP_TREE_COST    0
#define GROUP_TREE_COST_MUL 1
#define GROUP_TREE_BUL_COST 1
#define GROUP_TREE_TECH    0
#define GROUP_TREE_FIREC   0

#define GROUP_TREE2 	   44
#define GROUP_TREE2_COLOUR  (green(12))
#define GROUP_TREE2_COST    0
#define GROUP_TREE2_COST_MUL 1
#define GROUP_TREE2_BUL_COST 1
#define GROUP_TREE2_TECH    0
#define GROUP_TREE2_FIREC   0

#define GROUP_TREE3 	   45
#define GROUP_TREE3_COLOUR  (green(12))
#define GROUP_TREE3_COST    0
#define GROUP_TREE3_COST_MUL 1
#define GROUP_TREE3_BUL_COST 1
#define GROUP_TREE3_TECH    0
#define GROUP_TREE3_FIREC   0

#define GROUP_TRACK_BRIDGE        46
#define GROUP_TRACK_BRIDGE_COLOUR 32   /* 32 is a brown */
#define GROUP_TRACK_BRIDGE_COST   200000
#define GROUP_TRACK_BRIDGE_COST_MUL 2  /* 600K per section on water at max level */
#define GROUP_TRACK_BRIDGE_BUL_COST 1000000 /* like bulldoze on water */
#define GROUP_TRACK_BRIDGE_TECH   0
#define GROUP_TRACK_BRIDGE_FIREC  0    /* unused */

#define GROUP_ROAD_BRIDGE         47
#define GROUP_ROAD_BRIDGE_COLOUR  (white(18))
#define GROUP_ROAD_BRIDGE_COST    500000
#define GROUP_ROAD_BRIDGE_COST_MUL 2  /* 1.5M per section on water at max level */
#define GROUP_ROAD_BRIDGE_BUL_COST 1000000
#define GROUP_ROAD_BRIDGE_TECH    50
#define GROUP_ROAD_BRIDGE_FIREC   0

#define GROUP_RAIL_BRIDGE         48
#define GROUP_RAIL_BRIDGE_COLOUR  (magenta(18))
#define GROUP_RAIL_BRIDGE_COST    1000000
#define GROUP_RAIL_BRIDGE_COST_MUL 2  /* 3M per section on water at max level */
#define GROUP_RAIL_BRIDGE_BUL_COST 1000000
#define GROUP_RAIL_BRIDGE_TECH    180
#define GROUP_RAIL_BRIDGE_FIREC   0

#define GROUP_IS_RESIDENCE(group) \
            ((group == GROUP_RESIDENCE_LL) || \
             (group == GROUP_RESIDENCE_ML) || \
             (group == GROUP_RESIDENCE_HL) || \
             (group == GROUP_RESIDENCE_LH) || \
             (group == GROUP_RESIDENCE_MH) || \
             (group == GROUP_RESIDENCE_HH))

#define GROUP_IS_BARE(group) \
	    ((group == GROUP_BARE) || \
	     (group == GROUP_DESERT) || \
	     (group == GROUP_TREE) || \
	     (group == GROUP_TREE2) || \
	     (group == GROUP_TREE3))

#define red(x) (32 + x)
#define green(x) (64 + x)
#define yellow(x) (96 + x)
#define blue(x) (128 + x)
#define magenta(x) (160 + x)
#define cyan(x) (192 + x)
#define white(x) (224 + x)

/*
  *******   end of lin-city.h   ***********
  *****************************************
*/

/* FIXME : isn't it forbidden to include .h in .h ?  :-) */
#include "lintypes.h"

#endif /* __lin_city_h__ */
