/* ---------------------------------------------------------------------- *
 * mill.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "modules.h"
#include "mill.h"

void do_mill(int x, int y)
{
       // int_1 contains the goods at the mill
       // int_2 contains the food store
       // int_3 contains the coal store
       // int_4 unused
       // int_5 is the % count so far this month
       // int_6 is the % capacity last month
       // int_7 unused
       // MP_ANIM contains the animation trigger time since 1.91

    /* get food */
    int block_anim = 0;
    if (MP_INFO(x, y).int_2 < MAX_FOOD_AT_MILL)
        if (get_food(x, y, MILL_GET_FOOD) != 0)
            MP_INFO(x, y).int_2 += MILL_GET_FOOD;

    /* get coal or power */
    if (MP_INFO(x, y).int_3 < MAX_COAL_AT_MILL) {
        if (get_coal(x, y, MILL_GET_COAL) != 0)
            MP_INFO(x, y).int_3 += MILL_GET_COAL;
        else if (get_power(x, y, MILL_GET_COAL * MILL_POWER_PER_COAL, 0) != 0)
            MP_INFO(x, y).int_3 += MILL_GET_COAL;
    } else {
        // prevent blinking in minimap: we have power
        get_power(x,y,0,0);
    }

    if (MP_INFO(x, y).int_1 < MAX_GOODS_AT_MILL) {
        if (MP_INFO(x, y).int_2 > FOOD_USED_BY_MILL && MP_INFO(x, y).int_3 > COAL_USED_BY_MILL) {
            if (get_jobs(x, y, MILL_JOBS) != 0) {
                MP_INFO(x, y).int_2 -= FOOD_USED_BY_MILL;
                MP_INFO(x, y).int_3 -= COAL_USED_BY_MILL;
                MP_INFO(x, y).int_1 += GOODS_MADE_BY_MILL;
                MP_INFO(x, y).int_5++;
            } else {
                MP_TYPE(x, y) = CST_MILL_0;
                block_anim = 1;
            }
        } else
            block_anim = 1;
    }

    if (MP_INFO(x, y).int_1 > GOODS_MADE_BY_MILL)
        if (put_goods(x, y, GOODS_MADE_BY_MILL - 1) != 0)
            MP_INFO(x, y).int_1 -= (GOODS_MADE_BY_MILL - 1);

    if (total_time % 100 == 0) {
        MP_INFO(x, y).int_6 = MP_INFO(x, y).int_5;
        MP_INFO(x, y).int_5 = 0;
    }
    if (real_time >= MP_ANIM(x, y) && block_anim == 0) {
        MP_ANIM(x, y) = real_time + MILL_ANIM_SPEED;
        switch (MP_TYPE(x, y)) {
        case (CST_MILL_0):
            MP_TYPE(x, y) = CST_MILL_1;
            break;
        case (CST_MILL_1):
            MP_TYPE(x, y) = CST_MILL_2;
            break;
        case (CST_MILL_2):
            MP_TYPE(x, y) = CST_MILL_3;
            break;
        case (CST_MILL_3):
            MP_TYPE(x, y) = CST_MILL_4;
            break;
        case (CST_MILL_4):
            MP_TYPE(x, y) = CST_MILL_5;
            break;
        case (CST_MILL_5):
            MP_TYPE(x, y) = CST_MILL_6;
            break;
        case (CST_MILL_6):
            MP_TYPE(x, y) = CST_MILL_1;
            MP_POL(x, y)++;
            break;
        }
    }
}

void mps_mill(int x, int y)
{
    int i = 0;
    mps_store_title(i++, _("Textile Mill"));
    i++;
    mps_store_sfp(i++, _("Capacity"), MP_INFO(x, y).int_6);
    i++;
    mps_store_title(i++, _("Inventory"));
    mps_store_sfp(i++, _("Goods"), MP_INFO(x, y).int_1 * 100.0 / MAX_GOODS_AT_MILL);
    mps_store_sfp(i++, _("Food"), MP_INFO(x, y).int_2 * 100.0 / MAX_FOOD_AT_MILL);
    mps_store_sfp(i++, _("Coal"), MP_INFO(x, y).int_3 * 100.0 / MAX_COAL_AT_MILL);

}
