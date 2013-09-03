/* ---------------------------------------------------------------------- *
 * market.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#include <cstdlib>
#include "market.h"
#include "fire.h" //for playing with fire
#include "lincity-ng/ErrorInterface.hpp"

MarketConstructionGroup marketConstructionGroup(
    "Market",
     FALSE,                     /* need credit? */
     GROUP_MARKET,
     GROUP_MARKET_SIZE,
     GROUP_MARKET_COLOUR,
     GROUP_MARKET_COST_MUL,
     GROUP_MARKET_BUL_COST,
     GROUP_MARKET_FIREC,
     GROUP_MARKET_COST,
     GROUP_MARKET_TECH,
     GROUP_MARKET_RANGE
);

Construction *MarketConstructionGroup::createConstruction(int x, int y, unsigned short type) {
    return new Market(x, y, type);
}


void Market::update()
{
    int tmp, xx, yy, pears, n;
    int ratio;
    int market_lvl, market_cap;
    int market_ratio = 0;
    Commodities stuff_ID;
    std::map<Commodities, int>::iterator stuff_it;
    n = 0;
    for(stuff_it = commodityCount.begin() ; stuff_it != commodityCount.end() ; stuff_it++ )
    {
        stuff_ID = stuff_it->first;
        //dont handle stuff if neither give nor take
        if (!commodityRuleCount[stuff_ID].give
         && !commodityRuleCount[stuff_ID].take)
        {
            continue;
        }
        //dont handle anything else than jobs if to little jobs
        // deadlock

        if ((stuff_ID != STUFF_JOBS)
         && (commodityCount[STUFF_JOBS] < jobs)
         && !(flags & FLAG_EVACUATE)
           )
        {continue;}

        market_lvl = stuff_it->second;
        market_cap = (flags & FLAG_EVACUATE)?(market_lvl>0?market_lvl:1):constructionGroup->commodityRuleCount[stuff_ID].maxload;
        pears = 1;
        ratio = market_lvl * TRANSPORT_QUANTA / market_cap;
        market_ratio += ratio;
        n++;
        for(yy = ys; yy < ye; yy++)
        {
            for(xx = xs; xx < xe; xx++)
            {
                //Count Constructions only once
                //Never count other markets far away transport or power lines
                if ( !world(xx,yy)->construction
                  || (world(xx,yy)->getGroup() == GROUP_MARKET)
                  || (world(xx,yy)->getGroup() == GROUP_POWER_LINE)
                  || (world(xx,yy)->is_transport() &&
                    !( ((xx==(x-1)) || (xx==(x+constructionGroup->size)) ) &&
                    ((yy==(y-1)) || (yy==(y+constructionGroup->size)) ) )
                    )
                   )
                {
                    continue;
                }
                tmp = collect_transport_info( xx, yy, stuff_ID, market_ratio);
                if (tmp != -1)
                {
                    ratio += tmp;
                    pears++;
                }
            }//endfor xx
        }//endfor yy
        // not weighted average filling for stuff_ID
        ratio /= pears;
        for(yy = ys; yy < ye; yy++)
        {
            for(xx = xs; xx < xe; xx++)
            {
                //Deal with constructions only once
                //Never deal with markets power lines or far away transport
                if ( !world(xx,yy)->construction
                  || (world(xx,yy)->getGroup() == GROUP_MARKET)
                  || (world(xx,yy)->getGroup() == GROUP_POWER_LINE)
                  || (world(xx,yy)->is_transport() &&
                    !( ((xx==(x-1)) || (xx==(x+constructionGroup->size)) ) &&
                    ((yy==(y-1)) || (yy==(y+constructionGroup->size)) ) )
                    )
                   )
                {
                    continue;
                }
                int old_lvl = market_lvl;
                //do the normal flow
                //if (collect_transport_info( xx, yy, stuff_ID, market_ratio)!=-1)
                //{
                equilibrate_transport_stuff(xx, yy, &market_lvl, market_cap, ratio,stuff_ID);
                //}
                int flow = market_lvl - old_lvl;
                //revert flow if it conflicts with local rules
                if((!commodityRuleCount[stuff_ID].give && flow < 0)
                 ||(!commodityRuleCount[stuff_ID].take && flow > 0))
                {
                    market_lvl -= flow;
                    world(xx,yy)->construction->commodityCount[stuff_ID] += flow;
                }
            }//endfor xx
        }//endfor yy
        stuff_it->second = market_lvl;
    }//endfor stuff_it

    if (commodityCount[STUFF_JOBS] >= jobs)
    {
        commodityCount[STUFF_JOBS] -= jobs;
        //Have to collect taxes here since transport does not consider the market a consumer but rather as another transport
        income_tax += jobs;
    }

    if (total_time % 25 == 17)
    {
        //average filling of the market, catch n == 0 in case market has
        //not yet any commodities initialized
        if (n > 0)
        {   market_ratio = 100*market_ratio/(n * TRANSPORT_QUANTA);}
        else
        {   market_ratio = 0;}

        if (market_ratio < 10)
        {
            type = CST_MARKET_LOW;
            jobs = JOBS_MARKET_LOW;
        }
        else if (market_ratio < 50)
        {
            type = CST_MARKET_MED;
            jobs = JOBS_MARKET_MED;
        }
        else
        {
            type = CST_MARKET_FULL;
            jobs = JOBS_MARKET_FULL;
        }
    }
    if (commodityCount[STUFF_WASTE] >= (85 * MAX_WASTE_IN_MARKET / 100) && !world(x+1,y+1)->construction)
    {
        anim = real_time + 6 * WASTE_BURN_TIME;
        world(x+1,y+1)->pollution += MAX_WASTE_IN_MARKET/20;
        commodityCount[STUFF_WASTE] -= (7 * MAX_WASTE_IN_MARKET) / 10;
        if(!world(x+1,y+1)->construction)
        {
            Construction *fire = fireConstructionGroup.createConstruction(x+1, y+1, CST_FIRE_1);
            world(x+1,y+1)->construction = fire;
            world(x+1,y+1)->reportingConstruction = fire;
            //waste burning never spreads
            (dynamic_cast<Fire*>(fire))->burning_days = FIRE_LENGTH - FIRE_DAYS_PER_SPREAD + 1;
            (dynamic_cast<Fire*>(fire))->flags |= FLAG_IS_GHOST;
            ::constructionCount.add_construction(fire);

        }
    }
    else if ( real_time > anim && world(x+1,y+1)->construction)
    {
        ::constructionCount.remove_construction(world(x+1,y+1)->construction);
        delete world(x+1,y+1)->construction;
        world(x+1,y+1)->construction = NULL;
        world(x+1,y+1)->reportingConstruction = this;
    }
    else if (world(x+1,y+1)->construction)
    {   static_cast<Fire*> (world(x+1,y+1)->construction)->burning_days = FIRE_LENGTH - FIRE_DAYS_PER_SPREAD + 1;}
    if(refresh_cover)
    {   cover();}
}

void Market::cover() //do this for showing range in minimap
{
    for(int yy = ys; yy < ye; yy++)
    {
        for(int xx = xs; xx < xe; xx++)
        {
            world(xx,yy)->flags |= FLAG_MARKET_COVER;
        }
    }
}

void Market::report()
{
    int i = 0;

    mps_store_sd(i++,constructionGroup->name,ID);
    i++;
    //list_commodities(&i);
    std::map<Construction::Commodities, int>::iterator stuff_it;
    for(stuff_it = commodityCount.begin() ; stuff_it != commodityCount.end() ; stuff_it++)
    {
        char arrows[4]="---";
        if (flags & FLAG_EVACUATE)
        {
            arrows[0] = '<';
            arrows[1] = '<';
            arrows[2] = ' ';
        }
        else
        {
            if (commodityRuleCount[stuff_it->first].take)
            {   arrows[2] = '>';}
            if (commodityRuleCount[stuff_it->first].give)
            {   arrows[0] = '<';}
        }

        if(i < 14)
        {
            mps_store_ssddp(i++,arrows,commodityNames[stuff_it->first],stuff_it->second, commodityRuleCount[stuff_it->first].maxload);
        }//endif
    } //endfor
}

void Market::toggleEvacuation()
{
    bool evacuate = flags & FLAG_EVACUATE; //actually the previous state
    std::map<Construction::Commodities, CommodityRule>::iterator rule_it;
    for(rule_it = commodityRuleCount.begin() ; rule_it != commodityRuleCount.end() ; rule_it++)
    {
        if(!evacuate)
        {
            rule_it->second.give = true;
            rule_it->second.take = false;
        }
        else
        {
            rule_it->second.give = true;
            rule_it->second.take = true;
        }

    }
    flags &= ~FLAG_EVACUATE;
    if(!evacuate)
    {   flags |= FLAG_EVACUATE;}

}



/** @file lincity/modules/market.cpp */




