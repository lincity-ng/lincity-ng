/* ----------------------------------------------------------------------
 * init_game.h
 * This file is part of lincity-ng
 * see COPYING for license, and CREDITS for authors
 * ----------------------------------------------------------------------
 */
#ifndef __init_game_h__
#define __init_game_h__

void clear_game(void);
void setup_land(void);
void new_city(int *originx, int *originy, int random_village);
void create_new_city(int *originx, int *originy, int random_village, int old_setup_ground, int climate);

#endif /* __init_game_h__ */

/** @file lincity/init_game.h */

