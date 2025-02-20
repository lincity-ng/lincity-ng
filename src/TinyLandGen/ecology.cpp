/** Tiny Land Generator
 *
 * @file 	ecology.cpp
 * @author 	alain.baeckeroot@laposte.net
 * @copyright 	2010 Alain BAECKEROOT
 * @license 	GPL v3 or later
 */

#include "lctypes.h"
#include <algorithm>

extern lmap_struct lmap;
extern int alt_min, alt_max, alt_step;

const int INIT_DIST = 2 * WORLD_SIDE_LEN; // works for d0, d1, d2 distances

int global_aridity = rand() % 450 - 150;

void set_mappoint(int x, int y, int type)
{
	MP_TYPE(x,y) = type;
}

static void do_rand_ecology(int x, int y)
{
	if ((MP_FLAG(x, y) | FLAG_HAS_UNDERGROUND_WATER) == 0) return; /*true desert */
	int r = lmap.ecotable[x][y];

	if (r >= 300) {
		/* very dry land */
		int r2 = rand() % 10;
		set_mappoint(x, y, (r2 <= 6 ? DESERT : r2 <= 8 ? GREEN : TREE));
	} else if (r >= 160) {
		int r2 = rand() % 10;
		set_mappoint(x, y, (r2 <= 2 ? DESERT : r2 <= 6 ? GREEN : TREE));
	} else if (r >= 80) {
		int r2 = rand() % 10;
		set_mappoint(x, y, (r2 <= 1 ? DESERT : r2 <= 4 ? GREEN : r2 <= 6 ? TREE : TREE2));
	} else if (r >= 40) {
		int r2 = rand() % 40;
		set_mappoint(x, y, (r2 == 0 ? DESERT : r2 <= 12 ? GREEN : r2 <= 24 ? TREE : r2 <= 36 ? TREE2 : TREE3));
	} else if (r >= 0) {
		/* normal land */
		int r2 = rand() % 40;
		set_mappoint(x, y, (r2 <= 10 ? GREEN : r2 <= 20 ? TREE : r2 <= 30 ? TREE2 : TREE3));
	} else if (r >= -40) {
		/* forest */
		int r2 = rand() % 40;
		set_mappoint(x, y, (r2 <= 5 ? GREEN : r2 <= 10 ? TREE : r2 <= 25 ? TREE2 : TREE3));
	} else if (r >= -80) {
		int r2 = rand() % 40;
		if (r2 == 0)
			MP_TYPE(x, y) = WATER;
		else
			set_mappoint(x, y, (r2 <= 6 ? GREEN : r2 <= 15 ? TREE : r2 <= 28 ? TREE2 : TREE3));
	} else if (r >= -120) {
		int r2 = rand() % 40;
		if (r2 <= 1)
			MP_TYPE(x, y) = WATER;
		else
			set_mappoint(x, y, (r2 <= 6 ? GREEN : r2 <= 16 ? TREE : r2 <= 30 ? TREE2 : TREE3));
	} else {
		/* wetland */
		int r2 = rand() % 40;
		if (r2 <= 3)
			MP_TYPE(x, y) = WATER;
		else
			set_mappoint(x, y, (r2 <= 8 ? GREEN : r2 <= 20 ? TREE : r2 <= 35 ? TREE2 : TREE3));
	}
}

int dist_to_water( int x, int y)
{
	// Mahattan distance (d1) computed iteratively
	auto d2r = [](int x, int y) { return lmap.dist2w[x][y]; };
	int d = INIT_DIST;
	if (IS_RIVER(x,y))
		return 0;

	if (x == 0) 
		if (y == 0)
			d = std::min ( d2r(x,y+1), d2r(x+1,y) );
		else if (y == WORLD_SIDE_LEN) 
			d = std::min ( d2r(x, y-1), d2r(x+1,y) );
		else 
			d = std::min ( std::min (d2r(x, y-1), d2r(x, y+1)), d2r(x+1,y));
	else if (x == WORLD_SIDE_LEN)
		if (y == 0)
			d =  std::min (d2r(x,y+1), d2r(x-1,y) );
		else if (y == WORLD_SIDE_LEN) 
			d =  std::min (d2r(x, y-1), d2r(x-1,y) );
		else 
			d = std::min ( std::min (d2r(x, y-1), d2r(x, y+1)), d2r(x-1,y));
	else if (y == 0)
		d = std::min ( std::min (d2r(x-1, y), d2r(x+1, y)), d2r(x,y+1));
	else if (y == WORLD_SIDE_LEN)
		d = std::min ( std::min (d2r(x-1, y), d2r(x+1, y)), d2r(x,y-1));
	else if (lmap.dist2w[x][y] == INIT_DIST) 
		d = std::min( std::min(d2r(x - 1, y), d2r(x+1,y)), std::min( d2r(x, y - 1), d2r(x, y + 1)) );

	// d = distance of neighbours
	// Mahattan distance (d1) computed iteratively = the +1 below
	return (std::min(d + 1, lmap.dist2w[x][y]));
}

void distance_to_river(void)
{
	int newd, number, x, y;
	int iter = 0;
	number = WORLD_SIDE_LEN * WORLD_SIDE_LEN;

	for (y = 0; y < WORLD_SIDE_LEN ; y++)
		for (x = 0; x < WORLD_SIDE_LEN; x++)
			if (IS_RIVER(x,y)) {
				lmap.dist2w[x][y] = 0;
				number--;
			} else {
				lmap.dist2w[x][y] = INIT_DIST;
			}

	//fprintf(stdout,"initial remaining %i \n", number);
	while (number > 0) {
		for (y = 0; y < WORLD_SIDE_LEN; y++) {
			for (x = 0; x < WORLD_SIDE_LEN; x++) {
				newd = dist_to_water(x,y);
				if ( (newd != 0) && (newd < INIT_DIST)) {
					if (lmap.dist2w[x][y] == INIT_DIST)
						number--;
					lmap.dist2w[x][y] = newd;
					//fprintf(stdout," d2w = %i \n", newd);
				}
			}
			//fprintf(stdout," remaining %i \n", number);
		}
		++iter;
	}
	//fprintf(stdout," iteration %i \n", iter);


}

void setup_land()
{
	int x, y, xw, yw;
	int aridity = global_aridity;

	distance_to_river();

	for (y = 0; y < WORLD_SIDE_LEN; y++) {
		for (x = 0; x < WORLD_SIDE_LEN; x++) {
			int d2w_min = 2 * WORLD_SIDE_LEN * WORLD_SIDE_LEN;
			int r;
			int arid = aridity;
			//int alt0 = 0;

			/* test against IS_RIVER to prevent terrible recursion */
			if (IS_RIVER(x, y))
				continue;

			d2w_min = lmap.dist2w[x][y];

			/* near river lower aridity */
			if (aridity > 0) {
				if (d2w_min < 5)
					arid = aridity / 3;
				else if (d2w_min < 17)
					arid = (aridity * 2) / 3;
			}
			/* Altitude has same effect as distance */
			r = rand() % (d2w_min / 3 + 1) \
			    + arid \
			    + abs(ALT(x, y) * 15 / alt_step) \
			    + 3 * (ALT(x, y) * ALT(x, y)) / 1000000;

			lmap.ecotable[x][y] = r;
			/* needed to setup quasi randome land. The flag is set below */
			MP_FLAG(x, y) |= FLAG_HAS_UNDERGROUND_WATER;
			do_rand_ecology(x, y);

			/* preserve rivers, so that we can connect port later */
			if (MP_TYPE(x, y) == WATER) {
				int navigable = MP_FLAG(x, y) & FLAG_IS_RIVER;
				set_mappoint(x, y, WATER);
				MP_FLAG(x, y) |= navigable;
				MP_FLAG(x, y) |= FLAG_HAS_UNDERGROUND_WATER;
			}
			/* set undergroung water according to first random land setup */
			if (MP_TYPE(x, y) == DESERT) {
				MP_FLAG(x, y) &= (0xffffffff - FLAG_HAS_UNDERGROUND_WATER);
			}
		}
	}
	for (y = 0; y < WORLD_SIDE_LEN; y++)
		for (x = 0; x < WORLD_SIDE_LEN; x++)
			if (MP_TYPE(x, y) == WATER)
				MP_FLAG(x, y) |= FLAG_HAS_UNDERGROUND_WATER;

	//connect_rivers();
}
