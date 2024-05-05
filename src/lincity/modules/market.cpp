/* ---------------------------------------------------------------------- *
 * market.c
 * This file is part of lincity.
 * Lincity is copyright (c) I J Peters 1995-1997, (c) Greg Sharp 1997-2001.
 * ---------------------------------------------------------------------- */

#include "market.h"

#include <cstdlib>                  // for size_t
#include <vector>                   // for vector

#include "modules.h"
#include "fire.h"                   // for FIRE_ANIMATION_SPEED

MarketConstructionGroup marketConstructionGroup(
     N_("Market"),
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
//MarketConstructionGroup market_low_ConstructionGroup  = marketConstructionGroup;
//MarketConstructionGroup market_med_ConstructionGroup  = marketConstructionGroup;
//MarketConstructionGroup market_full_ConstructionGroup = marketConstructionGroup;


Construction *MarketConstructionGroup::createConstruction(int x, int y) {
    return new Market(x, y, this);
}


void Market::update()
{
    int ratio, trade_ratio, n;
    int lvl, market_lvl;
    int cap, market_cap;
    market_ratio = 0;
    const size_t partsize = partners.size();
    std::vector<bool> lvls(partsize);
    Commodity stuff_ID;
    n = 0;
    for(stuff_ID = STUFF_INIT ; stuff_ID < STUFF_COUNT ; stuff_ID++ )
    {
        //dont handle stuff if neither give nor take
        //dont handle anything else if there are to little jobs
        const CommodityRule& market_rule = commodityRuleCount[stuff_ID];
        if ((!market_rule.give
         && !market_rule.take) ||
         (commodityCount[STUFF_JOBS] < jobs && stuff_ID != STUFF_JOBS))
        {   continue;}

        market_lvl = commodityCount[stuff_ID];
        market_cap = market_rule.maxload;
        ratio = market_lvl * TRANSPORT_QUANTA / market_cap;
        market_ratio += ratio;
        n++;
        lvl = market_lvl;
        cap = market_cap;
        for(unsigned int i = 0; i < lvls.size(); ++i)
        {
            lvls[i] = false;
            Construction *pear = partners[i];
            const CommodityRule& pearrule = pear->constructionGroup->commodityRuleCount[stuff_ID];
            if(pearrule.maxload)
            {
                int lvlsi = pear->commodityCount[stuff_ID];
                int capsi = pearrule.maxload;
                if(pear->flags & FLAG_EVACUATE)
                {   capsi = 0;}
                else
                {
                    int pearat = lvlsi * TRANSPORT_QUANTA / capsi;
                    //only consider stuff that would tentatively move
                    //Here the local rules of this market apply
                    if(((pearat > ratio)&&!(market_rule.take &&
                            pearrule.give)) ||
                       ((pearat < ratio)&&!(market_rule.give &&
                            pearrule.take)))
                    {   continue;}
                }
                lvls[i] = true;
                lvl += lvlsi;
                cap += capsi;
            }
        }
        trade_ratio = lvl * TRANSPORT_QUANTA / cap;
        for(unsigned int i = 0; i < lvls.size(); ++i)
        {
            if(lvls[i])
            {   partners[i]->equilibrate_stuff(&market_lvl, market_rule, trade_ratio, stuff_ID);}
        }
        commodityCount[stuff_ID] = market_lvl;
    }

    if (commodityCount[STUFF_JOBS] >= jobs)
    {
        consumeStuff(STUFF_JOBS, jobs);
        //Have to collect taxes here since transport does not consider the market a consumer but rather as another transport
        income_tax += jobs;
        ++working_days;
    }

    if(total_time % 50)
    if(commodityCount[STUFF_WASTE] >= 85 * MAX_WASTE_IN_MARKET / 100) {
        start_burning_waste = true;
        world(x+1,y+1)->pollution += MAX_WASTE_IN_MARKET/20;
        consumeStuff(STUFF_WASTE, (7 * MAX_WASTE_IN_MARKET) / 10);
    }

    //monthly update
    if (total_time % 100 == 99) {
        reset_prod_counters();
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
        }
        else if (market_ratio < 20)
        {
            jobs = JOBS_MARKET_LOW;
        }
        else if (market_ratio < 50)
        {
            jobs = JOBS_MARKET_MED;
        }
        else
        {
            jobs = JOBS_MARKET_FULL;
        }
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

void Market::animate() {
  if (market_ratio < 10) {
      frameIt->resourceGroup = ResourceGroup::resMap["MarketEmpty"];
  }
  else if (market_ratio < 20) {
      frameIt->resourceGroup = ResourceGroup::resMap["MarketLow"];
  }
  else if (market_ratio < 50) {
      frameIt->resourceGroup = ResourceGroup::resMap["MarketMed"];
  }
  else {
      frameIt->resourceGroup = ResourceGroup::resMap["MarketFull"];
  }
  soundGroup = frameIt->resourceGroup;

  if(start_burning_waste) { // start fire
    start_burning_waste = false;
    anim = real_time + ANIM_THRESHOLD(6 * WASTE_BURN_TIME);
  }
  if(real_time >= anim) { // stop fire
    waste_fire_frit->frame = -1;
  }
  else if(real_time >= waste_fire_anim) { // continue fire
    waste_fire_anim = real_time + ANIM_THRESHOLD(FIRE_ANIMATION_SPEED);
    int num_frames = waste_fire_frit->resourceGroup->graphicsInfoVector.size();
    if(++waste_fire_frit->frame >= num_frames)
      waste_fire_frit->frame = 0;
  }
}

void Market::report()
{
    int i = 0;

    mps_store_sd(i++, constructionGroup->name, ID);
    i++;
    mps_store_sfp(i++, N_("busy"), (float) busy);
    i++;
    //list_commodities(&i);
    for(Commodity stuff = STUFF_INIT ; stuff < STUFF_COUNT ; stuff++)
    {
        CommodityRule& rule = commodityRuleCount[stuff];
        if(!rule.maxload) continue;
        char arrows[4]="---";
        if (flags & FLAG_EVACUATE)
        {
            arrows[0] = '<';
            arrows[1] = '<';
            arrows[2] = ' ';
        }
        else
        {
            if (rule.take)
            {   arrows[2] = '>';}
            if (rule.give)
            {   arrows[0] = '<';}
        }

        if(i < 14)
        {
            mps_store_ssddp(i++, arrows, getStuffName(stuff), commodityCount[stuff], rule.maxload);
        }//endif
    } //endfor
}

void Market::toggleEvacuation()
{
    bool evacuate = flags & FLAG_EVACUATE; //actually the previous state
    for(Commodity stuff = STUFF_INIT ; stuff < STUFF_COUNT ; stuff++)
    {
        CommodityRule& rule = commodityRuleCount[stuff];
        if(!rule.maxload) continue;
        if(!evacuate)
        {
            rule.give = true;
            rule.take = false;
        }
        else
        {
            rule.give = true;
            rule.take = true;
        }

    }
    flags &= ~FLAG_EVACUATE;
    if(!evacuate)
    {   flags |= FLAG_EVACUATE;}
}

/** @file lincity/modules/market.cpp */
