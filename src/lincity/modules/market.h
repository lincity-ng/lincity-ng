/* ---------------------------------------------------------------------- *
 * market.h
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#include "../engglobs.h"

void mps_market(int x, int y);

int get_food(int, int, int);
int put_food(int, int, int);
int get_jobs(int, int, int);
int put_jobs(int, int, int);
int get_goods(int, int, int);
int put_goods(int, int, int);
int get_ore(int, int, int);
int put_ore(int, int, int);
int get_coal(int, int, int);
int put_coal(int, int, int);
int add_a_market(int, int);
void remove_a_market(int, int);
void do_market(int, int);
void shuffle_markets(void);
int deal_with_transport(int, int, int, int);
int get_steel(int, int, int);
int put_steel(int, int, int);
int get_waste(int, int, int);
int put_waste(int, int, int);
int get_stuff(int, int, int, int);
int get_stuff2(Map_Point_Info *, int, int);
int get_stuff3(Map_Point_Info *, int, int);
int get_stuff4(Map_Point_Info *, int, int);
int put_stuff(int, int, int, int);
int put_stuff2(Map_Point_Info *, short *, int, int);
int put_stuff3(Map_Point_Info *, short *, int, int);
int put_stuff4(Map_Point_Info *, short *, int, int);

