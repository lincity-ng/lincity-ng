/* ---------------------------------------------------------------------- *
 * animate.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#ifndef __animate_h__
#define __animate_h__

/* ---------------------------------------------------------------------- *
 * Type Definitions
 * ---------------------------------------------------------------------- */
struct animation_info_struct
{
  int state;           /* Context dependent state variable */
  long timeout;        /* Reanimate when timeout is reached */
};
typedef struct animation_info_struct Animation_Info;


/* ---------------------------------------------------------------------- *
 * Global Variables
 * ---------------------------------------------------------------------- */
extern Animation_Info anim_info[WORLD_SIDE_LEN][WORLD_SIDE_LEN];

#define ANIM_STATE(x,y) anim_info[x][y].state
#define ANIM_TIMEOUT(x,y) anim_info[x][y].timeout

/* ---------------------------------------------------------------------- *
 * Function Prototypes
 * ---------------------------------------------------------------------- */
void init_animation (void);

void animate_organic_farm (int x, int y);
void animate_market (int x, int y);
void animate_industry_l (int x, int y);
void animate_coalmine (int x, int y);
void animate_power_source_coal (int x, int y);
void animate_industry_h (int x, int y);

void animate_commune (int x, int y);
void animate_oremine (int x, int y);
void animate_tip (int x, int y);
void animate_rocket (int x, int y);
void animate_monument (int x, int y);
void animate_windmill (int x, int y);
void animate_blacksmith (int x, int y);
void animate_firestation (int x, int y);
void animate_cricket (int x, int y);
void animate_fire (int x, int y);
void animate_mill (int x, int y);
void animate_pottery (int x, int y);

#endif /* __animate_h__ */
