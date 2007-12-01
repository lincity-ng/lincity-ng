/* ---------------------------------------------------------------------- *
 * lintypes.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __lintypes_h__
#define __lintypes_h__

/********** Data structures ***************/

struct GROUP {
    const char *name;           /* name of group */
    unsigned short no_credit;   /* TRUE if need credit to build */
    unsigned short group;       /* This is redundant: it must match
                                   the index into the table */
    unsigned short size;
    int colour;                 /* summary map colour */
    int cost_mul;               /* group cost multiplier */
    int bul_cost;               /* group bulldoze cost */
    int fire_chance;            /* probability of fire */
    int cost;                   /* group cost */
    int tech;                   /* group cost */
};

struct TYPE {
    int group;                  /* What group does this type belong to? */
    char *graphic;              /* Bitmap of the graphic */
};

int get_group_of_type(short selected_type);
void set_map_groups(void);
int get_type_cost(short type);
int get_group_cost(short group);
void get_type_name(short type, char *s);

extern struct GROUP main_groups[NUM_OF_GROUPS];
extern struct TYPE main_types[NUM_OF_TYPES];


struct map_point_info_struct {
    int population;
    int flags;
    unsigned short coal_reserve;
    unsigned short ore_reserve;
    int int_1;
    int int_2;
    int int_3;
    int int_4;
    int int_5;
    int int_6;
    int int_7;
};
typedef struct map_point_info_struct Map_Point_Info;

/* Set these flags to true when they need to be updated on the screen */
struct update_scoreboard_struct {
    int mps;
    int mini;
    int date;
    int money;
    int monthly;
    int yearly_1;
    int yearly_2;
    long int message_area;
};
typedef struct update_scoreboard_struct Update_Scoreboard;

#endif /* __lintypes_h__ */
