/* ---------------------------------------------------------------------- *
 * rocket_pad.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * (c) Corey Keasling, 2004
 * ---------------------------------------------------------------------- */

#include "modules.h"
#include "gui_interface/screen_interface.h"
#include "gui_interface/pbar_interface.h"
#include "rocket_pad.h"
#include "residence.h" //for removing people
#include "lincity-ng/Sound.hpp"
#include "lincity-ng/Dialog.hpp"

RocketPadConstructionGroup rocketPadConstructionGroup(
    "Rocket Pad",
     TRUE,                     /* need credit? */
     GROUP_ROCKET,
     GROUP_ROCKET_SIZE,
     GROUP_ROCKET_COLOUR,
     GROUP_ROCKET_COST_MUL,
     GROUP_ROCKET_BUL_COST,
     GROUP_ROCKET_FIREC,
     GROUP_ROCKET_COST,
     GROUP_ROCKET_TECH,
     GROUP_ROCKET_RANGE
);

Construction *RocketPadConstructionGroup::createConstruction(int x, int y) {
    return new RocketPad(x, y, this);
}

extern void ok_dial_box(const char *, int, const char *);

void RocketPad::update()
{
    // ok the party is over
    if (type == 7)
    {   return;}
    rocket_pad_cost += ROCKET_PAD_RUNNING_COST;
    // store as much as possible or needed
    while(
               (type < 4)
            && (commodityCount[STUFF_JOBS] >= ROCKET_PAD_JOBS)
            && (commodityCount[STUFF_GOODS] >= ROCKET_PAD_GOODS)
            && (commodityCount[STUFF_STEEL] >= ROCKET_PAD_STEEL)
            && (jobs_stored < ROCKET_PAD_JOBS_STORE)
            && (goods_stored < ROCKET_PAD_GOODS_STORE)
            && (steel_stored < ROCKET_PAD_STEEL_STORE)
            && (completion < 100)
         )
    {
        commodityCount[STUFF_JOBS] -= ROCKET_PAD_JOBS;
        jobs_stored += ROCKET_PAD_JOBS;
        commodityCount[STUFF_GOODS] -= ROCKET_PAD_GOODS;
        goods_stored += ROCKET_PAD_GOODS;
        commodityCount[STUFF_STEEL]-= ROCKET_PAD_STEEL;
        steel_stored += ROCKET_PAD_STEEL;
        commodityCount[STUFF_WASTE] += ROCKET_PAD_GOODS/3;
        step += 2;
        working_days++;
    }

    // see if we can build another % of Rocket
    if(    (completion < 100)
        && (jobs_stored >= ROCKET_PAD_JOBS_STORE)
        && (goods_stored >= ROCKET_PAD_GOODS_STORE)
        && (steel_stored >= ROCKET_PAD_STEEL_STORE)
      )
    {
        jobs_stored -= ROCKET_PAD_JOBS_STORE;
        goods_stored -= ROCKET_PAD_GOODS_STORE;
        steel_stored -= ROCKET_PAD_STEEL_STORE;
        completion++;
        step = 0;
    }
    //monthly update
    if (total_time % 100 == 0)
    {
        busy = working_days;
        working_days = 0;
    }

    /* animate and return if already said no to launch */
    if (type >= 4 && completion >= (100 * ROCKET_PAD_LAUNCH) / 100)
    {
        if (real_time >= anim)
        {
            anim = real_time + ROCKET_ANIMATION_SPEED;
            if(++type > 6)
            {   type = 4;}
        }
        return;
    }

    //Choose a Graphic and invoke Lauch Dialogue depening on completion
    if (completion < (25 * ROCKET_PAD_LAUNCH) / 100)
    {   type = 0;}
    else if (completion < (60 * ROCKET_PAD_LAUNCH) / 100)
    {   type = 1;}
    else if (completion < (90 * ROCKET_PAD_LAUNCH) / 100)
    {   type = 2;}
    else if (completion < (100 * ROCKET_PAD_LAUNCH) / 100)
    {   type = 3;}
    else if (completion >= (100 * ROCKET_PAD_LAUNCH) / 100)
    {
        type = 4;
        //OK Button will launch rocket remotely
        if(!(flags & FLAG_ROCKET_READY))
        {   new Dialog( ASK_LAUNCH_ROCKET, x, y );}
        flags |= FLAG_ROCKET_READY;
    }
}

void RocketPad::launch_rocket()
{
    int i, r, xx, yy, xxx, yyy;
    rockets_launched++;
    type = 7;
    busy = 0;
    /* The first five failures gives 49.419 % chances of 5 success
     * TODO: some stress could be added by 3,2,1,0 and animation of rocket with sound...
     */
    r = rand() % MAX_TECH_LEVEL;
    if (r > tech_level || rand() % 100 > (rockets_launched * 15 + 25))
    {
        /* the launch failed */
        //display_rocket_result_dialog(ROCKET_LAUNCH_BAD);
        getSound()->playSound( "RocketExplosion" );
        ok_dial_box ("launch-fail.mes", BAD, 0L);
        rockets_launched_success = 0;
        xx = ((rand() % 40) - 20) + x;
        yy = ((rand() % 40) - 20) + y;
        for (i = 0; i < 20; i++)
        {
            xxx = ((rand() % 20) - 10) + xx;
            yyy = ((rand() % 20) - 10) + yy;
            if (xxx > 0 && xxx < (world.len() - 1)
                && yyy > 0 && yyy < (world.len() - 1))
            {
                /* don't crash on it's own area */
                if (xxx >= x && xxx < (x + constructionGroup->size) && yyy >= y && yyy < (y + constructionGroup->size))
                {   continue;}
                fire_area(xxx, yyy);
                /* make a sound perhaps */
            }
        }
    }
    else
    {
        getSound()->playSound( "RocketTakeoff" );
        rockets_launched_success++;
        /* TODO: Maybe should generate some pollution ? */
        if (rockets_launched_success > 5)
        {
            remove_people(1000);
            if (people_pool || housed_population)
            {
                //display_rocket_result_dialog(ROCKET_LAUNCH_EVAC);
                ok_dial_box ("launch-evac.mes", GOOD, 0L);
            }
        }
        else
        {
            //display_rocket_result_dialog(ROCKET_LAUNCH_GOOD);
            ok_dial_box ("launch-good.mes", GOOD, 0L);
        }
    }
}

void RocketPad::remove_people(int num)
{
    {
        int ppl = (num < people_pool)?num:people_pool;
        num -= ppl;
        people_pool -= ppl;
        total_evacuated += ppl;
    }
    /* reset housed population so that we can display it correctly */
    housed_population = 1;
    while (housed_population && (num > 0))
    {
        housed_population = 0;
        for (int i = 0; i < constructionCount.size(); i++)
        {
            if (constructionCount[i])
            {
                unsigned short grp = constructionCount[i]->constructionGroup->group;
                if( (grp == GROUP_RESIDENCE_LL)
                 || (grp == GROUP_RESIDENCE_ML)
                 || (grp == GROUP_RESIDENCE_HL)
                 || (grp == GROUP_RESIDENCE_LH)
                 || (grp == GROUP_RESIDENCE_MH)
                 || (grp == GROUP_RESIDENCE_HH) )
                 {
                    Residence* residence = static_cast <Residence *> (constructionCount[i]);
                    if (residence->local_population)
                    {
                        residence->local_population--;
                        housed_population += residence->local_population;
                        num--;
                        total_evacuated++;
                    }
                 }
            }
        }
    }
    update_pbar (PPOP, housed_population + people_pool, 0);
    if (!housed_population && !people_pool)
    {   ok_dial_box("launch-gone.mes", GOOD, 0L);}
}

void RocketPad::report()
{
    int i = 0;
    mps_store_sd(i++,constructionGroup->name,ID);
    mps_store_sfp(i++, _("busy"), (busy));
    mps_store_sfp(i++, _("Tech"), (tech * 100.0) / MAX_TECH_LEVEL);
    mps_store_sfp(i++, "Overall Progress", completion);
    mps_store_sfp(i++, "Next Step", step);
    i++;
    list_commodities(&i);
}

/** @file lincity/modules/rocket_pad.cpp */

