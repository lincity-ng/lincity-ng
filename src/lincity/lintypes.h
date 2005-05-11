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

struct GROUP
{
    const char* name;      /* name of group */
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
