/* ---------------------------------------------------------------------- *
 * lintypes.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __lintypes_h__
#define __lintypes_h__

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define NUM_OF_TYPES    400
#define NUM_OF_GROUPS    42
#define GROUP_NAME_LEN   20
/*
typedef struct Group Group;

struct Group
{
	const char* name;
	int size;
	int need_credit;
	int cost_build;
	int cost_multiplier;
	int cost_buldoze;
	int fire_probability;
	int tech_requirement;
};

enum GROUP
{
	GROUP_LAND,
	GROUP_POWER_LINE,
	GROUP_SOLAR_POWER,
	GROUP_SUBWAY_STATION,
	GROUP_RESIDENCE_LL,
	GROUP_FARM,
	GROUP_MARKET,
	GROUP_TRACK,
	GROUP_COAL_MINE,
	GROUP_RAIL,
	GROUP_COAL_POWER,
	GROUP_ROAD,
	GROUP_LIGHT_INDUSTRY,
	GROUP_UNIVERSITY,
	GROUP_COMMUNE,
	GROUP_ORE_MINE,
	GROUP_TIP,
	GROUP_PORT,
	GROUP_HEAVY_INDUSTRY,
	GROUP_PARK,
	GROUP_RECYCLE,
	GROUP_WATER,
	GROUP_HOSPITAL,
	GROUP_ROCKET,
	GROUP_WIND_FARM,
	GROUP_MONUMENT,
	GROUP_SCHOOL,
	GROUP_BLACKSMITH,
	GROUP_MILL,
	GROUP_POTTERY,
	GROUP_FIRE_STATION,
	GROUP_SPORTS,
	GROUP_BURNT,
	GROUP_SHANTY_TOWN,
	GROUP_FIRE,
	GROUP_USED,
	GROUP_RESIDENCE_ML,
	GROUP_RESIDENCE_HL,
	GROUP_RESIDENCE_LH,
	GROUP_RESIDENCE_MH,
	GROUP_RESIDENCE_HH,
	GROUP_MAX
};
*/

struct GROUP
{
    char  name[GROUP_NAME_LEN+1];      /* name of group */
    unsigned short        no_credit;   /* TRUE if need credit to build */
    unsigned short        group;       /* This is redundant: it must match
					  the index into the table */
    unsigned short        size;
    int   colour;         /* summary map colour */
    int   cost_mul;       /* group cost multiplier */
    int   bul_cost;       /* group bulldoze cost */
    int   fire_chance;    /* probability of fire */
    int   cost;           /* group cost */
    int   tech;           /* group cost */
};

struct TYPE
{
    int group;          /* What group does this type belong to? */
    char* graphic;      /* Bitmap of the graphic */
};

int get_group_of_type (short selected_type);
void set_map_groups (void);
int get_type_cost (short type);
int get_group_cost (short group);
void get_type_name (short type, char * s);

extern struct GROUP main_groups[NUM_OF_GROUPS];
extern struct TYPE main_types[NUM_OF_TYPES];

#endif /* __lintypes_h__ */
