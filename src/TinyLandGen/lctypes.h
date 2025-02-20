/**
 * @file lctypes.h
 */

#ifndef __lctypes_h__
#define __lctypes_h__

/* Vector for visiting neigbours = ( di(k) , dj(k) )  ; ordered so that diagonal moves are the last 4 */
static const int di[8] = { -1, 0, 1, 0, 1, 1, -1, -1 };
static const int dj[8] = { 0, -1, 0, 1, 1, -1, 1, -1 };

enum {
	WORLD_SIDE_LEN = 100,
	SIZE = 8,
	SCREEN_SIZE = SIZE * WORLD_SIDE_LEN

/*
 * needed to save in LinCity-NG format
 */
//gui_interface/pbar_interface.h:
	PBAR_DATA_SIZE = 12,
	NUM_PBARS = 9,

//gui_interface/shared_globals.h:
	NUMOF_MODULES = 32
};

// src/lincity/lctypes.h  CST_stuff
enum {
	GREEN	= 0,
	WATER	= 221,
	DESERT	= 350,
	TREE	= 351,
	TREE2	= 352,
	TREE3	= 353
};

// src/lincity/loadsave.h
enum { WATERWELL_V2 = 1322 };

// src/lincity/lintypes.h
enum {
	MAX_NUMOF_SUBSTATIONS = 512,
	MAX_NUMOF_MARKETS = 512
};

struct lmap_struct {
	float altitude[WORLD_SIDE_LEN][WORLD_SIDE_LEN];
	unsigned long color[WORLD_SIDE_LEN][WORLD_SIDE_LEN];
	int type[WORLD_SIDE_LEN][WORLD_SIDE_LEN];
	int flag[WORLD_SIDE_LEN][WORLD_SIDE_LEN];
	int dist2w[WORLD_SIDE_LEN][WORLD_SIDE_LEN]; // distance to water; manhattan (L1) is fast to compute iteratively
    	int ecotable[WORLD_SIDE_LEN][WORLD_SIDE_LEN]; //
};

float ALT(int x, int y)			{ return lmap.altitude[x][y]; }
int MP_TYPE(int x, int y)		{ return lmap.type[x][y]; }
int MP_FLAG(int x, int y)		{ return lmap.flag[x][y]; }
unsigned long MP_COLOR(int x, int y)	{ return lmap.color[x][y]; }

//src/lincity/power.h:
bool IS_WATER(x,y)    { return MP_TYPE(x,y) == WATER; }

//src/lincity/init_game.cpp:
bool IS_RIVER(int x, inty) { return MP_FLAG(x,y) & FLAG_IS_RIVER; }

// src/lincity/lin-city.h
enum {
	FLAG_IS_RIVER			= 0x800000,
	FLAG_HAS_UNDERGROUND_WATER	= 0x20000000
};

#endif
