/* ---------------------------------------------------------------------- *
 * firestation.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "modules.h"
#include "firestation.h"

void do_firestation(int x, int y)
{
    /*
       // int_1 is the jobs stored at the fire station
       // int_2 is the goods stored at the fire station
       // int_3 is the animation flag
       // int_4 unused
       // int_5 is the pause counter
       // MP_ANIM is the time of the next frame since 1.91
     */
    /* XXX: should note whether we actually _produced_ fire cover in int_6 */
    if (MP_INFO(x, y).int_1 < (MAX_JOBS_AT_FIRESTATION - FIRESTATION_GET_JOBS))
        if (get_jobs(x, y, FIRESTATION_GET_JOBS) != 0)
            MP_INFO(x, y).int_1 += FIRESTATION_GET_JOBS;
    if (MP_INFO(x, y).int_2 < (MAX_GOODS_AT_FIRESTATION - FIRESTATION_GET_GOODS))
        if (get_goods(x, y, FIRESTATION_GET_GOODS) != 0)
            MP_INFO(x, y).int_2 += FIRESTATION_GET_GOODS;
    /* animate */
    if (MP_INFO(x, y).int_3 && real_time > MP_ANIM(x, y)) {
        MP_ANIM(x, y) = real_time + FIRESTATION_ANIMATION_SPEED;
        if (MP_INFO(x, y).int_5 > 0)
            MP_INFO(x, y).int_5--;
        else {
            switch (MP_TYPE(x, y)) {
            case (CST_FIRESTATION_1):
                MP_TYPE(x, y) = CST_FIRESTATION_2;
                break;
            case (CST_FIRESTATION_2):
                MP_TYPE(x, y) = CST_FIRESTATION_3;
                break;
            case (CST_FIRESTATION_3):
                MP_TYPE(x, y) = CST_FIRESTATION_4;
                break;
            case (CST_FIRESTATION_4):
                MP_TYPE(x, y) = CST_FIRESTATION_5;
                break;
            case (CST_FIRESTATION_5):
                MP_TYPE(x, y) = CST_FIRESTATION_6;
                break;
            case (CST_FIRESTATION_6):
                MP_TYPE(x, y) = CST_FIRESTATION_7;
                MP_INFO(x, y).int_5 = 10;       /* pause */

                break;
            case (CST_FIRESTATION_7):
                MP_TYPE(x, y) = CST_FIRESTATION_8;
                break;
            case (CST_FIRESTATION_8):
                MP_TYPE(x, y) = CST_FIRESTATION_9;
                break;
            case (CST_FIRESTATION_9):
                MP_TYPE(x, y) = CST_FIRESTATION_10;
                break;
            case (CST_FIRESTATION_10):
                MP_TYPE(x, y) = CST_FIRESTATION_1;
                MP_INFO(x, y).int_3 = 0;        /* stop */

                break;

            }
        }
    }
    /* That's all. Cover is done by different functions every 3 months or so. */

    fire_cost += FIRESTATION_RUNNING_COST;
}

void do_fire_cover(int x, int y)
{
    int xx, x1, x2, y1, y2;
    if (MP_INFO(x, y).int_1 < (FIRESTATION_JOBS * DAYS_BETWEEN_COVER) ||
        MP_INFO(x, y).int_2 < (FIRESTATION_GOODS * DAYS_BETWEEN_COVER))
        return;
    MP_INFO(x, y).int_1 -= (FIRESTATION_JOBS * DAYS_BETWEEN_COVER);
    MP_INFO(x, y).int_2 -= (FIRESTATION_GOODS * DAYS_BETWEEN_COVER);
    MP_INFO(x, y).int_3 = 1;    /* turn on animation */

    x1 = x - FIRESTATION_RANGE;
    if (x1 < 0)
        x1 = 0;
    x2 = x + FIRESTATION_RANGE;
    if (x2 > WORLD_SIDE_LEN)
        x2 = WORLD_SIDE_LEN;
    y1 = y - FIRESTATION_RANGE;
    if (y1 < 0)
        y1 = 0;
    y2 = y + FIRESTATION_RANGE;
    if (y2 > WORLD_SIDE_LEN)
        y2 = WORLD_SIDE_LEN;
    for (; y1 < y2; y1++)
        for (xx = x1; xx < x2; xx++)
            MP_INFO(xx, y1).flags |= FLAG_FIRE_COVER;
}

void mps_firestation(int x, int y)
{
    int i = 0;

    mps_store_title(i++, _("Fire Station"));
    i++;
    mps_store_title(i++, _("Inventory"));
    mps_store_sfp(i++, _("Jobs"), MP_INFO(x, y).int_1 * 100.0 / MAX_JOBS_AT_FIRESTATION);
    mps_store_sfp(i++, _("Goods"), MP_INFO(x, y).int_2 * 100.0 / MAX_GOODS_AT_FIRESTATION);

}
