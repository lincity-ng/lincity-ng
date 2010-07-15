/* ---------------------------------------------------------------------- *
 * market.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#include <cstdlib>
#include "modules.h"
#include "market.h"
#include "lincity-ng/ErrorInterface.hpp"
#include "../range.h"
#include "../transport.h"

    /*
     * MP_INFO(x,y)
     *  int_1 contains the food it holds
     *  int_2 contains the jobs
     *  int_3 contains the coal
     *  int_4 contains the goods
     *  int_5 contains the ore
     *  int_6 contains the steel
     *  int_7 contains the waste
     *
     */


int get_jobs(int x, int y, int jobs)
{
    int q;
    if (numof_markets > 0) {
        for (q = 0; q < numof_markets; q++) {
            if ( abs(marketx[q] - x) < MARKET_RANGE && abs(markety[q] - y) < MARKET_RANGE) {
                if ( MP_INFO(marketx[q], markety[q]).int_2 > (3 * jobs / 2) ) {
                    MP_INFO(marketx[q], markety[q]).int_2 -= jobs;
                    income_tax += jobs;
                    MP_INFO(marketx[q] + 1, markety[q]).int_3 += jobs;
                    MP_INFO(x,y).flags &= (0xFFFFFFFF - FLAG_LACK_JOBS);
                    return (1);
                }
            }
        }
    }

    /* second try with transports */
    if (get_stuff(x, y, jobs, T_JOBS) != 0) {
        income_tax += jobs;
        MP_INFO(x,y).flags &= (0xFFFFFFFF - FLAG_LACK_JOBS);
        return (1);
    }

    /* not enough jobs available */
    MP_INFO(x,y).flags |= FLAG_LACK_JOBS;
    return (0);
}

int put_jobs(int x, int y, int jobs)
{
    int q;
    if (numof_markets > 0) {
        for (q = 0; q < numof_markets; q++) {
            if (MP_INFO(marketx[q], markety[q]).int_2 >= (MAX_JOBS_IN_MARKET - jobs))
                continue;
            if (abs(marketx[q] - x) < EMPLOYER_RANGE && abs(markety[q] - y) < EMPLOYER_RANGE) {
                MP_INFO(marketx[q], markety[q]).int_2 += jobs;
                return (1);
            }
        }
    }
    if (put_stuff(x, y, jobs, T_JOBS) != 0) {
        return (1);
    }
    return (0);
}

int get_food(int x, int y, int food)
{
    int q;
    if (numof_markets > 0) {
        for (q = 0; q < numof_markets; q++) {
            if ((abs(marketx[q] - x) < MARKET_RANGE)
                && (abs(markety[q] - y) < MARKET_RANGE)
                && (MP_INFO(marketx[q], markety[q]).int_1 > food)) {
                MP_INFO(marketx[q], markety[q]).int_1 -= food;
                return (1);
            }
        }
    }
    if (get_stuff(x, y, food, T_FOOD) != 0)
        return (1);
    return (0);
}

int put_food(int x, int y, int food)
{
    int q;
    if (numof_markets > 0) {
        for (q = 0; q < numof_markets; q++) {
            if (MP_INFO(marketx[q], markety[q]).int_1 >= (MAX_FOOD_IN_MARKET - food))
                continue;
            if ((abs(marketx[q] - x) < ORG_FARM_RANGE)
                && (abs(markety[q] - y) < ORG_FARM_RANGE)) {
                MP_INFO(marketx[q], markety[q]).int_1 += food;
                return (1);
            }
        }
    }
    if (put_stuff(x, y, food, T_FOOD) != 0)
        return (1);
    return (0);
}

int get_goods(int x, int y, int goods)
{
    int q;
    if (numof_markets > 0) {
        for (q = 0; q < numof_markets; q++) {
            if (abs(marketx[q] - x) < MARKET_RANGE
                && abs(markety[q] - y) < MARKET_RANGE && (MP_INFO(marketx[q], markety[q]).int_4 > goods)) {
                MP_INFO(marketx[q], markety[q]).int_4 -= goods;
                goods_tax += goods;
                goods_used += goods;
                /* make the waste here. */
                MP_INFO(marketx[q], markety[q]).int_7 += goods / 3;
                return (1);
            }
        }
    }
    if (get_stuff(x, y, goods, T_GOODS) != 0) {
        put_stuff(x, y, goods / 3, T_WASTE);
        goods_tax += goods;
        goods_used += goods;
        return (1);
    }
    return (0);
}

int put_goods(int x, int y, int goods)
{
    int q;
    if (numof_markets > 0) {
        for (q = 0; q < numof_markets; q++) {
            if (MP_INFO(marketx[q], markety[q]).int_4 >= (MAX_GOODS_IN_MARKET - goods))
                continue;
            if ((abs(marketx[q] - x) < MARKET_RANGE)
                && (abs(markety[q] - y) < MARKET_RANGE)) {
                MP_INFO(marketx[q], markety[q]).int_4 += goods;
                return (1);
            }
        }
    }
    if (put_stuff(x, y, goods, T_GOODS) != 0)
        return (1);
    return (0);
}

int put_waste(int x, int y, int waste)
{
    int q;
    if (numof_markets > 0) {
        for (q = 0; q < numof_markets; q++) {
            if (MP_INFO(marketx[q], markety[q]).int_7 >= (MAX_WASTE_IN_MARKET - waste))
                continue;
            if ((abs(marketx[q] - x) < MARKET_RANGE)
                && (abs(markety[q] - y) < MARKET_RANGE)) {
                MP_INFO(marketx[q], markety[q]).int_7 += waste;
                return (1);
            }
        }
    }
    if (put_stuff(x, y, waste, T_WASTE) != 0)
        return (1);
    return (0);
}

int get_waste(int x, int y, int waste)
{
    int q;
    if (numof_markets > 0) {
        for (q = 0; q < numof_markets; q++) {
            if ((abs(marketx[q] - x) < MARKET_RANGE)
                && (abs(markety[q] - y) < MARKET_RANGE)
                && (MP_INFO(marketx[q], markety[q]).int_7 > waste)) {
                MP_INFO(marketx[q], markety[q]).int_7 -= waste;
                return (1);
            }
        }
    }
    if (get_stuff(x, y, waste, T_WASTE) != 0)
        return (1);
    return (0);
}

int get_steel(int x, int y, int steel)
{
    int q;
    if (numof_markets > 0) {
        for (q = 0; q < numof_markets; q++) {
            if ((abs(marketx[q] - x) < MARKET_RANGE)
                && (abs(markety[q] - y) < MARKET_RANGE)
                && (MP_INFO(marketx[q], markety[q]).int_6 > steel)) {
                MP_INFO(marketx[q], markety[q]).int_6 -= steel;
                return (1);
            }
        }
    }
    if (get_stuff(x, y, steel, T_STEEL) != 0)
        return (1);
    return (0);
}

int put_steel(int x, int y, int steel)
{
    int q;
    if (numof_markets > 0) {
        for (q = 0; q < numof_markets; q++) {
            if (MP_INFO(marketx[q], markety[q]).int_6 >= (MAX_STEEL_IN_MARKET - steel))
                continue;
            if ((abs(marketx[q] - x) < MARKET_RANGE)
                && (abs(markety[q] - y) < MARKET_RANGE)) {
                MP_INFO(marketx[q], markety[q]).int_6 += steel;
                return (1);
            }
        }
    }
    if (put_stuff(x, y, steel, T_STEEL) != 0)
        return (1);
    return (0);
}

int get_ore(int x, int y, int ore)
{
    int q;
    if (numof_markets > 0) {
        for (q = 0; q < numof_markets; q++) {
            if ((abs(marketx[q] - x) < MARKET_RANGE)
                && (abs(markety[q] - y) < MARKET_RANGE)
                && (MP_INFO(marketx[q], markety[q]).int_5 > ore)) {
                MP_INFO(marketx[q], markety[q]).int_5 -= ore;
                return (1);
            }
        }
    }
    if (get_stuff(x, y, ore, T_ORE) != 0)
        return (1);
    return (0);
}

int put_ore(int x, int y, int ore)
{
    int q;
    if (numof_markets > 0) {
        for (q = 0; q < numof_markets; q++) {
            if (MP_INFO(marketx[q], markety[q]).int_5 >= (MAX_ORE_IN_MARKET - ore))
                continue;
            if ((abs(marketx[q] - x) < MARKET_RANGE)
                && (abs(markety[q] - y) < MARKET_RANGE)) {
                MP_INFO(marketx[q], markety[q]).int_5 += ore;
                return (1);
            }
        }
    }
    if (put_stuff(x, y, ore, T_ORE) != 0)
        return (1);
    return (0);
}

int get_coal(int x, int y, int coal)
{
    int q;
    if (numof_markets > 0) {
        for (q = 0; q < numof_markets; q++) {
            if ((abs(marketx[q] - x) < MARKET_RANGE)
                && (abs(markety[q] - y) < MARKET_RANGE)
                && (MP_INFO(marketx[q], markety[q]).int_3 > coal)) {
                MP_INFO(marketx[q], markety[q]).int_3 -= coal;
                return (1);
            }
        }
    }
    if (get_stuff(x, y, coal, T_COAL) != 0)
        return (1);
    return (0);
}

int put_coal(int x, int y, int coal)
{
    int q;
    if (numof_markets > 0) {
        for (q = 0; q < numof_markets; q++) {
            if (MP_INFO(marketx[q], markety[q]).int_3 >= (MAX_COAL_IN_MARKET - coal))
                continue;
            if ((abs(marketx[q] - x) < MARKET_RANGE)
                && (abs(markety[q] - y) < MARKET_RANGE)) {
                MP_INFO(marketx[q], markety[q]).int_3 += coal;
                return (1);
            }
        }
    }
    if (put_stuff(x, y, coal, T_COAL) != 0)
        return (1);
    return (0);
}

int add_a_market(int x, int y)
{                               /* add to marketx markety to list */
    if (numof_markets >= MAX_NUMOF_MARKETS)
        return (0);
    marketx[numof_markets] = x;
    markety[numof_markets] = y;
    numof_markets++;
    /* oh dear. Got to bootstap markets with jobs, otherwise power won't work
     * GCS: Is this still true?
     * AL1: FIXME it seems not :) it's just needed to wait for periodic event
     */

    MP_INFO(x, y).int_2 = 2000;
    return (1);
}

void remove_a_market(int x, int y)
{
    /* AL1: Could use present jobs to try put_everything elsewhere
     * at least half of the content
     */
    int q;
    for (q = 0; q < numof_markets; q++)
        if (marketx[q] == x && markety[q] == y)
            break;
    for (; q < numof_markets; q++) {
        marketx[q] = marketx[q + 1];
        markety[q] = markety[q + 1];
    }
    numof_markets--;
}

void do_market(int x, int y)
{
    /*
       //  int_1 contains the food it holds
       //  int_2 contains the jobs
       //  int_3 contains the coal
       //  int_4 contains the goods
       //  int_5 contains the ore
       //  int_6 contains the steel
       //  int_7 contains the waste
     */

    int extra_jobs = 0;

    shuffle_markets();

    if (x > 0 && (MP_INFO(x - 1, y).flags & FLAG_IS_TRANSPORT) != 0)
        extra_jobs += deal_with_transport(x, y, x - 1, y);
    if (x > 0 && (MP_INFO(x - 1, y + 1).flags & FLAG_IS_TRANSPORT) != 0)
        extra_jobs += deal_with_transport(x, y, x - 1, y + 1);
    if (y > 0 && (MP_INFO(x, y - 1).flags & FLAG_IS_TRANSPORT) != 0)
        extra_jobs += deal_with_transport(x, y, x, y - 1);
    if (y > 0 && (MP_INFO(x + 1, y - 1).flags & FLAG_IS_TRANSPORT) != 0)
        extra_jobs += deal_with_transport(x, y, x + 1, y - 1);
    if (x < WORLD_SIDE_LEN - 2 && (MP_INFO(x + 2, y).flags & FLAG_IS_TRANSPORT) != 0)
        extra_jobs += deal_with_transport(x, y, x + 2, y);
    if (x < WORLD_SIDE_LEN - 2 && (MP_INFO(x + 2, y + 1).flags & FLAG_IS_TRANSPORT) != 0)
        extra_jobs += deal_with_transport(x, y, x + 2, y + 1);
    if (y < WORLD_SIDE_LEN - 2 && (MP_INFO(x, y + 2).flags & FLAG_IS_TRANSPORT) != 0)
        extra_jobs += deal_with_transport(x, y, x, y + 2);
    if (y < WORLD_SIDE_LEN - 2 && (MP_INFO(x + 1, y + 2).flags & FLAG_IS_TRANSPORT) != 0)
        extra_jobs += deal_with_transport(x, y, x + 1, y + 2);

    if (MP_INFO(x, y).int_1 > MAX_FOOD_IN_MARKET)
        MP_INFO(x, y).int_1 = MAX_FOOD_IN_MARKET;
    if (MP_INFO(x, y).int_2 > MAX_JOBS_IN_MARKET)
        MP_INFO(x, y).int_2 = MAX_JOBS_IN_MARKET;
    if (MP_INFO(x, y).int_4 > MAX_GOODS_IN_MARKET)
        MP_INFO(x, y).int_4 = MAX_GOODS_IN_MARKET;

    /* now choose a graphic only dependent on food (for now anyway) */
    if (total_time % 25 == 17) {
        if (MP_INFO(x, y).int_1 <= 0) {
            if (MP_INFO(x, y).int_2 > 0)
                MP_TYPE(x, y) = CST_MARKET_LOW;
            else
                MP_TYPE(x, y) = CST_MARKET_EMPTY;
        } else if (MP_INFO(x, y).int_1 < (MAX_FOOD_IN_MARKET / 2))
            MP_TYPE(x, y) = CST_MARKET_LOW;
        else if (MP_INFO(x, y).int_1 < (MAX_FOOD_IN_MARKET - MAX_FOOD_IN_MARKET / 4))
            MP_TYPE(x, y) = CST_MARKET_MED;
        else
            MP_TYPE(x, y) = CST_MARKET_FULL;
    }

    /* now employ some people */
    get_jobs(x, y, 1 + (extra_jobs / 5));

    /* keep the pbars accurate */
    inventory(x, y);
}

void shuffle_markets(void)
{
    register int x;
    int q, r, m;
    m = (numof_markets / 4) + 1;
    for (x = 0; x < m; x++) {
        r = rand() % numof_markets;
        if (r == x)
            continue;
        q = marketx[x];
        marketx[x] = marketx[r];
        marketx[r] = q;
        q = markety[x];
        markety[x] = markety[r];
        markety[r] = q;
    }
}

int deal_with_transport(int x, int y, int tx, int ty)
{
    int i, r, extra_jobs = 3, flags;
    flags = MP_INFO(x, y).flags;
    /* tracks */
    if (MP_GROUP(tx, ty) == GROUP_TRACK) {
        /* food */
        if ((flags & FLAG_MB_FOOD) != 0) {
            r = (MAX_FOOD_IN_MARKET * 1000)
                / (MAX_FOOD_ON_TRACK + MAX_FOOD_IN_MARKET);
            i = MP_INFO(x, y).int_1 + MP_INFO(tx, ty).int_1;
            MP_INFO(x, y).int_1 = (i * r) / 1000;
            MP_INFO(tx, ty).int_1 = i - MP_INFO(x, y).int_1;
        }

        /* jobs */
        if ((flags & FLAG_MB_JOBS) != 0) {
            r = (MAX_JOBS_IN_MARKET * 1000)
                / (MAX_JOBS_ON_TRACK + MAX_JOBS_IN_MARKET);
            i = MP_INFO(x, y).int_2 + MP_INFO(tx, ty).int_2;
            MP_INFO(x, y).int_2 = (i * r) / 1000;
            MP_INFO(tx, ty).int_2 = i - MP_INFO(x, y).int_2;
        }

        /* coal */
        if ((flags & FLAG_MB_COAL) != 0) {
            r = (MAX_COAL_IN_MARKET * 1000)
                / (MAX_COAL_ON_TRACK + MAX_COAL_IN_MARKET);
            i = MP_INFO(x, y).int_3 + MP_INFO(tx, ty).int_3;
            MP_INFO(x, y).int_3 = (i * r) / 1000;
            MP_INFO(tx, ty).int_3 = i - MP_INFO(x, y).int_3;
        }

        /* goods */
        if ((flags & FLAG_MB_GOODS) != 0) {
            r = (MAX_GOODS_IN_MARKET * 1000)
                / (MAX_GOODS_ON_TRACK + MAX_GOODS_IN_MARKET);
            i = MP_INFO(x, y).int_4 + MP_INFO(tx, ty).int_4;
            MP_INFO(x, y).int_4 = (i * r) / 1000;
            MP_INFO(tx, ty).int_4 = i - MP_INFO(x, y).int_4;
        }

        /* ore */
        if ((flags & FLAG_MB_ORE) != 0) {
            r = (MAX_ORE_IN_MARKET * 1000)
                / (MAX_ORE_ON_TRACK + MAX_ORE_IN_MARKET);
            i = MP_INFO(x, y).int_5 + MP_INFO(tx, ty).int_5;
            MP_INFO(x, y).int_5 = (i * r) / 1000;
            MP_INFO(tx, ty).int_5 = i - MP_INFO(x, y).int_5;
        }

        /* steel */
        if ((flags & FLAG_MB_STEEL) != 0) {
            r = (MAX_STEEL_IN_MARKET * 1000)
                / (MAX_STEEL_ON_TRACK + MAX_STEEL_IN_MARKET);
            i = MP_INFO(x, y).int_6 + MP_INFO(tx, ty).int_6;
            MP_INFO(x, y).int_6 = (i * r) / 1000;
            MP_INFO(tx, ty).int_6 = i - MP_INFO(x, y).int_6;
        }

        /* waste */
        r = (MAX_WASTE_IN_MARKET * 1000)
            / (MAX_WASTE_ON_TRACK + MAX_WASTE_IN_MARKET);
        i = MP_INFO(x, y).int_7 + MP_INFO(tx, ty).int_7;
        MP_INFO(x, y).int_7 = (i * r) / 1000;
        MP_INFO(tx, ty).int_7 = i - MP_INFO(x, y).int_7;

        /* if it's full of waste, burn some to make pollution. */
        if (MP_INFO(x, y).int_7 >= (99 * MAX_WASTE_IN_MARKET / 100)) {
            MP_POL(x, y) += 3000;
            MP_INFO(x, y).int_7 -= (7 * MAX_WASTE_IN_MARKET) / 10;
        }
    }

    /* do rail traffic */
    else if (MP_GROUP(tx, ty) == GROUP_RAIL) {
        /* food */
        if ((flags & FLAG_MB_FOOD) != 0) {
            r = (MAX_FOOD_IN_MARKET * 1000)
                / (MAX_FOOD_ON_RAIL + MAX_FOOD_IN_MARKET);
            i = MP_INFO(x, y).int_1 + MP_INFO(tx, ty).int_1;
            MP_INFO(x, y).int_1 = (i * r) / 1000;
            MP_INFO(tx, ty).int_1 = i - MP_INFO(x, y).int_1;
        }

        /* jobs */
        if ((flags & FLAG_MB_JOBS) != 0) {
            r = (MAX_JOBS_IN_MARKET * 1000)
                / (MAX_JOBS_ON_RAIL + MAX_JOBS_IN_MARKET);
            i = MP_INFO(x, y).int_2 + MP_INFO(tx, ty).int_2;
            MP_INFO(x, y).int_2 = (i * r) / 1000;
            MP_INFO(tx, ty).int_2 = i - MP_INFO(x, y).int_2;
        }

        /* coal */
        if ((flags & FLAG_MB_COAL) != 0) {
            r = (MAX_COAL_IN_MARKET * 1000)
                / (MAX_COAL_ON_RAIL + MAX_COAL_IN_MARKET);
            i = MP_INFO(x, y).int_3 + MP_INFO(tx, ty).int_3;
            MP_INFO(x, y).int_3 = (i * r) / 1000;
            MP_INFO(tx, ty).int_3 = i - MP_INFO(x, y).int_3;
        }

        /* goods */
        if ((flags & FLAG_MB_GOODS) != 0) {
            r = (MAX_GOODS_IN_MARKET * 1000)
                / (MAX_GOODS_ON_RAIL + MAX_GOODS_IN_MARKET);
            i = MP_INFO(x, y).int_4 + MP_INFO(tx, ty).int_4;
            MP_INFO(x, y).int_4 = (i * r) / 1000;
            MP_INFO(tx, ty).int_4 = i - MP_INFO(x, y).int_4;
        }

        /* ore */
        if ((flags & FLAG_MB_ORE) != 0) {
            r = (MAX_ORE_IN_MARKET * 1000)
                / (MAX_ORE_ON_RAIL + MAX_ORE_IN_MARKET);
            i = MP_INFO(x, y).int_5 + MP_INFO(tx, ty).int_5;
            MP_INFO(x, y).int_5 = (i * r) / 1000;
            MP_INFO(tx, ty).int_5 = i - MP_INFO(x, y).int_5;
        }

        /* steel */
        if ((flags & FLAG_MB_STEEL) != 0) {
            r = (MAX_STEEL_IN_MARKET * 1000)
                / (MAX_STEEL_ON_RAIL + MAX_STEEL_IN_MARKET);
            i = MP_INFO(x, y).int_6 + MP_INFO(tx, ty).int_6;
            MP_INFO(x, y).int_6 = (i * r) / 1000;
            MP_INFO(tx, ty).int_6 = i - MP_INFO(x, y).int_6;
        }

        /* waste */
        r = (MAX_WASTE_IN_MARKET * 1000)
            / (MAX_WASTE_ON_RAIL + MAX_WASTE_IN_MARKET);
        i = MP_INFO(x, y).int_7 + MP_INFO(tx, ty).int_7;
        MP_INFO(x, y).int_7 = (i * r) / 1000;
        MP_INFO(tx, ty).int_7 = i - MP_INFO(x, y).int_7;

    }

    /* do road traffic */
    else if (MP_GROUP(tx, ty) == GROUP_ROAD) {
        /* food */
        if ((flags & FLAG_MB_FOOD) != 0) {
            r = (MAX_FOOD_IN_MARKET * 1000)
                / (MAX_FOOD_ON_ROAD + MAX_FOOD_IN_MARKET);
            i = MP_INFO(x, y).int_1 + MP_INFO(tx, ty).int_1;
            MP_INFO(x, y).int_1 = (i * r) / 1000;
            MP_INFO(tx, ty).int_1 = i - MP_INFO(x, y).int_1;
        }

        /* jobs */
        if ((flags & FLAG_MB_JOBS) != 0) {
            r = (MAX_JOBS_IN_MARKET * 1000)
                / (MAX_JOBS_ON_ROAD + MAX_JOBS_IN_MARKET);
            i = MP_INFO(x, y).int_2 + MP_INFO(tx, ty).int_2;
            MP_INFO(x, y).int_2 = (i * r) / 1000;
            MP_INFO(tx, ty).int_2 = i - MP_INFO(x, y).int_2;
        }

        /* coal */
        if ((flags & FLAG_MB_COAL) != 0) {
            r = (MAX_COAL_IN_MARKET * 1000)
                / (MAX_COAL_ON_ROAD + MAX_COAL_IN_MARKET);
            i = MP_INFO(x, y).int_3 + MP_INFO(tx, ty).int_3;
            MP_INFO(x, y).int_3 = (i * r) / 1000;
            MP_INFO(tx, ty).int_3 = i - MP_INFO(x, y).int_3;
        }

        /* goods */
        if ((flags & FLAG_MB_GOODS) != 0) {
            r = (MAX_GOODS_IN_MARKET * 1000)
                / (MAX_GOODS_ON_ROAD + MAX_GOODS_IN_MARKET);
            i = MP_INFO(x, y).int_4 + MP_INFO(tx, ty).int_4;
            MP_INFO(x, y).int_4 = (i * r) / 1000;
            MP_INFO(tx, ty).int_4 = i - MP_INFO(x, y).int_4;
        }

        /* ore */
        if ((flags & FLAG_MB_ORE) != 0) {
            r = (MAX_ORE_IN_MARKET * 1000)
                / (MAX_ORE_ON_ROAD + MAX_ORE_IN_MARKET);
            i = MP_INFO(x, y).int_5 + MP_INFO(tx, ty).int_5;
            MP_INFO(x, y).int_5 = (i * r) / 1000;
            MP_INFO(tx, ty).int_5 = i - MP_INFO(x, y).int_5;
        }

        /* steel */
        if ((flags & FLAG_MB_STEEL) != 0) {
            r = (MAX_STEEL_IN_MARKET * 1000)
                / (MAX_STEEL_ON_ROAD + MAX_STEEL_IN_MARKET);
            i = MP_INFO(x, y).int_6 + MP_INFO(tx, ty).int_6;
            MP_INFO(x, y).int_6 = (i * r) / 1000;
            MP_INFO(tx, ty).int_6 = i - MP_INFO(x, y).int_6;
        }

        /* waste */
        r = (MAX_WASTE_IN_MARKET * 1000)
            / (MAX_WASTE_ON_ROAD + MAX_WASTE_IN_MARKET);
        i = MP_INFO(x, y).int_7 + MP_INFO(tx, ty).int_7;
        MP_INFO(x, y).int_7 = (i * r) / 1000;
        MP_INFO(tx, ty).int_7 = i - MP_INFO(x, y).int_7;
    } else
        extra_jobs = 0;

    return (extra_jobs);
}

int get_stuff(int x, int y, int stuff, int stuff_type)
{
    int res = 0;
    map_point_info_struct *minfo = &MP_INFO(x, y);

    switch (MP_SIZE(x, y)) {
    case 2:
        res = get_stuff2(minfo, stuff, stuff_type);
        break;
    case 3:
        res = get_stuff3(minfo, stuff, stuff_type);
        break;
    case 4:
        res = get_stuff4(minfo, stuff, stuff_type);
        break;
    default:
        do_error("Bad area size in get_stuff()");
    }
    return (res);
}

static const int t2[8] = {
    -1,
    WORLD_SIDE_LEN - 1,
    2 * WORLD_SIDE_LEN,
    2 * WORLD_SIDE_LEN + 1,
    WORLD_SIDE_LEN + 2,
    2,
    1 - WORLD_SIDE_LEN,
    -WORLD_SIDE_LEN
};

/* worth inlining -- (ThMO) */
int get_stuff2(map_point_info_struct * map, int stuff, int stuff_type)
{
    static int tstart2 = 0;
    int i, st, tst, *ip, *stack[8], **ssp;      /* stack is a pipe -- (ThMO) */

    /* we'll stack our found pointers so to avoid re-looping and
     * testing again (ThMO) */

    tst = tstart2;

    /* can we find enough on the transport? */

    for (ssp = stack, st = 0, i = 0; i < 8; i++) {
        if (map[t2[tst]].flags & FLAG_IS_TRANSPORT) {
            ip = &map[t2[tst]].int_1;
            ip += stuff_type;
            st += *ip;
            *ssp++ = ip;        /* push it -- (ThMO) */
            if (st >= stuff) {
                ssp = stack;
                do {
                    ip = *ssp++;        /* pop it -- (ThMO) */
                    *ip = (stuff -= *ip) < 0 ? -stuff : 0;
                }
                while (stuff > 0);
                tstart2 = ++tst & 7;
                return (1);
            }
        }
        if (++tst >= 8)
            tst = 0;
    }
    return (0);
}

static const int t3[12] = {
    -1,
    WORLD_SIDE_LEN - 1,
    2 * WORLD_SIDE_LEN - 1,
    3 * WORLD_SIDE_LEN,
    3 * WORLD_SIDE_LEN + 1,
    3 * WORLD_SIDE_LEN + 2,
    2 * WORLD_SIDE_LEN + 3,
    WORLD_SIDE_LEN + 3,
    3,
    2 - WORLD_SIDE_LEN,
    1 - WORLD_SIDE_LEN,
    -WORLD_SIDE_LEN
};

/* worth inlining -- (ThMO) */
int get_stuff3(map_point_info_struct * map, int stuff, int stuff_type)
{
    static int tstart3 = 0;

    int i, st, tst, *ip, *stack[12], **ssp;     /* stack is a pipe -- (ThMO) */

    /* we'll stack our found pointers so to avoid re-looping and
     * testing again
     * (ThMO)
     */

    tst = tstart3;

    /* can we find enough on the transport? */

    for (ssp = stack, st = 0, i = 0; i < 12; i++) {
        if (map[t3[tst]].flags & FLAG_IS_TRANSPORT) {
            ip = &map[t3[tst]].int_1;
            ip += stuff_type;
            st += *ip;
            *ssp++ = ip;        /* push it -- (ThMO) */
            if (st >= stuff) {
                ssp = stack;
                do {
                    ip = *ssp++;        /* pop it -- (ThMO) */
                    *ip = (stuff -= *ip) < 0 ? -stuff : 0;
                }
                while (stuff > 0);
                ++tst;
                tstart3 = tst >= 12 ? 0 : tst;
                return (1);
            }
        }
        if (++tst >= 12)
            tst = 0;
    }
    return (0);
}

static const int t4[16] = {
    -1,
    WORLD_SIDE_LEN - 1,
    2 * WORLD_SIDE_LEN - 1,
    3 * WORLD_SIDE_LEN - 1,
    4 * WORLD_SIDE_LEN,
    4 * WORLD_SIDE_LEN + 1,
    4 * WORLD_SIDE_LEN + 2,
    4 * WORLD_SIDE_LEN + 3,
    3 * WORLD_SIDE_LEN + 4,
    2 * WORLD_SIDE_LEN + 4,
    WORLD_SIDE_LEN + 4,
    4,
    3 - WORLD_SIDE_LEN,
    2 - WORLD_SIDE_LEN,
    1 - WORLD_SIDE_LEN,
    -WORLD_SIDE_LEN
};

/* worth inlining -- (ThMO) */
int get_stuff4(map_point_info_struct * map, int stuff, int stuff_type)
{
    static int tstart4 = 0;

    int i, st, tst, *ip, *stack[16], **ssp;     /* stack is a pipe -- (ThMO) */

    /* we'll stack our found pointers so to avoid re-looping and
     * testing again
     * (ThMO)
     */

    tst = tstart4;

    /* can we find enough on the transport? */

    for (ssp = stack, st = 0, i = 0; i < 16; i++) {
        if (map[t4[tst]].flags & FLAG_IS_TRANSPORT) {
            ip = &map[t4[tst]].int_1;
            ip += stuff_type;
            st += *ip;
            *ssp++ = ip;        /* push it -- (ThMO) */
            if (st >= stuff) {
                ssp = stack;
                do {
                    ip = *ssp++;        /* pop it -- (ThMO) */
                    *ip = (stuff -= *ip) < 0 ? -stuff : 0;
                }
                while (stuff > 0);
                tstart4 = ++tst & 15;
                return (1);
            }
        }
        if (++tst >= 16)
            tst = 0;
    }
    return (0);
}

int put_stuff(int x, int y, int stuff, int stuff_type)
{
    int res = 0;
    short *type = &MP_TYPE(x, y);
    map_point_info_struct *minfo = &MP_INFO(x, y);
    switch (MP_SIZE(x, y)) {
    case 2:
        res = put_stuff2(minfo, type, stuff, stuff_type);
        break;
    case 3:
        res = put_stuff3(minfo, type, stuff, stuff_type);
        break;
    case 4:
        res = put_stuff4(minfo, type, stuff, stuff_type);
        break;
    default:
        do_error("Bad area size in put_stuff()");
    }
    return res;
}

int tmax[3][7] = {
    {MAX_FOOD_ON_TRACK, MAX_JOBS_ON_TRACK, MAX_COAL_ON_TRACK, MAX_GOODS_ON_TRACK, MAX_ORE_ON_TRACK, MAX_STEEL_ON_TRACK,
     MAX_WASTE_ON_TRACK},
    {MAX_FOOD_ON_ROAD, MAX_JOBS_ON_ROAD, MAX_COAL_ON_ROAD, MAX_GOODS_ON_ROAD, MAX_ORE_ON_ROAD, MAX_STEEL_ON_ROAD,
     MAX_WASTE_ON_ROAD},
    {MAX_FOOD_ON_RAIL, MAX_JOBS_ON_RAIL, MAX_COAL_ON_RAIL, MAX_GOODS_ON_RAIL, MAX_ORE_ON_RAIL, MAX_STEEL_ON_RAIL,
     MAX_WASTE_ON_RAIL}
};

struct stack {
    int *ip, max_val;
};

/* worth inlining -- (ThMO) */
int put_stuff2(map_point_info_struct * minfo, short *type, int stuff, int stuff_type)
{
    static int tstart2 = 0;
    int i, st, tst, *ip, tp = 0;
    struct stack stack[8], *ssp;        /* stack is really a pipe -- (ThMO) */

    /* we'll stack our found pointers so to avoid re-looping and
     * testing again (ThMO) */

    tst = tstart2;

    /* can we put enough on the transport? */

    for (ssp = stack, st = 0, i = 0; i < 8; i++) {
        int map_index = t2[tst];
        if (minfo[map_index].flags & FLAG_IS_TRANSPORT) {
            switch (get_group_of_type(type[map_index])) {
            case GROUP_TRACK:
            case GROUP_TRACK_BRIDGE:
                tp = tmax[0][stuff_type];
                break;
            case GROUP_ROAD:
            case GROUP_ROAD_BRIDGE:
                tp = tmax[1][stuff_type];
                break;
            case GROUP_RAIL:
            case GROUP_RAIL_BRIDGE:
                tp = tmax[2][stuff_type];
                break;
            default:
                do_error("Bad transport type in put_stuff2");
            }
            ip = &minfo[map_index].int_1;
            ssp->ip = ip += stuff_type; /* push it -- (ThMO) */
            ssp++->max_val = tp;
            st += tp - *ip;
            if (st >= stuff) {
                ssp = stack;
                do {
                    tp = ssp->max_val - *ssp->ip;       /* pop it -- (ThMO) */
                    if ((stuff -= tp) < 0)
                        *ssp->ip += tp + stuff; /* == orig. stuff -- (ThMO) */
                    else
                        *ssp->ip = ssp->max_val;
                    ++ssp;
                } while (stuff > 0);
                tstart2 = ++tst & 7;
                return (1);
            }
        }
        if (++tst >= 8)
            tst = 0;
    }
    return (0);
}

/* worth inlining -- (ThMO) */
int put_stuff3(map_point_info_struct * minfo, short *type, int stuff, int stuff_type)
{
    static int tstart3 = 0;
    int i, st, tst, *ip, tp = 0;
    struct stack stack[12], *ssp;       /* stack is really a pipe -- (ThMO)
                                           we'll stack our found pointers so
                                           to avoid re-looping and
                                           testing again (ThMO) */

    tst = tstart3;

    /* can we put enough on the transport? */

    for (ssp = stack, st = 0, i = 0; i < 12; i++) {
        int map_index = t3[tst];
        if (minfo[map_index].flags & FLAG_IS_TRANSPORT) {
            switch (get_group_of_type(type[map_index])) {
            case GROUP_TRACK:
            case GROUP_TRACK_BRIDGE:
                tp = tmax[0][stuff_type];
                break;
            case GROUP_ROAD:
            case GROUP_ROAD_BRIDGE:
                tp = tmax[1][stuff_type];
                break;
            case GROUP_RAIL:
            case GROUP_RAIL_BRIDGE:
                tp = tmax[2][stuff_type];
                break;
            default:
                do_error("Bad transport type in put_stuff3");
            }
            ip = &minfo[map_index].int_1;
            ssp->ip = ip += stuff_type; /* push it -- (ThMO) */
            ssp++->max_val = tp;
            st += tp - *ip;
            if (st >= stuff) {
                ssp = stack;
                do {
                    tp = ssp->max_val - *ssp->ip;       /* pop it -- (ThMO) */
                    if ((stuff -= tp) < 0)
                        *ssp->ip += tp + stuff; /* == orig. stuff -- (ThMO) */
                    else
                        *ssp->ip = ssp->max_val;
                    ++ssp;
                } while (stuff > 0);
                ++tst;
                tstart3 = tst >= 12 ? 0 : tst;
                return (1);
            }
        }
        if (++tst >= 12)
            tst = 0;
    }
    return (0);
}

/* worth inlining -- (ThMO) */
int put_stuff4(map_point_info_struct * minfo, short *type, int stuff, int stuff_type)
{
    static int tstart4 = 0;
    int i, st, tst, *ip, tp = 0;
    struct stack stack[16], *ssp;       /* stack is really a pipe -- (ThMO)
                                         */
    /* we'll stack our found pointers so to avoid re-looping and
     * testing again (ThMO) */

    tst = tstart4;

    /* can we put enough on the transport? */

    for (ssp = stack, st = 0, i = 0; i < 16; i++) {
        int map_index = t4[tst];
        if (minfo[map_index].flags & FLAG_IS_TRANSPORT) {
            switch (get_group_of_type(type[map_index])) {
            case GROUP_TRACK:
            case GROUP_TRACK_BRIDGE:
                tp = tmax[0][stuff_type];
                break;
            case GROUP_ROAD:
            case GROUP_ROAD_BRIDGE:
                tp = tmax[1][stuff_type];
                break;
            case GROUP_RAIL:
            case GROUP_RAIL_BRIDGE:
                tp = tmax[2][stuff_type];
                break;
            default:
                do_error("Bad transport type in put_stuff4");
            }
            ip = &minfo[map_index].int_1;
            ssp->ip = ip += stuff_type; /* push it -- (ThMO) */
            ssp++->max_val = tp;
            st += tp - *ip;
            if (st >= stuff) {
                ssp = stack;
                do {
                    tp = ssp->max_val - *ssp->ip;       /* pop it -- (ThMO) */
                    if ((stuff -= tp) < 0)
                        *ssp->ip += tp + stuff; /* == orig. stuff -- (ThMO) */
                    else
                        *ssp->ip = ssp->max_val;
                    ++ssp;
                } while (stuff > 0);
                tstart4 = ++tst & 15;
                return (1);
            }
        }
        if (++tst >= 16)
            tst = 0;
    }
    return (0);
}

void mps_market(int x, int y)
{
    int i = 0;

    mps_store_title(i++, _("Market"));

    i++;

	mps_store_sddp(i++, _("Food"), MP_INFO(x, y).int_1, MAX_FOOD_IN_MARKET);
    mps_store_sddp(i++, _("Jobs"), MP_INFO(x, y).int_2, MAX_JOBS_IN_MARKET);
    mps_store_sddp(i++, _("Coal"), MP_INFO(x, y).int_3, MAX_COAL_IN_MARKET);
    mps_store_sddp(i++, _("Goods"), MP_INFO(x, y).int_4, MAX_GOODS_IN_MARKET);
    mps_store_sddp(i++, _("Ore"), MP_INFO(x, y).int_5, MAX_ORE_IN_MARKET);
    mps_store_sddp(i++, _("Steel"), MP_INFO(x, y).int_6, MAX_STEEL_IN_MARKET);
    mps_store_sddp(i++, _("Waste"), MP_INFO(x, y).int_7, MAX_WASTE_IN_MARKET);
}

/** @file lincity/modules/market.cpp */

