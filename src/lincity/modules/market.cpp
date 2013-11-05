/* ---------------------------------------------------------------------- *
 * market.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */
#include <cstdlib>
#include "market.h"
#include "fire.h" //for playing with fire
#include "lincity-ng/Sound.hpp"

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

//helper groups for graphics and sound sets, dont add them to ConstructionGroup::groupMap
MarketConstructionGroup market_low_ConstructionGroup  = marketConstructionGroup;
MarketConstructionGroup market_med_ConstructionGroup  = marketConstructionGroup;
MarketConstructionGroup market_full_ConstructionGroup = marketConstructionGroup;


Construction *MarketConstructionGroup::createConstruction(int x, int y) {
    return new Market(x, y, this);
}


void Market::update()
{
    int ratio, trade_ratio, n;
    int lvl, market_lvl;
    int cap, market_cap;
    int market_ratio = 0;
    const size_t partsize = partners.size();
    bool lvls[partsize];
    Commodities stuff_ID;
    std::map<Commodities, int>::iterator stuff_it;
    n = 0;
    for(stuff_it = commodityCount.begin() ; stuff_it != commodityCount.end() ; stuff_it++ )
    {
        stuff_ID = stuff_it->first;
        //dont handle stuff if neither give nor take
        //dont handle anything else if there are to little jobs
        if ((!commodityRuleCount[stuff_ID].give
         && !commodityRuleCount[stuff_ID].take) ||
         (commodityCount[STUFF_JOBS] < jobs && stuff_ID !=STUFF_JOBS))
        {   continue;}

        market_lvl = stuff_it->second;
        market_cap = constructionGroup->commodityRuleCount[stuff_ID].maxload;
        ratio = market_lvl * TRANSPORT_QUANTA / market_cap;
        market_ratio += ratio;
        n++;
        lvl = market_lvl;
        cap = market_cap;
        for(unsigned int i = 0; i < partsize; ++i)
        {
            lvls[i] = false;
            Construction *pear = partners[i];
            if(pear->commodityCount.count(stuff_ID))
            {
                int lvlsi = pear->commodityCount[stuff_ID];
                int capsi = pear->constructionGroup->commodityRuleCount[stuff_ID].maxload;
                if(pear->flags & FLAG_EVACUATE)
                {   lvlsi = lvlsi?capsi:-1;}
                else
                {
                    int pearat = lvlsi * TRANSPORT_QUANTA / capsi;
                    //only consider stuff that would tentatively move
                    //Here the local rules of this market apply
                    if(((pearat > ratio)&&!(commodityRuleCount[stuff_ID].take &&
                            pear->constructionGroup->commodityRuleCount[stuff_ID].give)) ||
                       ((pearat < ratio)&&!(commodityRuleCount[stuff_ID].give &&
                            pear->constructionGroup->commodityRuleCount[stuff_ID].take)))
                    {   continue;}
                }
                lvls[i] = true;
                lvl += lvlsi;
                cap += capsi;
            }
        }
        trade_ratio = lvl * TRANSPORT_QUANTA / cap;
        for(unsigned int i = 0; i < partsize; ++i)
        {
            if(lvls[i])
            {   partners[i]->equilibrate_stuff(&market_lvl, market_cap, trade_ratio, stuff_ID, constructionGroup);}
        }
        stuff_it->second = market_lvl;
    }

    if (commodityCount[STUFF_JOBS] >= jobs)
    {
        commodityCount[STUFF_JOBS] -= jobs;
        //Have to collect taxes here since transport does not consider the market a consumer but rather as another transport
        income_tax += jobs;
        ++working_days;
    }
    //monthly update
    if (total_time % 100 == 0)
    {
        busy = working_days;
        working_days = 0;
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
            jobs = JOBS_MARKET_EMPTY;
            constructionGroup = &marketConstructionGroup;
        }
        else if (market_ratio < 20)
        {
            jobs = JOBS_MARKET_LOW;
            constructionGroup = &market_low_ConstructionGroup;
        }
        else if (market_ratio < 50)
        {
            jobs = JOBS_MARKET_MED;
            constructionGroup = &market_med_ConstructionGroup;
        }
        else
        {
            jobs = JOBS_MARKET_FULL;
             constructionGroup = &market_full_ConstructionGroup;
        }
    }
    if (commodityCount[STUFF_WASTE] >= (85 * MAX_WASTE_IN_MARKET / 100) && !world(x+1,y+1)->construction)
    {
        anim = real_time + 6 * WASTE_BURN_TIME;
        world(x+1,y+1)->pollution += MAX_WASTE_IN_MARKET/20;
        commodityCount[STUFF_WASTE] -= (7 * MAX_WASTE_IN_MARKET) / 10;
        if(!world(x+1,y+1)->construction)
        {
            Construction *fire = fireConstructionGroup.createConstruction(x+1, y+1);
            world(x+1,y+1)->construction = fire;
            world(x+1,y+1)->reportingConstruction = fire;
            //waste burning never spreads
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

    if(refresh_cover)
    {   cover();}
}

void Market::cover()
{
    for(int yy = ys; yy < ye; yy++)
    {
        for(int xx = xs; xx < xe; xx++)
        {   world(xx,yy)->flags |= FLAG_MARKET_COVER;}
    }
}

void Market::report()
{
    int i = 0;

    mps_store_sd(i++,constructionGroup->name,ID);
    i++;
    mps_store_sfp(i++, _("busy"), (float) busy);
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




