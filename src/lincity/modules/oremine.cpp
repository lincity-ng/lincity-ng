/* ---------------------------------------------------------------------- *
 * oremine.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "modules.h"
#include "gui_interface/screen_interface.h"
#include "oremine.h"
#include <cstdlib>
#include "../transport.h"

void do_oremine(int x, int y)
{
    // int_1 is the ore at in stock
    // int_2 is the ore reserve under the ground or at the surface really.
    //
    // Animation stuff
    // int_3 is current displayed tile
    // int_4 is a time shift for animation to prevent identical animation
    // int_6 is a tile increment/decrement   ..         ..        ..
    int xx, yy, xs, ys, xe, ye, cr;
    if (MP_INFO(x, y).int_1 < MAX_ORE_AT_MINE - 5000) {
        xs = x;
        ys = y;
        xe = x + 4;
        ye = y + 4;
        cr = 0;
        for (yy = ys; yy < ye; yy++)
            for (xx = xs; xx < xe; xx++)
                cr += MP_INFO(xx, yy).ore_reserve;
        MP_INFO(x, y).int_2 = cr;
        if (cr > 0)
            if (get_jobs(x, y, JOBS_DIG_ORE) != 0)
                for (yy = ys; yy < ye; yy++)
                    for (xx = xs; xx < xe; xx++)
                        if (MP_INFO(xx, yy).ore_reserve > 0) {
                            MP_INFO(xx, yy).ore_reserve--;
                            MP_INFO(x, y).int_1 += 5000;
                            ore_made += 5000;
                            sust_dig_ore_coal_tip_flag = 0;
                            /* maybe want an ore tax? */
                            yy = ye;
                            xx = xe;    /* break out */
                        }
    }

    for (cr=0, xx = x-1, yy = y; yy > y-2; xx++, yy--) {
        // (xx,yy) = (x-1, y) then (x, y-1) ; neighbouring tranport coordinates
        if ((MP_INFO(xx, yy).flags & FLAG_IS_TRANSPORT) && get_jobs(x, y, JOBS_LOAD_ORE)) {
            if (MP_GROUP(xx, yy) == GROUP_RAIL
                    && MP_INFO(xx, yy).int_5 < MAX_ORE_ON_RAIL
                    && MP_INFO(x, y).int_1 >= (MAX_ORE_ON_RAIL - MP_INFO(xx, yy).int_5)) {
                MP_INFO(x, y).int_1 -= (MAX_ORE_ON_RAIL - MP_INFO(xx, yy).int_5);
                MP_INFO(xx, yy).int_5 = MAX_ORE_ON_RAIL;
                cr += 3;
            } else if (MP_GROUP(xx, yy) == GROUP_ROAD
                    && MP_INFO(xx, yy).int_5 < MAX_ORE_ON_ROAD
                    && MP_INFO(x, y).int_1 >= (MAX_ORE_ON_ROAD - MP_INFO(xx, yy).int_5)) {
                MP_INFO(x, y).int_1 -= (MAX_ORE_ON_ROAD - MP_INFO(xx, yy).int_5);
                MP_INFO(xx, yy).int_5 = MAX_ORE_ON_ROAD;
                cr += 2;
            } else if (MP_GROUP(xx, yy) == GROUP_TRACK
                    && MP_INFO(xx, yy).int_5 < MAX_ORE_ON_TRACK
                    && MP_INFO(x, y).int_1 >= (MAX_ORE_ON_TRACK - MP_INFO(xx, yy).int_5)) {
                MP_INFO(x, y).int_1 -= (MAX_ORE_ON_TRACK - MP_INFO(xx, yy).int_5);
                MP_INFO(xx, yy).int_5 = MAX_ORE_ON_TRACK;
                cr += 1;
            }
        }
    }

    // Anim according to ore mine activity
    if (cr && real_time > MP_ANIM(x, y)) {
        // MP_ANIM is reseted to 0 when we start/load a game
        if (MP_ANIM(x,y) == 0) {
            MP_INFO(x,y).int_3 = 0;
            MP_INFO(x,y).int_4 = 0;
            MP_INFO(x,y).int_6 = 0;
        } else {
            // Compute random inc/dec
            if (real_time > MP_INFO(x, y).int_4) {
                MP_INFO(x, y).int_4 = real_time + (16 * OREMINE_ANIMATION_SPEED) + (rand() % (16 * OREMINE_ANIMATION_SPEED));
                (MP_INFO(x, y).int_6 > 0) ? MP_INFO(x, y).int_6 = -1 : MP_INFO(x, y).int_6 = 1;
            }
        }
        // old behavior was to show reserve
        // xx = 7 * (MP_INFO(x, y).int_2 + (3 * ORE_RESERVE / 2)) / (16 * ORE_RESERVE);
        //
        // new behavior is to have faster animation for more active mines
        MP_ANIM(x, y) = real_time + ((8 - cr) * OREMINE_ANIMATION_SPEED);

        xx = (MP_INFO(x, y).int_3 + MP_INFO(x, y).int_6) & 15;
        MP_INFO(x, y).int_3 = xx;
        switch (xx) {
            case (0):
                MP_TYPE(x, y) = CST_OREMINE_1;
                break;
            case (1):
                MP_TYPE(x, y) = CST_OREMINE_2;
                break;
            case (2):
                MP_TYPE(x, y) = CST_OREMINE_3;
                break;
            case (3):
                MP_TYPE(x, y) = CST_OREMINE_4;
                break;
            case (4):
                MP_TYPE(x, y) = CST_OREMINE_5;
                break;
            case (5):
                MP_TYPE(x, y) = CST_OREMINE_6;
                break;
            case (6):
                MP_TYPE(x, y) = CST_OREMINE_7;
                break;
            case (7):
                MP_TYPE(x, y) = CST_OREMINE_8;
                break;
            case (8):
                MP_TYPE(x, y) = CST_OREMINE_7;
                break;
            case (9):
                MP_TYPE(x, y) = CST_OREMINE_6;
                break;
            case (10):
                MP_TYPE(x, y) = CST_OREMINE_5;
                break;
            case (11):
                MP_TYPE(x, y) = CST_OREMINE_4;
                break;
            case (12):
                MP_TYPE(x, y) = CST_OREMINE_5;
                break;
            case (13):
                MP_TYPE(x, y) = CST_OREMINE_4;
                break;
            case (14):
                MP_TYPE(x, y) = CST_OREMINE_3;
                break;
            case (15):
                MP_TYPE(x, y) = CST_OREMINE_2;
                break;
        }
    }
    if (MP_INFO(x, y).int_2 <= 0) {
        int i, j;
        for (j = 0; j < 4; j++) {
            for (i = 0; i < 4; i++) {
                do_bulldoze_area(CST_WATER, x + i, y + j);
            }
        }
        connect_rivers();
        refresh_main_screen();
    }
}

void mps_oremine(int x, int y)
{
    int i = 0;

    mps_store_title(i++, _("Ore Mine"));
    i++;

    mps_store_sddp(i++, _("Stock"), MP_INFO(x, y).int_1, MAX_ORE_AT_MINE);
    i++;

    mps_store_sddp(i++, _("Reserve"), MP_INFO(x, y).int_2 * 5000, (ORE_RESERVE * 16 * 5000));
}

/** @file lincity/modules/oremine.cpp */

