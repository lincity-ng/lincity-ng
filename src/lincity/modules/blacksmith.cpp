/* ---------------------------------------------------------------------- *
 * blacksmith.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "modules.h"
#include "blacksmith.h"
//#include "cliglobs.h"

void do_blacksmith(int x, int y)
{
    /*
       // int_1 contains the goods at the blacksmith
       // int_2 contains the goods made - for the animation
       // int_3 contains the coal store
       // int_4 unused
       // int_5 is the % made so far this month
       // int_6 is the % capacity last month
       // int_7 contains the jobs stored at the blacksmith
       // MP_ANIM(x,y) is the animation trigger time (since 1.91)
     */
    if (MP_INFO(x, y).int_3 < MAX_COAL_AT_BLACKSMITH)
        if (get_coal(x, y, BLACKSMITH_GET_COAL) != 0)
            MP_INFO(x, y).int_3 += BLACKSMITH_GET_COAL;

    if (MP_INFO(x, y).int_1 < MAX_GOODS_AT_BLACKSMITH && MP_INFO(x, y).int_3 >= BLACKSMITH_COAL_USED) {
        if (get_steel(x, y, BLACKSMITH_STEEL_USED) != 0) {
            MP_INFO(x, y).int_1 += GOODS_MADE_BY_BLACKSMITH;
            MP_INFO(x, y).int_3 -= BLACKSMITH_COAL_USED;
        }
    }

    if (MP_INFO(x, y).int_7 < BLACKSMITH_JOBS) {
        if (get_jobs(x, y, BLACKSMITH_JOBS))
            MP_INFO(x, y).int_7 = BLACKSMITH_JOBS;
    }

    if (MP_INFO(x, y).int_7 >= BLACKSMITH_JOBS) {
        if (MP_INFO(x, y).int_1 > GOODS_MADE_BY_BLACKSMITH) {
            if (put_goods(x, y, GOODS_MADE_BY_BLACKSMITH - 1) != 0) {
                MP_INFO(x, y).int_1 -= (GOODS_MADE_BY_BLACKSMITH - 1);
                MP_INFO(x, y).int_2 += (GOODS_MADE_BY_BLACKSMITH - 1);
                MP_INFO(x, y).int_5++;
                MP_INFO(x, y).int_7 -= BLACKSMITH_JOBS;
            }
        }
    } else {
        MP_TYPE(x, y) = CST_BLACKSMITH_0;
    }

    if (MP_INFO(x, y).int_2 > BLACKSMITH_BATCH && real_time >= MP_ANIM(x, y)) {
        MP_ANIM(x, y) = real_time + BLACKSMITH_ANIM_SPEED;
        switch (MP_TYPE(x, y)) {
        case (CST_BLACKSMITH_0):
            MP_TYPE(x, y) = CST_BLACKSMITH_1;
            break;
        case (CST_BLACKSMITH_1):
            MP_TYPE(x, y) = CST_BLACKSMITH_2;
            break;
        case (CST_BLACKSMITH_2):
            MP_TYPE(x, y) = CST_BLACKSMITH_3;
            break;
        case (CST_BLACKSMITH_3):
            MP_TYPE(x, y) = CST_BLACKSMITH_4;
            break;
        case (CST_BLACKSMITH_4):
            MP_TYPE(x, y) = CST_BLACKSMITH_5;
            break;
        case (CST_BLACKSMITH_5):
            MP_TYPE(x, y) = CST_BLACKSMITH_6;
            break;
        case (CST_BLACKSMITH_6):
            MP_TYPE(x, y) = CST_BLACKSMITH_1;
            MP_INFO(x, y).int_2 = 0;
            MP_POL(x, y)++;
            break;
        }
    }
    if (total_time % 100 == 0) {
        MP_INFO(x, y).int_6 = MP_INFO(x, y).int_5;
        MP_INFO(x, y).int_5 = 0;
    }
}

void mps_blacksmith(int x, int y)
{
    int i = 0;

    mps_store_title(i++, _("Blacksmith"));
    i++;

    mps_store_sfp(i++, _("Capacity"), MP_INFO(x, y).int_6);
    i++;
    mps_store_title(i++, _("Inventory"));
    mps_store_sddp(i++, _("Goods"), MP_INFO(x, y).int_1, MAX_GOODS_AT_BLACKSMITH);
    mps_store_sddp(i++, _("Coal"), MP_INFO(x, y).int_3, MAX_COAL_AT_BLACKSMITH);
}
