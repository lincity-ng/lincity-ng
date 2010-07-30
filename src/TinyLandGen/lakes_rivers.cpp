/** TinyEarth LandGenerator
 *
 * @file 	lakes_rivers.cpp
 * @author 	alain.baeckeroot@laposte.net
 * @copyright 	2010 Alain BAECKEROOT
 * @license 	GPL v3 or later
 */

#include "lctypes.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

extern int alt_min, alt_max, alt_step;
extern lmap_struct lmap;

struct Shoreline {
	int x;
	int y;
	int altitude;
	struct Shoreline *next;
};

void connect_rivers(void)
{
}

static int in_map(int x, int y)
{
	return ((x >= 0) && (x < WORLD_SIDE_LEN) && (y >= 0) && (y < WORLD_SIDE_LEN));
}

static int is_border(int x, int y)
{
	if ((x == 0) || (x == (WORLD_SIDE_LEN - 1)) || (y == 0) || (y == (WORLD_SIDE_LEN - 1)))
		return 1;
	else
		return 0;
}

static void set_river_tile(int i, int j)
{
	MP_TYPE(i, j) = WATER;
	MP_FLAG(i, j) |= FLAG_IS_RIVER;
	MP_COLOR(i, j) = 128;	//dark blue
}

static void sort_by_altitude(int n, int *tabx, int *taby)
{
	// sort ascending
	int tmp_x, tmp_y;
	bool sorted = false;

	// bubble sort. n is near 10 so ...
	for (int i = 0; i < n && !sorted; i++) {
		sorted = true;
		for (int j = 1; j < n - i; j++)
			if (ALT(tabx[j], taby[j]) < ALT(tabx[j - 1], taby[j - 1])) {
				tmp_x = tabx[j - 1];
				tmp_y = taby[j - 1];
				tabx[j - 1] = tabx[j];
				taby[j - 1] = taby[j];
				tabx[j] = tmp_x;
				taby[j] = tmp_y;
				sorted = false;
			}
	}
}

static void free_shore(Shoreline * shore)
{
	Shoreline *tmp;
	while (shore->next != NULL) {
		tmp = shore;
		shore = shore->next;
		free(tmp);
	}
	free(shore);
}

static Shoreline *init_shore(void)
{
	Shoreline *shore;

	shore = (Shoreline *) malloc(sizeof(struct Shoreline));
	shore->next = (Shoreline *) malloc(sizeof(struct Shoreline));

	shore->x = -1;
	shore->y = -1;
	shore->altitude = -1;
	shore->next = NULL;

	return shore;
}

static void add_shore_point(Shoreline * current, int x, int y, int altitude)
{
	Shoreline *newp;
	newp = (Shoreline *) malloc(sizeof(struct Shoreline));
	newp->next = (Shoreline *) malloc(sizeof(struct Shoreline));
	newp->x = x;
	newp->y = y;
	newp->altitude = altitude;
	newp->next = current->next;
	current->next = newp;
}

static void try_shore_point(int x, int y, Shoreline * shore)
{
	Shoreline *current;
	int a;
	current = shore;
	a = ALT(x, y);

	while (current->next != NULL) {
		if (a < current->next->altitude) {
			// insert in beginning of the list
			add_shore_point(current, x, y, a);
			return;
		} else if (a == current->next->altitude) {
			while ((current->next != NULL) && (current->next->altitude == a)) {
				if ((current->x == x) && (current->y == y)) {
					// do not insert the same shore point several times at different places
					return;
				};
				current = current->next;
			};
			//insert the shore point in the list
			add_shore_point(current, x, y, a);
			return;
		};
		current = current->next;
	};
	// we reached end of list
	// altitude of the point is strict maximum of the list
	if ((current->x != x) && (current->y != y))
		add_shore_point(current, x, y, a);
}

static int setup_one_river(int xx, int yy, int lake_id, Shoreline * shore)
{
	int alt_max, x, y, alt, x0, y0;
	// start a river from point (xx, yy)
	set_river_tile(xx, yy);
	alt_max = ALT(xx, yy);

	x0 = xx;
	y0 = yy;
	/* follow most important slope and go downward */
	while (((xx != x) || (yy != y)) && (xx != 0) && (xx != (WORLD_SIDE_LEN - 1)) && (yy != 0)
	       && (yy != WORLD_SIDE_LEN - 1)) {
		int m = 0;
		x = xx;
		y = yy;
		alt = ALT(x, y);
		for (int n = 0; n < 8; n++) {
			if (in_map(x + di[n], y + dj[n])) {
				if (ALT(x + di[n], y + dj[n]) < alt) {
					xx = x + di[n];
					yy = y + dj[n];
					alt = ALT(xx, yy);
					m = n;
				}
				// find neighbours and update shore line if needed
				// may mark as shoreline a point which will be set as river later. We don't care
				if (!IS_WATER(x + di[n], y + dj[n]))
					try_shore_point(x + di[n], y + dj[n], shore);
			}
		}

		set_river_tile(xx, yy);
		if (m > 3) {
			// we did diagonal move, so we need to connect river
			if (ALT(x + di[m], y) > ALT(x, y + dj[m]))
				set_river_tile(x, y + dj[m]);
			else
				set_river_tile(x + di[m], y);
		}
	};
	// We are in a local minimum or at the borders of the map (strictly the lowest points)

}

static void overfill_lake(int x, int y, Shoreline * shore, int lake_id)
{
	// Starting point is a local minimum
	// Lake growth is done iteratively by flooding the lowest shore point and rising water level
	// shore point = neighbour without water 
	//      (at this point we have no water in the map, except other lakes and rivers)
	//
	// We have a list of shore points sorted by altitude

	int i, level;

	if (is_border(x, y))
		return;

	set_river_tile(x, y);
	level = ALT(x, y);

	// find neighbours
	for (i = 0; i < 8; i++) {
		if (in_map(x + di[i], y + dj[i]) && !IS_WATER(x + di[i], y + dj[i]))
			try_shore_point(x + di[i], y + dj[i], shore);
	}

	if (shore->next != NULL) {
		shore = shore->next;
		x = shore->x;
		y = shore->y;

		if ((ALT(x, y) < level)) {
			set_river_tile(x, y);
			// create river and continue to build shoreline
			// we will continue to overfill (from a lower point) until we reach border of the map
			//fprintf(stdout, "We found a pass x %i, y %i, alt %i \n", x, y, ALT(x,y));
			setup_one_river(x, y, lake_id, shore);
		}
		overfill_lake(x, y, shore, lake_id);
	} else {
		// Q: ? Should this happen ?
		// A: yes if we are in a lake that was previously filled by a higher one which overfilled here
		//    else ? it should not happen ?
		//fprintf(stderr,"the shoreline list is empty, x = %i, y = %i\n", x, y);
	}
}

void setup_lake_river(void)
{
	// brute search of local minimum

	int lakx[WORLD_SIDE_LEN * WORLD_SIDE_LEN], laky[WORLD_SIDE_LEN * WORLD_SIDE_LEN];
	Shoreline *shore;
	int i, j, l, m, alt;

	// Put the gray border (not visible) at alt_min - 1, for easier rivers handling.
	for (i = 0; i < WORLD_SIDE_LEN; i++) {
		ALT(i, 0) = alt_min - 1;
		ALT(i, WORLD_SIDE_LEN - 1) = alt_min - 1;
		ALT(0, i) = alt_min - 1;
		ALT(WORLD_SIDE_LEN - 1, i) = alt_min - 1;
	}

	l = 0;
	for (i = 1; i < WORLD_SIDE_LEN - 1; i++) {
		for (j = 1; j < WORLD_SIDE_LEN - 1; j++) {
			alt = ALT(i, j);
			m = -1;
			for (int n = 0; n < 8; n++) {
				if (ALT(i + di[n], j + dj[n]) < alt) {
					m = n;
				}
			}
			if (m == -1) {
				lakx[l] = i;
				laky[l] = j;
				l++;
			}
		}
	}

	sort_by_altitude(l, lakx, laky);
	for (i = 0; i < l; i++) {
		// start by the lowest lake
		//fprintf(stdout, "LAKE %i : %i, %i, alt %f\n", i, lakx[i], laky[i], ALT(lakx[i], laky[i]));
		shore = init_shore();
		set_river_tile(lakx[i], laky[i]);
		overfill_lake(lakx[i], laky[i], shore, WORLD_SIDE_LEN * lakx[i] + laky[i]);
		free_shore(shore);
	}
}
