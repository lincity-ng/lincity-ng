/* ---------------------------------------------------------------------- *
 * shanty.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "modules.h"
//#include "mouse.h" /* XXX: fire_area! */
#include "shanty.h"
#include "commune.h"

extern void set_mappoint(int x, int y, short selected_type);


#include <stdlib.h>
static int spiral_find_2x2(int startx, int starty);
static int spiral_find_group(int startx, int starty, int group);

void add_a_shanty(void)
{
    int r, x, y;
    x = rand() % WORLD_SIDE_LEN;
    y = rand() % WORLD_SIDE_LEN;
    if (numof_shanties > 0 && rand() % 8 != 0) {
        r = spiral_find_group(x, y, GROUP_SHANTY);
        if (r == -1) {
            printf("Looked for a shanty, without any! x=%d y=%d\n", x, y);
            return;
        }
        y = r / WORLD_SIDE_LEN;
        x = r % WORLD_SIDE_LEN;
        r = spiral_find_2x2(x, y);
        if (r == -1) {
            /* wck: These are annoying when the map is full */
            fprintf(stderr,"Adding a shanty (s), no space for it?!\n");
            return;
        }
        y = r / WORLD_SIDE_LEN;
        x = r % WORLD_SIDE_LEN;
    } else {
        r = spiral_find_group(x, y, GROUP_MARKET);
        if (r == -1)
            return;             /* silently return, we havn't started yet. */

        y = r / WORLD_SIDE_LEN;
        x = r % WORLD_SIDE_LEN;
        r = spiral_find_2x2(x, y);
        if (r == -1) {
            /* see above */
            fprintf(stderr,"Adding a shanty (r), no space for it?!\n");
            return;
        }
        y = r / WORLD_SIDE_LEN;
        x = r % WORLD_SIDE_LEN;
    }
    set_mappoint(x, y, CST_SHANTY);
    numof_shanties++;
}

void remove_a_shanty(int x, int y)
{
    numof_shanties--;
    /* ATTENTION:
     * fire_area calls bulldoze_item which calls remove shanty.
     */
   do_bulldoze_area(CST_FIRE_1, x, y);

   /* now put the fire out: it becomes impossible to bulldoze */
   MP_INFO(x, y).int_2 = FIRE_LENGTH + 1;
   MP_INFO(x + 1, y).int_2 = FIRE_LENGTH + 1;
   MP_INFO(x, y + 1).int_2 = FIRE_LENGTH + 1;
   MP_INFO(x + 1, y + 1).int_2 = FIRE_LENGTH + 1;
}

void update_shanty(void)
{
    int i, pp;
    people_pool += .3 * numof_shanties;
    pp = people_pool - (COMMUNE_POP * numof_communes);
    i = (pp - SHANTY_MIN_PP) / SHANTY_POP;
    if (i > numof_shanties) {
        for (int n = 0; n < 1 + (i - numof_shanties)/10; n++)
            add_a_shanty();
    } else if (numof_shanties > 0 && (i < (numof_shanties - 1) )) {
        for (int n=0; n < (1+(numof_shanties - i)/10); n++) {
            int x, y, r;
            x = rand() % WORLD_SIDE_LEN;
            y = rand() % WORLD_SIDE_LEN;
            r = spiral_find_group(x, y, GROUP_SHANTY);
            if (r == -1) {
                fprintf(stderr, "Can't find a shanty to remove!\n");
                return;
            }
            y = r / WORLD_SIDE_LEN;
            x = r % WORLD_SIDE_LEN;
            remove_a_shanty(x, y);  /* this will fire_area */
        }
    }
}

void do_shanty(int x, int y)
{                               /* just steal some stuff and make pollution. */

    get_food(x, y, SHANTY_GET_FOOD);
    if (get_goods(x, y, SHANTY_GET_GOODS) != 0)
        if ((goods_tax -= SHANTY_GET_GOODS * 2) < 0)
            goods_tax = 0;
    get_ore(x, y, SHANTY_GET_ORE);
    get_steel(x, y, SHANTY_GET_STEEL);
    if (get_jobs(x, y, SHANTY_GET_JOBS) != 0)
        if ((income_tax -= SHANTY_GET_JOBS * 2) < 0)
            income_tax = 0;
    if (get_coal(x, y, SHANTY_GET_COAL) != 0)
        if ((coal_tax -= SHANTY_GET_COAL * 2) < 0)
            coal_tax = 0;
    if (rand()%10 == 1) {
        if ((total_time & 1) == 0)
            MP_POL(x, y)++;
        else
            MP_POL(x + 1, y + 1)++;
    }
}

/*
   // spiral round from startx,starty until we hit something of group group.
   // return the x y coords encoded as x+y*WORLD_SIDE_LEN
   // return -1 if we don't find one.
 */
static int spiral_find_group(int startx, int starty, int group)
{
    int i, j, x, y;
    x = startx;
    y = starty;
    /* let's just do a complete spiral for now, work out the bounds later */
    for (i = 1; i < (WORLD_SIDE_LEN + WORLD_SIDE_LEN); i++) {
        for (j = 0; j < i; j++) {
            x--;
            if (x > 0 && x < WORLD_SIDE_LEN && y > 0 && y < WORLD_SIDE_LEN)
                if (MP_GROUP(x, y) == group)
                    return (x + y * WORLD_SIDE_LEN);
        }
        for (j = 0; j < i; j++) {
            y--;
            if (x > 0 && x < WORLD_SIDE_LEN && y > 0 && y < WORLD_SIDE_LEN)
                if (MP_GROUP(x, y) == group)
                    return (x + y * WORLD_SIDE_LEN);
        }
        i++;
        for (j = 0; j < i; j++) {
            x++;
            if (x > 0 && x < WORLD_SIDE_LEN && y > 0 && y < WORLD_SIDE_LEN)
                if (MP_GROUP(x, y) == group)
                    return (x + y * WORLD_SIDE_LEN);
        }
        for (j = 0; j < i; j++) {
            y++;
            if (x > 0 && x < WORLD_SIDE_LEN && y > 0 && y < WORLD_SIDE_LEN)
                if (MP_GROUP(x, y) == group)
                    return (x + y * WORLD_SIDE_LEN);
        }
    }
    return (-1);
}

/*
   // spiral round from startx,starty until we hit a 2x2 space.
   // return the x y coords encoded as x+y*WORLD_SIDE_LEN
   // return -1 if we don't find one.
 */
static int spiral_find_2x2(int startx, int starty)
{
    int i, j, x, y;
    x = startx;
    y = starty;
    /* let's just do a complete spiral for now, work out the bounds later */
    for (i = 1; i < (WORLD_SIDE_LEN + WORLD_SIDE_LEN); i++) {
        for (j = 0; j < i; j++) {
            x--;
            if (x > 1 && x < WORLD_SIDE_LEN - 2 && y > 1 && y < WORLD_SIDE_LEN - 2)
                if (GROUP_IS_BARE(MP_GROUP(x, y))
                    && GROUP_IS_BARE(MP_GROUP(x + 1, y))
                    && GROUP_IS_BARE(MP_GROUP(x, y + 1))
                    && GROUP_IS_BARE(MP_GROUP(x + 1, y + 1)))
                    return (x + y * WORLD_SIDE_LEN);
        }
        for (j = 0; j < i; j++) {
            y--;
            if (x > 1 && x < WORLD_SIDE_LEN - 2 && y > 1 && y < WORLD_SIDE_LEN - 2)
                if (GROUP_IS_BARE(MP_GROUP(x, y))
                    && GROUP_IS_BARE(MP_GROUP(x + 1, y))
                    && GROUP_IS_BARE(MP_GROUP(x, y + 1))
                    && GROUP_IS_BARE(MP_GROUP(x + 1, y + 1)))
                    return (x + y * WORLD_SIDE_LEN);
        }
        i++;
        for (j = 0; j < i; j++) {
            x++;
            if (x > 1 && x < WORLD_SIDE_LEN - 2 && y > 1 && y < WORLD_SIDE_LEN - 2)
                if (GROUP_IS_BARE(MP_GROUP(x, y))
                    && GROUP_IS_BARE(MP_GROUP(x + 1, y))
                    && GROUP_IS_BARE(MP_GROUP(x, y + 1))
                    && GROUP_IS_BARE(MP_GROUP(x + 1, y + 1)))
                    return (x + y * WORLD_SIDE_LEN);
        }
        for (j = 0; j < i; j++) {
            y++;
            if (x > 1 && x < WORLD_SIDE_LEN - 2 && y > 1 && y < WORLD_SIDE_LEN - 2)
                if (GROUP_IS_BARE(MP_GROUP(x, y))
                    && GROUP_IS_BARE(MP_GROUP(x + 1, y))
                    && GROUP_IS_BARE(MP_GROUP(x, y + 1))
                    && GROUP_IS_BARE(MP_GROUP(x + 1, y + 1)))
                    return (x + y * WORLD_SIDE_LEN);
        }
    }
    return (-1);
}
