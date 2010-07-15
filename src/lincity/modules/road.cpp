/* ---------------------------------------------------------------------- *
 * road.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "modules.h"
#include "../transport.h"
#include "road.h"

/* ---------------------------------------------------------------------
   For track, road and rail:

   int_1 contains the amount of food
   int_2 contains the amount of jobs
   int_3 contains the amount of coal
   int_4 contains the amount of goods
   int_5 contains the amount of ore
   int_6 contains the amount of steel
   int_7 contains the amount of waste
  --------------------------------------------------------------------- */

void do_road(int x, int y)
{
    int *pol = &MP_POL(x, y);
    map_point_info_struct *minfo = &MP_INFO(x, y);
    ++transport_cost;
    if (total_time % DAYS_PER_ROAD_POLLUTION == 0)
        *pol += ROAD_POLLUTION;
    if ((total_time & ROAD_GOODS_USED_MASK) == 0 && minfo->int_4 > 0) {
        --minfo->int_4;
        ++minfo->int_7;
    }
    general_transport(x, y, MAX_WASTE_ON_ROAD);
}

void mps_road(int x, int y)
{
    int i = 0;

    mps_store_title(i++, _(main_groups[MP_GROUP(x, y)].name));
    i++;

    mps_store_sddp(i++, _("Food"), MP_INFO(x, y).int_1, MAX_FOOD_ON_ROAD);
    mps_store_sddp(i++, _("Jobs"), MP_INFO(x, y).int_2, MAX_JOBS_ON_ROAD);
    mps_store_sddp(i++, _("Coal"), MP_INFO(x, y).int_3, MAX_COAL_ON_ROAD);
    mps_store_sddp(i++, _("Goods"), MP_INFO(x, y).int_4, MAX_GOODS_ON_ROAD);
    mps_store_sddp(i++, _("Ore"), MP_INFO(x, y).int_5, MAX_ORE_ON_ROAD);
    mps_store_sddp(i++, _("Steel"), MP_INFO(x, y).int_6, MAX_STEEL_ON_ROAD);
    mps_store_sddp(i++, _("Waste"), MP_INFO(x, y).int_7, MAX_WASTE_ON_ROAD);

}

/** @file lincity/modules/road.cpp */

