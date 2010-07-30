/**
 * @file lctypes.h
 */

#ifndef __lctypes_h__
#define __lctypes_h__

// from http://www.ibm.com/developerworks/linux/library/l-gcc-hacks/index.html
#define min(x, y) ({				\
	typeof(x) _min1 = (x);			\
	typeof(y) _min2 = (y);			\
	(void) (&_min1 == &_min2);		\
	_min1 < _min2 ? _min1 : _min2; })	


/* Vector for visiting neigbours = ( di(k) , dj(k) )  ; ordered so that diagonal moves are the last 4 */
static const int di[8] = { -1, 0, 1, 0, 1, 1, -1, -1 };
static const int dj[8] = { 0, -1, 0, 1, 1, -1, 1, -1 };

#define WORLD_SIDE_LEN 100
#define SIZE 8
#define SCREEN_SIZE (SIZE * WORLD_SIDE_LEN)

/*
 * needed to save in LinCity-NG format
 */
//gui_interface/pbar_interface.h:
#define PBAR_DATA_SIZE 12
#define NUM_PBARS 9

//gui_interface/shared_globals.h:
#define NUMOF_MODULES 32

// src/lincity/lctypes.h  CST_stuff
#define GREEN		0
#define WATER		221
#define DESERT		350
#define TREE		351
#define TREE2		352
#define TREE3		353

// src/lincity/loadsave.h
#define WATERWELL_V2 1322

// src/lincity/lintypes.h
#define MAX_NUMOF_SUBSTATIONS 512
#define MAX_NUMOF_MARKETS 512

struct lmap_struct {
	float altitude[WORLD_SIDE_LEN][WORLD_SIDE_LEN];
	unsigned long color[WORLD_SIDE_LEN][WORLD_SIDE_LEN];
	int type[WORLD_SIDE_LEN][WORLD_SIDE_LEN];
	int flag[WORLD_SIDE_LEN][WORLD_SIDE_LEN];
	int dist2w[WORLD_SIDE_LEN][WORLD_SIDE_LEN]; // distance to water; manhattan (L1) is fast to compute iteratively
    	int ecotable[WORLD_SIDE_LEN][WORLD_SIDE_LEN]; //
};

#define ALT(x,y) lmap.altitude[x][y]
#define MP_TYPE(x,y)   lmap.type[x][y]
#define MP_FLAG(x,y)   lmap.flag[x][y]
#define MP_COLOR(x,y)	lmap.color[x][y]

//src/lincity/power.h:
#define IS_WATER(x,y)    (MP_TYPE(x,y) == WATER)

//src/lincity/init_game.cpp:
#define IS_RIVER(x,y) (MP_FLAG(x,y) & FLAG_IS_RIVER)

// src/lincity/lin-city.h
#define FLAG_IS_RIVER           (0x800000)
#define FLAG_HAS_UNDERGROUND_WATER (0x20000000)

#endif
