/* ---------------------------------------------------------------------- *
 * transport.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "lin-city.h"
#include "lctypes.h"
#include "transport.h"
#include "power.h"
#include "stats.h"              /* for transport_cost */

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

void general_transport(Map_Point_Info * minfo, int *pol, int max_waste, int *waste_count)
{
    int tot, av, *base, xm1, xp1, ym1, yp1;

    /* 30. Oct 1996:
     * we'll use a loop with pointers here instead of doin' each
     * operation by hand.  this reduces code complexity and should
     * lead to a higher cache hit ratio - theoretically
     * (ThMO)
     *
     * 12. Dec 1996:
     * as this is a heavy used routine, another speedup improvement is
     * needed.  we'll now use 1 pointer, which will be incremented and
     * 1 .. 4 constant indices, which replaces the old pointer-pure
     * version.
     * advantages:  elimination of unnecessary pointer increments.
     * Note:  this *only* works, if the related addresses use one and the
     *        same address space - which is naturally for 2-dimensional
     *        arrays.
     * (ThMO)
     */

    base = &minfo->int_1;
    switch (minfo->flags & 0x0F) {
    case 0:
        return;

    case 1:                    /* inlined t_av_l() -- (ThMO) */
        xm1 = &minfo[-WORLD_SIDE_LEN].int_1 - base;
        do {
            tot = *base + base[xm1];
            av = tot / 2;
            base[xm1] = av;
            *base++ = av + tot % 2;
        } while (base <= &minfo->int_7);
        break;

    case 2:                    /* inlined t_av_u() -- (ThMO) */
        ym1 = &minfo[-1].int_1 - base;
        do {
            tot = *base + base[ym1];
            av = tot / 2;
            base[ym1] = av;
            *base++ = av + tot % 2;
        } while (base <= &minfo->int_7);
        break;

    case 3:                    /* inlined t_av_lu() -- (ThMO) */
        xm1 = &minfo[-WORLD_SIDE_LEN].int_1 - base;
        ym1 = &minfo[-1].int_1 - base;
        do {
            tot = *base + base[xm1] + base[ym1];
            av = tot / 3;
            base[xm1] = base[ym1] = av;
            *base++ = av + tot % 3;
        } while (base <= &minfo->int_7);
        break;

    case 4:                    /* inlined t_av_r() -- (ThMO) */
        xp1 = &minfo[WORLD_SIDE_LEN].int_1 - base;
        do {
            tot = *base + base[xp1];
            av = tot / 2;
            base[xp1] = av;
            *base++ = av + tot % 2;
        } while (base <= &minfo->int_7);
        break;

    case 5:                    /* inlined t_av_lr() -- (ThMO) */
        xm1 = &minfo[-WORLD_SIDE_LEN].int_1 - base;
        xp1 = &minfo[WORLD_SIDE_LEN].int_1 - base;
        do {
            tot = *base + base[xm1] + base[xp1];
            av = tot / 3;
            base[xm1] = base[xp1] = av;
            *base++ = av + tot % 3;
        } while (base <= &minfo->int_7);
        break;

    case 6:                    /* inline t_av_ur() -- (ThMO) */
        ym1 = &minfo[-1].int_1 - base;
        xp1 = &minfo[WORLD_SIDE_LEN].int_1 - base;
        do {
            tot = *base + base[ym1] + base[xp1];
            av = tot / 3;
            base[ym1] = base[xp1] = av;
            *base++ = av + tot % 3;
        } while (base <= &minfo->int_7);
        break;

    case 7:                    /* inlined t_av_lur() -- (ThMO) */
        xm1 = &minfo[-WORLD_SIDE_LEN].int_1 - base;
        ym1 = &minfo[-1].int_1 - base;
        xp1 = &minfo[WORLD_SIDE_LEN].int_1 - base;
        do {
            tot = *base + base[xm1] + base[ym1] + base[xp1];
            av = tot / 4;
            base[xm1] = base[ym1] = base[xp1] = av;
            *base++ = av + tot % 4;
        } while (base <= &minfo->int_7);
        break;

    case 8:                    /* inlined t_av_d() -- (ThMO) */
        yp1 = &minfo[1].int_1 - base;
        do {
            tot = *base + base[yp1];
            av = tot / 2;
            base[yp1] = av;
            *base++ = av + tot % 2;
        } while (base <= &minfo->int_7);
        break;

    case 9:                    /* inlined t_av_ld() -- (ThMO) */
        xm1 = &minfo[-WORLD_SIDE_LEN].int_1 - base;
        yp1 = &minfo[1].int_1 - base;
        do {
            tot = *base + base[xm1] + base[yp1];
            av = tot / 3;
            base[xm1] = base[yp1] = av;
            *base++ = av + tot % 3;
        } while (base <= &minfo->int_7);
        break;

    case 10:                   /* inlined t_av_ud() -- (ThMO) */
        ym1 = &minfo[-1].int_1 - base;
        yp1 = &minfo[1].int_1 - base;
        do {
            tot = *base + base[ym1] + base[yp1];
            av = tot / 3;
            base[ym1] = base[yp1] = av;
            *base++ = av + tot % 3;
        } while (base <= &minfo->int_7);
        break;

    case 11:                   /* inlined t_av_lud() -- (ThMO) */
        xm1 = &minfo[-WORLD_SIDE_LEN].int_1 - base;
        ym1 = &minfo[-1].int_1 - base;
        yp1 = &minfo[1].int_1 - base;
        do {
            tot = *base + base[xm1] + base[ym1] + base[yp1];
            av = tot / 4;
            base[xm1] = base[ym1] = base[yp1] = av;
            *base++ = av + tot % 4;
        } while (base <= &minfo->int_7);
        break;

    case 12:                   /* inlined t_av_rd() -- (ThMO) */
        xp1 = &minfo[WORLD_SIDE_LEN].int_1 - base;
        yp1 = &minfo[1].int_1 - base;
        do {
            tot = *base + base[xp1] + base[yp1];
            av = tot / 3;
            base[xp1] = base[yp1] = av;
            *base++ = av + tot % 3;
        } while (base <= &minfo->int_7);
        break;

    case 13:                   /* inlined t_av_lrd() -- (ThMO) */
        xm1 = &minfo[-WORLD_SIDE_LEN].int_1 - base;
        xp1 = &minfo[WORLD_SIDE_LEN].int_1 - base;
        yp1 = &minfo[1].int_1 - base;
        do {
            tot = *base + base[xm1] + base[xp1] + base[yp1];
            av = tot / 4;
            base[xm1] = base[xp1] = base[yp1] = av;
            *base++ = av + tot % 4;
        } while (base <= &minfo->int_7);
        break;

    case 14:                   /* inlined t_av_urd() -- (ThMO) */
        ym1 = &minfo[-1].int_1 - base;
        xp1 = &minfo[WORLD_SIDE_LEN].int_1 - base;
        yp1 = &minfo[1].int_1 - base;
        do {
            tot = *base + base[ym1] + base[xp1] + base[yp1];
            av = tot / 4;
            base[ym1] = base[xp1] = base[yp1] = av;
            *base++ = av + tot % 4;
        } while (base <= &minfo->int_7);
        break;

    case 15:                   /* inlined t_av_lurd() -- (ThMO) */
        xm1 = &minfo[-WORLD_SIDE_LEN].int_1 - base;
        ym1 = &minfo[-1].int_1 - base;
        xp1 = &minfo[WORLD_SIDE_LEN].int_1 - base;
        yp1 = &minfo[1].int_1 - base;
        do {
            tot = *base + base[xm1] + base[ym1] + base[xp1] + base[yp1];
            av = tot / 5;
            base[xm1] = base[ym1] = base[xp1] = base[yp1] = av;
            *base++ = av + tot % 5;
        } while (base <= &minfo->int_7);
        break;
    }
    if (*--base >= max_waste) {
        *base -= WASTE_BURN_ON_TRANSPORT;
        ++*pol;
        if (*waste_count > TRANSPORT_BURN_WASTE_COUNT) {
            *waste_count = 0;
        } else {
            ++*waste_count;
        }
    }
}

void connect_transport(int originx, int originy, int w, int h)
{
    // sets the correct TYPE depending on neighbours, => gives the correct tile to display
    int x, y, mask, tflags;
    short group, type;

    static const short power_table[16] = {
        CST_POWERL_H_D, CST_POWERL_V_D, CST_POWERL_H_D, CST_POWERL_RD_D,
        CST_POWERL_H_D, CST_POWERL_LD_D, CST_POWERL_H_D, CST_POWERL_LDR_D,
        CST_POWERL_V_D, CST_POWERL_V_D, CST_POWERL_RU_D, CST_POWERL_UDR_D,
        CST_POWERL_LU_D, CST_POWERL_LDU_D, CST_POWERL_LUR_D, CST_POWERL_LUDR_D
    };
    static const short track_table[16] = {
        CST_TRACK_LR, CST_TRACK_LR, CST_TRACK_UD, CST_TRACK_LU,
        CST_TRACK_LR, CST_TRACK_LR, CST_TRACK_UR, CST_TRACK_LUR,
        CST_TRACK_UD, CST_TRACK_LD, CST_TRACK_UD, CST_TRACK_LUD,
        CST_TRACK_DR, CST_TRACK_LDR, CST_TRACK_UDR, CST_TRACK_LUDR
    };
    static const short road_table[16] = {
        CST_ROAD_LR, CST_ROAD_LR, CST_ROAD_UD, CST_ROAD_LU,
        CST_ROAD_LR, CST_ROAD_LR, CST_ROAD_UR, CST_ROAD_LUR,
        CST_ROAD_UD, CST_ROAD_LD, CST_ROAD_UD, CST_ROAD_LUD,
        CST_ROAD_DR, CST_ROAD_LDR, CST_ROAD_UDR, CST_ROAD_LUDR
    };
    static const short rail_table[16] = {
        CST_RAIL_LR, CST_RAIL_LR, CST_RAIL_UD, CST_RAIL_LU,
        CST_RAIL_LR, CST_RAIL_LR, CST_RAIL_UR, CST_RAIL_LUR,
        CST_RAIL_UD, CST_RAIL_LD, CST_RAIL_UD, CST_RAIL_LUD,
        CST_RAIL_DR, CST_RAIL_LDR, CST_RAIL_UDR, CST_RAIL_LUDR
    };
    static const short water_table[16] = {
        CST_WATER, CST_WATER_D, CST_WATER_R, CST_WATER_RD,
        CST_WATER_L, CST_WATER_LD, CST_WATER_LR, CST_WATER_LRD,
        CST_WATER_U, CST_WATER_UD, CST_WATER_UR, CST_WATER_URD,
        CST_WATER_LU, CST_WATER_LUD, CST_WATER_LUR, CST_WATER_LURD
    };

#if	FLAG_LEFT != 1 || FLAG_UP != 2 || FLAG_RIGHT != 4 || FLAG_DOWN != 8
#error  connect_transport(): you loose
#error  the algorithm depends on proper flag settings -- (ThMO)
#endif

    /* Adjust originx,originy,w,h to proper range */
    if (originx <= 0) {
        originx = 1;
        w -= 1 - originx;
    }
    if (originy <= 0) {
        originy = 1;
        h -= 1 - originy;
    }
    if (originx + w >= WORLD_SIDE_LEN) {
        w = WORLD_SIDE_LEN - originx;
    }
    if (originy + h >= WORLD_SIDE_LEN) {
        h = WORLD_SIDE_LEN - originy;
    }

    for (x = originx; x < originx + w; x++) {
        for (y = originy; y < originy + h; y++) {
            switch (MP_GROUP(x, y)) {
            case GROUP_POWER_LINE:
                /* First, set up a mask indicating into which directions 
                 * power may be transferred */
                mask = 0;

                /* up -- (ThMO) */
                group = MP_GROUP(x, y - 1);
                /* see if dug under track, rail or road */
                if (y > 1 && (group == GROUP_TRACK
                            || group == GROUP_RAIL || group == GROUP_ROAD || group == GROUP_WATER))
                    group = MP_GROUP(x, y - 2);
                switch (group) {
                    case GROUP_POWER_LINE:
                    case GROUP_SOLAR_POWER:
                    case GROUP_SUBSTATION:
                    case GROUP_COAL_POWER:
                        mask |= 8;
                        break;
                }

                /* left -- (ThMO) */
                group = MP_GROUP(x - 1, y);
                if (x > 1 && (group == GROUP_TRACK
                            || group == GROUP_RAIL || group == GROUP_ROAD || group == GROUP_WATER))
                    group = MP_GROUP(x - 2, y);
                switch (group) {
                    case GROUP_POWER_LINE:
                    case GROUP_SOLAR_POWER:
                    case GROUP_SUBSTATION:
                    case GROUP_COAL_POWER:
                        mask |= 4;
                        break;
                }

                /* right -- (ThMO) */
                group = MP_GROUP(x + 1, y);
                if (x < WORLD_SIDE_LEN - 2 && (group == GROUP_TRACK
                            || group == GROUP_RAIL
                            || group == GROUP_ROAD || group == GROUP_WATER))
                    group = MP_GROUP(x + 2, y);
                switch (group) {
                    case GROUP_WINDMILL:
                        if (MP_TECH(x + 1, y) < MODERN_WINDMILL_TECH)
                            break;
                    case GROUP_POWER_LINE:
                    case GROUP_SOLAR_POWER:
                    case GROUP_SUBSTATION:
                    case GROUP_COAL_POWER:
                        mask |= 2;
                        break;
                }

                /* down -- (ThMO) */
                group = MP_GROUP(x, y + 1);
                if (y < WORLD_SIDE_LEN - 2 && (group == GROUP_TRACK
                            || group == GROUP_RAIL
                            || group == GROUP_ROAD || group == GROUP_WATER))
                    group = MP_GROUP(x, y + 2);
                switch (group) {
                    case GROUP_WINDMILL:
                        if (MP_TECH(x, y + 1) < MODERN_WINDMILL_TECH)
                            break;
                    case GROUP_POWER_LINE:
                    case GROUP_SOLAR_POWER:
                    case GROUP_SUBSTATION:
                    case GROUP_COAL_POWER:
                        ++mask;
                        break;
                }

                /* Next, set the connectivity into MP_TYPE */
                MP_TYPE(x, y) = power_table[mask];
                /* Finally, adjust MP_TYPE to show electon bolt */
                /* WCK:This is done in do_power_line now if (MP_INFO(x, y).int_1 != 0) */
                break;

            case GROUP_TRACK:
                mask = 0;

                if (MP_GROUP(x, y - 1) == GROUP_TRACK)
                    mask |= FLAG_UP;
                if (MP_GROUP(x - 1, y) == GROUP_TRACK)
                    mask |= FLAG_LEFT;
                tflags = mask;

                switch (MP_GROUP(x + 1, y)) {
                    case GROUP_TRACK:
                        tflags |= FLAG_RIGHT;
                    case GROUP_COMMUNE:
                    case GROUP_COALMINE:
                    case GROUP_OREMINE:
                    case GROUP_INDUSTRY_L:
                    case GROUP_INDUSTRY_H:
                    case GROUP_RECYCLE:
                    case GROUP_TIP:
                    case GROUP_PORT:
                        mask |= FLAG_RIGHT;
                        break;
                    default:
                        if (MP_GROUP(x + 1, y - 1) == GROUP_COAL_POWER)
                            mask |= FLAG_RIGHT;
                        break;
                }

                switch (MP_GROUP(x, y + 1)) {
                    case GROUP_TRACK:
                        tflags |= FLAG_DOWN;
                    case GROUP_COMMUNE:
                    case GROUP_COALMINE:
                    case GROUP_OREMINE:
                    case GROUP_INDUSTRY_L:
                    case GROUP_INDUSTRY_H:
                    case GROUP_RECYCLE:
                    case GROUP_TIP:
                    case GROUP_PORT:
                        mask |= FLAG_DOWN;
                        break;
                    default:
                        if (MP_GROUP(x - 1, y + 1) == GROUP_COAL_POWER)
                            mask |= FLAG_DOWN;
                        break;
                }
                MP_INFO(x, y).flags &= ~(FLAG_UP | FLAG_DOWN | FLAG_LEFT | FLAG_RIGHT);
                MP_INFO(x, y).flags |= tflags;
                MP_TYPE(x, y) = track_table[mask];
                break;

            case GROUP_ROAD:
                mask = 0;

                if (MP_GROUP(x, y - 1) == GROUP_ROAD)
                    mask |= FLAG_UP;
                if (MP_GROUP(x - 1, y) == GROUP_ROAD)
                    mask |= FLAG_LEFT;
                tflags = mask;

                switch (MP_GROUP(x + 1, y)) {
                    case GROUP_ROAD:
                        tflags |= FLAG_RIGHT;
                    case GROUP_COMMUNE:
                    case GROUP_COALMINE:
                    case GROUP_OREMINE:
                    case GROUP_INDUSTRY_L:
                    case GROUP_INDUSTRY_H:
                    case GROUP_RECYCLE:
                    case GROUP_TIP:
                    case GROUP_PORT:
                        mask |= FLAG_RIGHT;
                        break;
                    default:
                        if (MP_GROUP(x + 1, y - 1) == GROUP_COAL_POWER)
                            mask |= FLAG_RIGHT;
                        break;
                }
                switch (MP_GROUP(x, y + 1)) {
                    case GROUP_ROAD:
                        tflags |= FLAG_DOWN;
                    case GROUP_COMMUNE:
                    case GROUP_COALMINE:
                    case GROUP_OREMINE:
                    case GROUP_INDUSTRY_L:
                    case GROUP_INDUSTRY_H:
                    case GROUP_RECYCLE:
                    case GROUP_TIP:
                    case GROUP_PORT:
                        mask |= FLAG_DOWN;
                        break;
                    default:
                        if (MP_GROUP(x - 1, y + 1) == GROUP_COAL_POWER)
                            mask |= FLAG_DOWN;
                        break;
                }
                MP_INFO(x, y).flags &= ~(FLAG_UP | FLAG_DOWN | FLAG_LEFT | FLAG_RIGHT);
                MP_INFO(x, y).flags |= tflags;
                MP_TYPE(x, y) = road_table[mask];
                break;

            case GROUP_RAIL:
                mask = 0;

                if (MP_GROUP(x, y - 1) == GROUP_RAIL)
                    mask |= FLAG_UP;
                if (MP_GROUP(x - 1, y) == GROUP_RAIL)
                    mask |= FLAG_LEFT;
                tflags = mask;

                switch (MP_GROUP(x + 1, y)) {
                    case GROUP_RAIL:
                        tflags |= FLAG_RIGHT;
                    case GROUP_COMMUNE:
                    case GROUP_COALMINE:
                    case GROUP_OREMINE:
                    case GROUP_INDUSTRY_L:
                    case GROUP_INDUSTRY_H:
                    case GROUP_RECYCLE:
                    case GROUP_TIP:
                    case GROUP_PORT:
                        mask |= FLAG_RIGHT;
                        break;
                    default:
                        if (MP_GROUP(x + 1, y - 1) == GROUP_COAL_POWER)
                            mask |= FLAG_RIGHT;
                        break;
                }
                switch (MP_GROUP(x, y + 1)) {
                    case GROUP_RAIL:
                        tflags |= FLAG_DOWN;
                    case GROUP_COMMUNE:
                    case GROUP_COALMINE:
                    case GROUP_OREMINE:
                    case GROUP_INDUSTRY_L:
                    case GROUP_INDUSTRY_H:
                    case GROUP_RECYCLE:
                    case GROUP_TIP:
                    case GROUP_PORT:
                        mask |= FLAG_DOWN;
                        break;
                    default:
                        if (MP_GROUP(x - 1, y + 1)
                                == GROUP_COAL_POWER)
                            mask |= FLAG_DOWN;
                        break;
                }
                MP_INFO(x, y).flags &= ~(FLAG_UP | FLAG_DOWN | FLAG_LEFT | FLAG_RIGHT);
                MP_INFO(x, y).flags |= tflags;
                MP_TYPE(x, y) = rail_table[mask];
                break;

            case GROUP_WATER:
                mask = 0;
                /* up -- (ThMO) */
                if (MP_GROUP(x, y - 1) == GROUP_WATER)
                    mask |= 8;

                /* left -- (ThMO) */
                type = MP_TYPE(x - 1, y);
                if ((type == CST_USED && MP_GROUP(MP_INFO(x - 1, y).int_1, MP_INFO(x - 1, y).int_2)
                            == GROUP_PORT)
                        || get_group_of_type(type) == GROUP_WATER)
                    mask |= 4;

                /* right -- (ThMO) */
                if (MP_GROUP(x + 1, y) == GROUP_WATER)
                    mask |= 2;

                /* down -- (ThMO) */
                if (MP_GROUP(x, y + 1) == GROUP_WATER)
                    ++mask;

                MP_TYPE(x, y) = water_table[mask];
                break;
            }                   /* end switch */
        }                       /* end for */
    }                           /* end for */
}
