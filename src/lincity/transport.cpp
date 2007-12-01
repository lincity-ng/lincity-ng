/* ---------------------------------------------------------------------- *
 * transport.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "lin-city.h"
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
