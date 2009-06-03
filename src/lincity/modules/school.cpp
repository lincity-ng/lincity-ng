/* ---------------------------------------------------------------------- *
 * school.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "modules.h"
#include "school.h"

void do_school(int x, int y)
{
    /*
       // int_1 contains the job pool
       // int_2 contains the goods at the school
       // int_3 has the tech points made
       // int_4 is the tech count so far this 100 days
       // int_5 is the tech count last 100 days to give a % of max production
     */
    if (MP_INFO(x, y).int_1 < (MAX_JOBS_AT_SCHOOL - SCHOOL_JOBS))
        if (get_jobs(x, y, SCHOOL_JOBS) != 0)
            MP_INFO(x, y).int_1 += SCHOOL_JOBS;
    if (MP_INFO(x, y).int_2 < (MAX_GOODS_AT_SCHOOL - SCHOOL_GOODS))
        if (get_goods(x, y, SCHOOL_GOODS) != 0)
            MP_INFO(x, y).int_2 += SCHOOL_GOODS;
    if (MP_INFO(x, y).int_1 >= JOBS_MAKE_TECH_SCHOOL && MP_INFO(x, y).int_2 >= GOODS_MAKE_TECH_SCHOOL) {
        MP_INFO(x, y).int_1 -= JOBS_MAKE_TECH_SCHOOL;
        MP_INFO(x, y).int_2 -= GOODS_MAKE_TECH_SCHOOL;
        MP_INFO(x, y).int_3 += TECH_MADE_BY_SCHOOL;
        MP_INFO(x, y).int_4++;
        tech_level += TECH_MADE_BY_SCHOOL;
    }
    school_cost += SCHOOL_RUNNING_COST;
    if ((total_time % 100) == 0) {
        MP_INFO(x, y).int_5 = MP_INFO(x, y).int_4;
        MP_INFO(x, y).int_4 = 0;
    }
}

void mps_school(int x, int y)
{
    int i = 0;
    mps_store_title(i++, _("School"));
    i++;
    mps_store_title(i++, _("Lessons Learned"));
    mps_store_f(i++, MP_INFO(x, y).int_3 * 100.0 / MAX_TECH_LEVEL);
    i++;
    mps_store_title(i++, _("Inventory"));
    mps_store_sddp(i++, _("Jobs"), MP_INFO(x, y).int_1, MAX_JOBS_AT_SCHOOL);
    mps_store_sddp(i++, _("Goods"), MP_INFO(x, y).int_2, MAX_GOODS_AT_SCHOOL);

    mps_store_sfp(i++, _("Capacity"), MP_INFO(x, y).int_5 * 4);

}
