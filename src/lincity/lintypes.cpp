/* ---------------------------------------------------------------------- *
 * lintypes.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#include <string.h>             /* XXX: portability issue?  for strcpy */
#include "lcconfig.h"
#include "lin-city.h"
#include "engglobs.h"
#include "lctypes.h"
#include "lintypes.h"
#include "tinygettext/gettext.hpp"
#include "fileutil.h"
#include "gui_interface/readpng.h"
#include "loadsave.h"

struct TYPE main_types[NUM_OF_TYPES];

struct GROUP main_groups[NUM_OF_GROUPS] = {
    /* 0 */
    {N_("Empty"),               /* name */
     FALSE,                     /* need credit? */
     GROUP_BARE,                /* group number */
     1,                         /* size */
     (green(12)),               /* color */
     1,                         /* cost multiplier */
     1,                         /* bulldoze cost */
     0,                         /* probability of fire */
     0,                         /* cost */
     0                          /* tech */
     },
    /* 1 */
    {N_("Power line"),
     FALSE,                     /* need credit? */
     GROUP_POWER_LINE,
     1,                         /* size */
     GROUP_POWER_LINE_COLOUR,
     GROUP_POWER_LINE_COST_MUL,
     GROUP_POWER_LINE_BUL_COST,
     GROUP_POWER_LINE_FIREC,
     GROUP_POWER_LINE_COST,
     GROUP_POWER_LINE_TECH},
    /* 2 */
    {N_("Solar PS"),
     TRUE,                      /* need credit? */
     GROUP_SOLAR_POWER,
     4,                         /* size */
     GROUP_SOLAR_POWER_COLOUR,
     GROUP_SOLAR_POWER_COST_MUL,
     GROUP_SOLAR_POWER_BUL_COST,
     GROUP_SOLAR_POWER_FIREC,
     GROUP_SOLAR_POWER_COST,
     GROUP_SOLAR_POWER_TECH},
    /* 3 */
    {N_("Substation"),
     FALSE,                     /* need credit? */
     GROUP_SUBSTATION,
     2,                         /* size */
     GROUP_SUBSTATION_COLOUR,
     GROUP_SUBSTATION_COST_MUL,
     GROUP_SUBSTATION_BUL_COST,
     GROUP_SUBSTATION_FIREC,
     GROUP_SUBSTATION_COST,
     GROUP_SUBSTATION_TECH},
    /* 4 */
    {N_("Residential LL"),
     FALSE,                     /* need credit? */
     GROUP_RESIDENCE_LL,
     3,                         /* size */
     GROUP_RESIDENCE_LL_COLOUR,
     GROUP_RESIDENCE_LL_COST_MUL,
     GROUP_RESIDENCE_LL_BUL_COST,
     GROUP_RESIDENCE_LL_FIREC,
     GROUP_RESIDENCE_LL_COST,
     GROUP_RESIDENCE_LL_TECH},
    /* 5 */
    {N_("Farm"),
     FALSE,                     /* need credit? */
     GROUP_ORGANIC_FARM,
     4,                         /* size */
     GROUP_ORGANIC_FARM_COLOUR,
     GROUP_ORGANIC_FARM_COST_MUL,
     GROUP_ORGANIC_FARM_BUL_COST,
     GROUP_ORGANIC_FARM_FIREC,
     GROUP_ORGANIC_FARM_COST,
     GROUP_ORGANIC_FARM_TECH},
    /* 6 */
    {N_("Market"),
     FALSE,                     /* need credit? */
     GROUP_MARKET,
     2,                         /* size */
     GROUP_MARKET_COLOUR,
     GROUP_MARKET_COST_MUL,
     GROUP_MARKET_BUL_COST,
     GROUP_MARKET_FIREC,
     GROUP_MARKET_COST,
     GROUP_MARKET_TECH},
    /* 7 */
    {N_("Track"),
     FALSE,                     /* need credit? */
     GROUP_TRACK,
     1,                         /* size */
     GROUP_TRACK_COLOUR,
     GROUP_TRACK_COST_MUL,
     GROUP_TRACK_BUL_COST,
     GROUP_TRACK_FIREC,
     GROUP_TRACK_COST,
     GROUP_TRACK_TECH},
    /* 8 */
    {N_("Coalmine"),
     FALSE,                     /* need credit? */
     GROUP_COALMINE,
     4,                         /* size */
     GROUP_COALMINE_COLOUR,
     GROUP_COALMINE_COST_MUL,
     GROUP_COALMINE_BUL_COST,
     GROUP_COALMINE_FIREC,
     GROUP_COALMINE_COST,
     GROUP_COALMINE_TECH},
    /* 9 */
    {N_("Railway"),
     FALSE,                     /* need credit? */
     GROUP_RAIL,
     1,                         /* size */
     GROUP_RAIL_COLOUR,
     GROUP_RAIL_COST_MUL,
     GROUP_RAIL_BUL_COST,
     GROUP_RAIL_FIREC,
     GROUP_RAIL_COST,
     GROUP_RAIL_TECH},
    /* 10 */
    {N_("Coal PS"),
     FALSE,                     /* need credit? */
     GROUP_COAL_POWER,
     4,                         /* size */
     GROUP_COAL_POWER_COLOUR,
     GROUP_COAL_POWER_COST_MUL,
     GROUP_COAL_POWER_BUL_COST,
     GROUP_COAL_POWER_FIREC,
     GROUP_COAL_POWER_COST,
     GROUP_COAL_POWER_TECH},
    /* 11 */
    {N_("Road"),
     FALSE,                     /* need credit? */
     GROUP_ROAD,
     1,                         /* size */
     GROUP_ROAD_COLOUR,
     GROUP_ROAD_COST_MUL,
     GROUP_ROAD_BUL_COST,
     GROUP_ROAD_FIREC,
     GROUP_ROAD_COST,
     GROUP_ROAD_TECH},
    /* 12 */
    {N_("Lt. Industry"),
     FALSE,                     /* need credit? */
     GROUP_INDUSTRY_L,
     3,                         /* size */
     GROUP_INDUSTRY_L_COLOUR,
     GROUP_INDUSTRY_L_COST_MUL,
     GROUP_INDUSTRY_L_BUL_COST,
     GROUP_INDUSTRY_L_FIREC,
     GROUP_INDUSTRY_L_COST,
     GROUP_INDUSTRY_L_TECH},
    /* 13 */
    {N_("University"),
     FALSE,                     /* need credit? */
     GROUP_UNIVERSITY,
     3,                         /* size */
     GROUP_UNIVERSITY_COLOUR,
     GROUP_UNIVERSITY_COST_MUL,
     GROUP_UNIVERSITY_BUL_COST,
     GROUP_UNIVERSITY_FIREC,
     GROUP_UNIVERSITY_COST,
     GROUP_UNIVERSITY_TECH},
    /* 14 */
    {N_("Commune"),
     FALSE,                     /* need credit? */
     GROUP_COMMUNE,
     4,                         /* size */
     GROUP_COMMUNE_COLOUR,
     GROUP_COMMUNE_COST_MUL,
     GROUP_COMMUNE_BUL_COST,
     GROUP_COMMUNE_FIREC,
     GROUP_COMMUNE_COST,
     GROUP_COMMUNE_TECH},
    /* 15 */
    {N_("Ore mine"),
     FALSE,                     /* need credit? */
     GROUP_OREMINE,
     4,                         /* size */
     GROUP_OREMINE_COLOUR,
     GROUP_OREMINE_COST_MUL,
     GROUP_OREMINE_BUL_COST,
     GROUP_OREMINE_FIREC,
     GROUP_OREMINE_COST,
     GROUP_OREMINE_TECH},
    /* 16 */
    {N_("Rubbish tip"),
     FALSE,                     /* need credit? */
     GROUP_TIP,
     4,                         /* size */
     GROUP_TIP_COLOUR,
     GROUP_TIP_COST_MUL,
     GROUP_TIP_BUL_COST,
     GROUP_TIP_FIREC,
     GROUP_TIP_COST,
     GROUP_TIP_TECH},
    /* 17 */
    {N_("Port"),
     FALSE,                     /* need credit? */
     GROUP_PORT,
     4,                         /* size */
     GROUP_PORT_COLOUR,
     GROUP_PORT_COST_MUL,
     GROUP_PORT_BUL_COST,
     GROUP_PORT_FIREC,
     GROUP_PORT_COST,
     GROUP_PORT_TECH},
    /* 18 */
    {N_("Hv. Industry"),
     FALSE,                     /* need credit? */
     GROUP_INDUSTRY_H,
     4,                         /* size */
     GROUP_INDUSTRY_H_COLOUR,
     GROUP_INDUSTRY_H_COST_MUL,
     GROUP_INDUSTRY_H_BUL_COST,
     GROUP_INDUSTRY_H_FIREC,
     GROUP_INDUSTRY_H_COST,
     GROUP_INDUSTRY_H_TECH},
    /* 19 */
    {N_("Park"),
     TRUE,                      /* need credit? */
     GROUP_PARKLAND,
     1,                         /* size */
     GROUP_PARKLAND_COLOUR,
     GROUP_PARKLAND_COST_MUL,
     GROUP_PARKLAND_BUL_COST,
     GROUP_PARKLAND_FIREC,
     GROUP_PARKLAND_COST,
     GROUP_PARKLAND_TECH},
    /* 20 */
    {N_("Recycle"),
     TRUE,                      /* need credit? */
     GROUP_RECYCLE,
     2,                         /* size */
     GROUP_RECYCLE_COLOUR,
     GROUP_RECYCLE_COST_MUL,
     GROUP_RECYCLE_BUL_COST,
     GROUP_RECYCLE_FIREC,
     GROUP_RECYCLE_COST,
     GROUP_RECYCLE_TECH},
    /* 21 */
    {N_("Water"),
     FALSE,                     /* need credit? */
     GROUP_WATER,
     1,                         /* size */
     GROUP_WATER_COLOUR,
     GROUP_WATER_COST_MUL,
     GROUP_WATER_BUL_COST,
     GROUP_WATER_FIREC,
     GROUP_WATER_COST,
     GROUP_WATER_TECH},
    /* 22 */
    {N_("Health centre"),
     FALSE,                     /* need credit? */
     GROUP_HEALTH,
     2,                         /* size */
     GROUP_HEALTH_COLOUR,
     GROUP_HEALTH_COST_MUL,
     GROUP_HEALTH_BUL_COST,
     GROUP_HEALTH_FIREC,
     GROUP_HEALTH_COST,
     GROUP_HEALTH_TECH},
    /* 23 */
    {N_("Rocket pad"),
     TRUE,                      /* need credit? */
     GROUP_ROCKET,
     4,                         /* size */
     GROUP_ROCKET_COLOUR,
     GROUP_ROCKET_COST_MUL,
     GROUP_ROCKET_BUL_COST,
     GROUP_ROCKET_FIREC,
     GROUP_ROCKET_COST,
     GROUP_ROCKET_TECH},
    /* 24 */
    {N_("Windmill"),
     FALSE,                     /* need credit? */
     GROUP_WINDMILL,
     2,                         /* size */
     GROUP_WINDMILL_COLOUR,
     GROUP_WINDMILL_COST_MUL,
     GROUP_WINDMILL_BUL_COST,
     GROUP_WINDMILL_FIREC,
     GROUP_WINDMILL_COST,
     GROUP_WINDMILL_TECH},
    /* 25 */
    {N_("Monument"),
     FALSE,                     /* need credit? */
     GROUP_MONUMENT,
     2,                         /* size */
     GROUP_MONUMENT_COLOUR,
     GROUP_MONUMENT_COST_MUL,
     GROUP_MONUMENT_BUL_COST,
     GROUP_MONUMENT_FIREC,
     GROUP_MONUMENT_COST,
     GROUP_MONUMENT_TECH},
    /* 26 */
    {N_("School"),
     FALSE,                     /* need credit? */
     GROUP_SCHOOL,
     2,                         /* size */
     GROUP_SCHOOL_COLOUR,
     GROUP_SCHOOL_COST_MUL,
     GROUP_SCHOOL_BUL_COST,
     GROUP_SCHOOL_FIREC,
     GROUP_SCHOOL_COST,
     GROUP_SCHOOL_TECH},
    /* 27 */
    {N_("Blacksmith"),
     FALSE,                     /* need credit? */
     GROUP_BLACKSMITH,
     2,                         /* size */
     GROUP_BLACKSMITH_COLOUR,
     GROUP_BLACKSMITH_COST_MUL,
     GROUP_BLACKSMITH_BUL_COST,
     GROUP_BLACKSMITH_FIREC,
     GROUP_BLACKSMITH_COST,
     GROUP_BLACKSMITH_TECH},
    /* 28 */
    {N_("Mill"),
     FALSE,                     /* need credit? */
     GROUP_MILL,
     2,                         /* size */
     GROUP_MILL_COLOUR,
     GROUP_MILL_COST_MUL,
     GROUP_MILL_BUL_COST,
     GROUP_MILL_FIREC,
     GROUP_MILL_COST,
     GROUP_MILL_TECH},
    /* 29 */
    {N_("Pottery"),
     FALSE,                     /* need credit? */
     GROUP_POTTERY,
     2,                         /* size */
     GROUP_POTTERY_COLOUR,
     GROUP_POTTERY_COST_MUL,
     GROUP_POTTERY_BUL_COST,
     GROUP_POTTERY_FIREC,
     GROUP_POTTERY_COST,
     GROUP_POTTERY_TECH},
    /* 30 */
    {N_("Fire sta'n"),
     FALSE,                     /* need credit? */
     GROUP_FIRESTATION,
     2,                         /* size */
     GROUP_FIRESTATION_COLOUR,
     GROUP_FIRESTATION_COST_MUL,
     GROUP_FIRESTATION_BUL_COST,
     GROUP_FIRESTATION_FIREC,
     GROUP_FIRESTATION_COST,
     GROUP_FIRESTATION_TECH},
    /* 31 */
    {N_("Sports field"),
     FALSE,                     /* need credit? */
     GROUP_CRICKET,
     2,                         /* size */
     GROUP_CRICKET_COLOUR,
     GROUP_CRICKET_COST_MUL,
     GROUP_CRICKET_BUL_COST,
     GROUP_CRICKET_FIREC,
     GROUP_CRICKET_COST,
     GROUP_CRICKET_TECH},
    /* 32 */
    /* there is no button for this. */
    {N_("Burnt"),
     FALSE,                     /* need credit? */
     GROUP_BURNT,
     1,                         /* size */
     GROUP_BURNT_COLOUR,
     GROUP_BURNT_COST_MUL,
     GROUP_BURNT_BUL_COST,
     GROUP_BURNT_FIREC,
     GROUP_BURNT_COST,
     GROUP_BURNT_TECH},
    /* 33 */
    /* there is no button for this. */
    {N_("Shanty town"),
     FALSE,                     /* need credit? */
     GROUP_SHANTY,
     2,                         /* size */
     GROUP_SHANTY_COLOUR,
     GROUP_SHANTY_COST_MUL,
     GROUP_SHANTY_BUL_COST,
     GROUP_SHANTY_FIREC,
     GROUP_SHANTY_COST,
     GROUP_SHANTY_TECH},
    /* 34 */
    /* there is no button for this. */
    {N_("Fire"),
     FALSE,                     /* need credit? */
     GROUP_FIRE,
     1,                         /* size */
     GROUP_FIRE_COLOUR,
     GROUP_FIRE_COST_MUL,
     GROUP_FIRE_BUL_COST,
     GROUP_FIRE_FIREC,
     GROUP_FIRE_COST,
     GROUP_FIRE_TECH},
    /* 35 */
    /* there is no button for this. */
    {N_("Used"),
     FALSE,                     /* need credit? */
     GROUP_USED,
     1,                         /* size */
     GROUP_USED_COLOUR,
     GROUP_USED_COST_MUL,
     GROUP_USED_BUL_COST,
     GROUP_USED_FIREC,
     GROUP_USED_COST,
     GROUP_USED_TECH},
    /* 36 */
    {N_("Residential ML"),
     FALSE,                     /* need credit? */
     GROUP_RESIDENCE_ML,
     3,                         /* size */
     GROUP_RESIDENCE_ML_COLOUR,
     GROUP_RESIDENCE_ML_COST_MUL,
     GROUP_RESIDENCE_ML_BUL_COST,
     GROUP_RESIDENCE_ML_FIREC,
     GROUP_RESIDENCE_ML_COST,
     GROUP_RESIDENCE_ML_TECH},
    /* 37 */
    {N_("Residential HL"),
     FALSE,                     /* need credit? */
     GROUP_RESIDENCE_HL,
     3,                         /* size */
     GROUP_RESIDENCE_HL_COLOUR,
     GROUP_RESIDENCE_HL_COST_MUL,
     GROUP_RESIDENCE_HL_BUL_COST,
     GROUP_RESIDENCE_HL_FIREC,
     GROUP_RESIDENCE_HL_COST,
     GROUP_RESIDENCE_HL_TECH},
    /* 38 */
    {N_("Residential LH"),
     FALSE,                     /* need credit? */
     GROUP_RESIDENCE_LH,
     3,                         /* size */
     GROUP_RESIDENCE_LH_COLOUR,
     GROUP_RESIDENCE_LH_COST_MUL,
     GROUP_RESIDENCE_LH_BUL_COST,
     GROUP_RESIDENCE_LH_FIREC,
     GROUP_RESIDENCE_LH_COST,
     GROUP_RESIDENCE_LH_TECH},
    /* 39 */
    {N_("Residential MH"),
     FALSE,                     /* need credit? */
     GROUP_RESIDENCE_MH,
     3,                         /* size */
     GROUP_RESIDENCE_MH_COLOUR,
     GROUP_RESIDENCE_MH_COST_MUL,
     GROUP_RESIDENCE_MH_BUL_COST,
     GROUP_RESIDENCE_MH_FIREC,
     GROUP_RESIDENCE_MH_COST,
     GROUP_RESIDENCE_MH_TECH},
    /* 40 */
    {N_("Residential HH"),
     FALSE,                     /* need credit? */
     GROUP_RESIDENCE_HH,
     3,                         /* size */
     GROUP_RESIDENCE_HH_COLOUR,
     GROUP_RESIDENCE_HH_COST_MUL,
     GROUP_RESIDENCE_HH_BUL_COST,
     GROUP_RESIDENCE_HH_FIREC,
     GROUP_RESIDENCE_HH_COST,
     GROUP_RESIDENCE_HH_TECH},
    /* 41 */
    {N_("Water well"),
     FALSE,                     /* need credit? */
     GROUP_WATERWELL,
     2,                         /* size */
     GROUP_WATERWELL_COLOUR,
     GROUP_WATERWELL_COST_MUL,
     GROUP_WATERWELL_BUL_COST,
     GROUP_WATERWELL_FIREC,
     GROUP_WATERWELL_COST,
     GROUP_WATERWELL_TECH},

    /* 42 */
    {N_("Desert"),              /* name */
     FALSE,                     /* need credit? */
     GROUP_DESERT,              /* group number */
     1,                         /* size */
     GROUP_DESERT_COLOUR,       /* color */
     GROUP_DESERT_COST_MUL,     /* cost multiplier */
     GROUP_DESERT_BUL_COST,     /* bulldoze cost */
     GROUP_DESERT_FIREC,        /* probability of fire */
     GROUP_DESERT_COST,         /* cost */
     GROUP_DESERT_TECH          /* tech */
     },
    /* 43 */
    {N_("Tree"),                /* name */
     FALSE,                     /* need credit? */
     GROUP_TREE,                /* group number */
     1,                         /* size */
     GROUP_TREE_COLOUR,         /* color */
     GROUP_TREE_COST_MUL,       /* cost multiplier */
     GROUP_TREE_BUL_COST,       /* bulldoze cost */
     GROUP_TREE_FIREC,          /* probability of fire */
     GROUP_TREE_COST,           /* cost */
     GROUP_TREE_TECH            /* tech */
     },

    /* 44 */
    {N_("Trees"),               /* name */
     FALSE,                     /* need credit? */
     GROUP_TREE2,               /* group number */
     1,                         /* size */
     GROUP_TREE2_COLOUR,        /* color */
     GROUP_TREE2_COST_MUL,      /* cost multiplier */
     GROUP_TREE2_BUL_COST,      /* bulldoze cost */
     GROUP_TREE2_FIREC,         /* probability of fire */
     GROUP_TREE2_COST,          /* cost */
     GROUP_TREE2_TECH           /* tech */
     },

    /* 45 */
    {N_("Forest"),              /* name */
     FALSE,                     /* need credit? */
     GROUP_TREE3,               /* group number */
     1,                         /* size */
     GROUP_TREE3_COLOUR,        /* color */
     GROUP_TREE3_COST_MUL,      /* cost multiplier */
     GROUP_TREE3_BUL_COST,      /* bulldoze cost */
     GROUP_TREE3_FIREC,         /* probability of fire */
     GROUP_TREE3_COST,          /* cost */
     GROUP_TREE3_TECH           /* tech */
     },

    /* 46 */
    {N_("Track Bridge"),
     FALSE,                     /* need credit? */
     GROUP_TRACK_BRIDGE,
     1,                         /* size */
     GROUP_TRACK_BRIDGE_COLOUR,
     GROUP_TRACK_BRIDGE_COST_MUL,
     GROUP_TRACK_BRIDGE_BUL_COST,
     GROUP_TRACK_BRIDGE_FIREC,
     GROUP_TRACK_BRIDGE_COST,
     GROUP_TRACK_BRIDGE_TECH},

     /* 47 */
    {N_("Road Bridge"),
     FALSE,                     /* need credit? */
     GROUP_ROAD_BRIDGE,
     1,                         /* size */
     GROUP_ROAD_BRIDGE_COLOUR,
     GROUP_ROAD_BRIDGE_COST_MUL,
     GROUP_ROAD_BRIDGE_BUL_COST,
     GROUP_ROAD_BRIDGE_FIREC,
     GROUP_ROAD_BRIDGE_COST,
     GROUP_ROAD_BRIDGE_TECH},

     /* 48 */
    {N_("Rail Bridge"),
     FALSE,                     /* need credit? */
     GROUP_RAIL_BRIDGE,
     1,                         /* size */
     GROUP_RAIL_BRIDGE_COLOUR,
     GROUP_RAIL_BRIDGE_COST_MUL,
     GROUP_RAIL_BRIDGE_BUL_COST,
     GROUP_RAIL_BRIDGE_FIREC,
     GROUP_RAIL_BRIDGE_COST,
     GROUP_RAIL_BRIDGE_TECH},

     /* 49 */
    /* End of Data */
    {"EOF",
     FALSE,                     /* need credit? */
     0,
     0,                         /* size */
     0,
     0,
     0,
     0,
     0,
     0}
};

void init_types(void)
{
    char png_file[LC_PATH_MAX], txt_file[LC_PATH_MAX];
    sprintf(png_file, "%s%c%s", opening_path, PATH_SLASH, "icons.png");
    sprintf(txt_file, "%s%c%s", opening_path, PATH_SLASH, "iconlist.txt");

    load_png_graphics(txt_file, png_file);

    main_types[CST_USED].group = GROUP_USED;
    main_types[CST_USED].graphic = 0;   /* Won't be dereferenced! */
}

int get_group_of_type(short type)
{
    if (type == CST_NONE)
        return GROUP_BARE;
    return main_types[type].group;
}

void set_map_groups(void)
{
    int x, y;
    for (x = 0; x < WORLD_SIDE_LEN; x++) {
        for (y = 0; y < WORLD_SIDE_LEN; y++) {
            MP_GROUP(x, y) = get_group_of_type(MP_TYPE(x, y));
        }
    }
}

int get_group_cost(short group)
{
    return (int)((float)main_groups[group].cost
                 + ((float)main_groups[group].cost * ((float)main_groups[group].cost_mul * (float)tech_level)
                    / (float)MAX_TECH_LEVEL));
}

void get_type_name(short type, char *s)
{
    short g;
    g = get_group_of_type(type);
    strcpy(s, _(main_groups[g].name));
}

int get_type_cost(short type)
{
    return get_group_cost((short)get_group_of_type(type));
}
