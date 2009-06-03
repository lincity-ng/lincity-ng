/* ---------------------------------------------------------------------- *
 * cricket.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "modules.h"
#include "cricket.h"

void do_cricket(int x, int y)
{
    /*
       // int_1 is the jobs stored at the pavillion
       // int_2 is the goods stored at the pavillion
       // int_3 is the animation flag 
       //
       // MP_ANIM is the time of the next frame since 1.91
     */
    if (MP_INFO(x, y).int_1 < (MAX_JOBS_AT_CRICKET - CRICKET_GET_JOBS))
        if (get_jobs(x, y, CRICKET_GET_JOBS) != 0)
            MP_INFO(x, y).int_1 += CRICKET_GET_JOBS;
    if (MP_INFO(x, y).int_2 < (MAX_GOODS_AT_CRICKET - CRICKET_GET_GOODS))
        if (get_goods(x, y, CRICKET_GET_GOODS) != 0)
            MP_INFO(x, y).int_2 += CRICKET_GET_GOODS;
    /* animate */
    if (MP_INFO(x, y).int_3 && real_time > MP_ANIM(x, y)) {
        MP_ANIM(x, y) = real_time + CRICKET_ANIMATION_SPEED;
        switch (MP_TYPE(x, y)) {
        case (CST_CRICKET_1):
            MP_TYPE(x, y) = CST_CRICKET_2;
            break;
        case (CST_CRICKET_2):
            MP_TYPE(x, y) = CST_CRICKET_3;
            break;
        case (CST_CRICKET_3):
            MP_TYPE(x, y) = CST_CRICKET_4;
            break;
        case (CST_CRICKET_4):
            MP_TYPE(x, y) = CST_CRICKET_5;
            break;
        case (CST_CRICKET_5):
            MP_TYPE(x, y) = CST_CRICKET_6;
            break;
        case (CST_CRICKET_6):
            MP_TYPE(x, y) = CST_CRICKET_7;
            break;
        case (CST_CRICKET_7):
            MP_TYPE(x, y) = CST_CRICKET_1;
            MP_INFO(x, y).int_3 = 0;    /* disable anim */

            break;
        }
    }

    /* That's all. Cover is done by different functions every 3 months or so. */

    cricket_cost += CRICKET_RUNNING_COST;
}

void do_cricket_cover(int x, int y)
{
    int xx, x1, x2, y1, y2;
    if (MP_INFO(x, y).int_1 < (CRICKET_JOBS * DAYS_BETWEEN_COVER) ||
        MP_INFO(x, y).int_2 < (CRICKET_GOODS * DAYS_BETWEEN_COVER))
        return;
    MP_INFO(x, y).int_1 -= (CRICKET_JOBS * DAYS_BETWEEN_COVER);
    MP_INFO(x, y).int_2 -= (CRICKET_GOODS * DAYS_BETWEEN_COVER);
    MP_INFO(x, y).int_3 = 1;    /* turn on animation */

    x1 = x - CRICKET_RANGE;
    if (x1 < 0)
        x1 = 0;
    x2 = x + CRICKET_RANGE;
    if (x2 > WORLD_SIDE_LEN)
        x2 = WORLD_SIDE_LEN;
    y1 = y - CRICKET_RANGE;
    if (y1 < 0)
        y1 = 0;
    y2 = y + CRICKET_RANGE;
    if (y2 > WORLD_SIDE_LEN)
        y2 = WORLD_SIDE_LEN;
    for (; y1 < y2; y1++)
        for (xx = x1; xx < x2; xx++)
            MP_INFO(xx, y1).flags |= FLAG_CRICKET_COVER;
}

void mps_cricket(int x, int y)
{
    int i = 0;

    mps_store_title(i++, _("Sports field"));
    i++;
    mps_store_title(i++, _("Inventory"));
    mps_store_sddp(i++, _("Jobs"), MP_INFO(x, y).int_1, MAX_JOBS_AT_CRICKET);
    mps_store_sddp(i++, _("Goods"), MP_INFO(x, y).int_2, MAX_GOODS_AT_CRICKET);

}
