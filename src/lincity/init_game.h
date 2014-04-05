/* ----------------------------------------------------------------------
 * init_game.h
 * This file is part of lincity-ng
 * see COPYING for license, and CREDITS for authors
 * ----------------------------------------------------------------------
 */
#ifndef __init_game_h__
#define __init_game_h__

typedef struct _CitySettings city_settings;

struct _CitySettings {
  bool with_village;
  bool without_trees;
};

void destroy_game(void);
void clear_game(void);
void setup_land(void);
void create_new_city(int *originx, int *originy, city_settings *city, int old_setup_ground, int climate);

void new_city(int *originx, int *originy, city_settings *city);
void new_desert_city(int *originx, int *originy, city_settings *city);
void new_temperate_city(int *originx, int *originy, city_settings *city);
void new_swamp_city(int *originx, int *originy, city_settings *city);

#endif /* __init_game_h__ */

/** @file lincity/init_game.h */

