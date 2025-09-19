/* ---------------------------------------------------------------------- *
 * src/lincity/modules/residence.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 1995-1997 I J Peters
 * Copyright (C) 1997-2005 Greg Sharp
 * Copyright (C) 2000-2004 Corey Keasling
 * Copyright (C) 2022-2025 David Bears <dbear4q@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
** ---------------------------------------------------------------------- */

#include "residence.hpp"

#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml/xmlwriter.h>             // for xmlTextWriterWriteElement
#include <stdlib.h>                       // for rand
#include <iostream>                       // for basic_ostream, operator<<
#include <string>                         // for basic_string, char_traits

#include "lincity-ng/Mps.hpp"             // for Mps
#include "lincity/MapPoint.hpp"           // for MapPoint, operator<<
#include "lincity/lin-city.hpp"           // for FALSE, FLAG_EMPLOYED, FLAG_FED
#include "lincity/stats.hpp"              // for Stats, Stat
#include "lincity/world.hpp"              // for World, Map, MapTile
#include "util/xmlutil.hpp"               // for xmlFormat, xmlParse, xmlStr
#include "util/gettextutil.hpp"

ResidenceConstructionGroup residenceLLConstructionGroup(
    N_("Residence"),
    N_("Residences"),
    FALSE,                     /* need credit? */
    GROUP_RESIDENCE_LL,
    GROUP_RESIDENCE_SIZE,
    GROUP_RESIDENCE_LL_COLOUR,
    GROUP_RESIDENCE_LL_COST_MUL,
    GROUP_RESIDENCE_LL_BUL_COST,
    GROUP_RESIDENCE_LL_FIREC,
    GROUP_RESIDENCE_LL_COST,
    GROUP_RESIDENCE_LL_TECH,
    GROUP_RESIDENCE_RANGE
);

ResidenceConstructionGroup residenceMLConstructionGroup(
    N_("Residence"),
    N_("Residences"),
    FALSE,                     /* need credit? */
    GROUP_RESIDENCE_ML,
    GROUP_RESIDENCE_SIZE,
    GROUP_RESIDENCE_ML_COLOUR,
    GROUP_RESIDENCE_ML_COST_MUL,
    GROUP_RESIDENCE_ML_BUL_COST,
    GROUP_RESIDENCE_ML_FIREC,
    GROUP_RESIDENCE_ML_COST,
    GROUP_RESIDENCE_ML_TECH,
    GROUP_RESIDENCE_RANGE
);

ResidenceConstructionGroup residenceHLConstructionGroup(
    N_("Residence"),
    N_("Residences"),
    FALSE,                     /* need credit? */
    GROUP_RESIDENCE_HL,
    GROUP_RESIDENCE_SIZE,
    GROUP_RESIDENCE_HL_COLOUR,
    GROUP_RESIDENCE_HL_COST_MUL,
    GROUP_RESIDENCE_HL_BUL_COST,
    GROUP_RESIDENCE_HL_FIREC,
    GROUP_RESIDENCE_HL_COST,
    GROUP_RESIDENCE_HL_TECH,
    GROUP_RESIDENCE_RANGE
);

ResidenceConstructionGroup residenceLHConstructionGroup(
    N_("Residence"),
    N_("Residences"),
    FALSE,                     /* need credit? */
    GROUP_RESIDENCE_LH,
    GROUP_RESIDENCE_SIZE,
    GROUP_RESIDENCE_LH_COLOUR,
    GROUP_RESIDENCE_LH_COST_MUL,
    GROUP_RESIDENCE_LH_BUL_COST,
    GROUP_RESIDENCE_LH_FIREC,
    GROUP_RESIDENCE_LH_COST,
    GROUP_RESIDENCE_LH_TECH,
    GROUP_RESIDENCE_RANGE
);

ResidenceConstructionGroup residenceMHConstructionGroup(
    N_("Residence"),
    N_("Residences"),
    FALSE,                     /* need credit? */
    GROUP_RESIDENCE_MH,
    GROUP_RESIDENCE_SIZE,
    GROUP_RESIDENCE_MH_COLOUR,
    GROUP_RESIDENCE_MH_COST_MUL,
    GROUP_RESIDENCE_MH_BUL_COST,
    GROUP_RESIDENCE_MH_FIREC,
    GROUP_RESIDENCE_MH_COST,
    GROUP_RESIDENCE_MH_TECH,
    GROUP_RESIDENCE_RANGE
);

ResidenceConstructionGroup residenceHHConstructionGroup(
    N_("Residence"),
    N_("Residences"),
    FALSE,                     /* need credit? */
    GROUP_RESIDENCE_HH,
    GROUP_RESIDENCE_SIZE,
    GROUP_RESIDENCE_HH_COLOUR,
    GROUP_RESIDENCE_HH_COST_MUL,
    GROUP_RESIDENCE_HH_BUL_COST,
    GROUP_RESIDENCE_HH_FIREC,
    GROUP_RESIDENCE_HH_COST,
    GROUP_RESIDENCE_HH_TECH,
    GROUP_RESIDENCE_RANGE
);


Construction *ResidenceConstructionGroup::createConstruction(World& world) {
  return new Residence(world, this);
}

Residence::Residence(World& world, ConstructionGroup *cstgrp) :
  Construction(world)
{
  this->constructionGroup = cstgrp;
  this->local_population = 0;
  this->desireability = 0;
  this->births = 120000;
  this->deaths = 120000;
  this->pol_deaths = 0;
  if (cstgrp == &residenceLLConstructionGroup)
    this->max_population = GROUP_RESIDENCE_LL_MAX_POP;
  else if (cstgrp == &residenceMLConstructionGroup)
    this->max_population = GROUP_RESIDENCE_ML_MAX_POP;
  else if (cstgrp == &residenceHLConstructionGroup)
    this->max_population = GROUP_RESIDENCE_HL_MAX_POP;
  else if (cstgrp == &residenceLHConstructionGroup)
    this->max_population = GROUP_RESIDENCE_LH_MAX_POP;
  else if (cstgrp == &residenceMHConstructionGroup)
    this->max_population = GROUP_RESIDENCE_MH_MAX_POP;
  else if (cstgrp == &residenceHHConstructionGroup)
    this->max_population = GROUP_RESIDENCE_HH_MAX_POP;
  else {
    this->max_population = 50;
    std::cout << "unknown ConstructionGroup in new Residence at "
      << point << std::endl;
  }

  initialize_commodities();
  commodityMaxCons[STUFF_FOOD] = 100 * max_population;
  commodityMaxCons[STUFF_WATER] = 100 * max_population;
  commodityMaxCons[STUFF_LOVOLT] = 100 * (POWER_RES_OVERHEAD +
    (POWER_USE_PER_PERSON * max_population) + max_population/2);
  commodityMaxProd[STUFF_LABOR] = 100 * (max_population * (
      WORKING_POP_PERCENT + JOB_SWING +
      HC_WORKING_POP + HC_JOB_SWING +
      CRICKET_WORKING_POP + CRICKET_JOB_SWING)
    / 100);
  commodityMaxCons[STUFF_GOODS] = 100 * (max_population / 4) * 2;
  commodityMaxProd[STUFF_WASTE] = 100 * (max_population / 12) * 2;
}

Residence::~Residence() {
  //everyone survives demolition
  world.people_pool += local_population;
}

void Residence::update()
{
    int bad = 35, good = 30;    /* (un)desirability of living here */
    int r, po, swing;
    int brm = 0, drm = 0;       /* birth/death rate modifier */
    /* birts = 1/(BASE_BR + brm) deaths = 1/(BASE_DR - drm)
    the different signs are intentional higher brm less a little less babys, higher drm much more deaths*/
    bool cc = false;                 /* extra labor from sports activity*/
    int birth_flag = (FLAG_FED | FLAG_EMPLOYED);/* can we have babies*/
    bool extra_births = false;  /* full houses are more fertile*/
    bool hc = false;            /* have health cover ? */
    //int pol_death = 0;             //sometimes pollution kills

    /*Determine Health,Fire,Cricket cover*/
    if((hc = world.map(point)->flags & FLAG_HEALTH_COVER)) {
      brm = RESIDENCE_BRM_HEALTH;
      good += 15;
    }
    if (world.map(point)->flags & FLAG_FIRE_COVER)
      good += 15;
    else
      bad += 5;
    if((cc = world.map(point)->flags & FLAG_CRICKET_COVER)) {
      good += 20;
    }

    /* now get fed */
    if (   (commodityCount[STUFF_FOOD] >= local_population)
        && (commodityCount[STUFF_WATER] >= local_population)
        //&& (world(x,y)->flags & FLAG_WATERWELL_COVER)
        && local_population)
    {
        consumeStuff(STUFF_FOOD, local_population);
        consumeStuff(STUFF_WATER, local_population);
        flags |= (FLAG_FED); //enable births
        good += 10;
    }
    else
    {
        flags &= ~(FLAG_FED); //disable births
        if (local_population)
        {
            if (rand() % DAYS_PER_STARVE == 1)
            {
                local_population--; //starving maybe deadly
                ++world.stats.population.deaths_m;
                ++world.stats.population.unnat_deaths_m;
                ++world.stats.population.starve_deaths_t;
                world.stats.population.starve_deaths_history += 1.0;
            }
            world.stats.population.starving_m += local_population; //only the survivors are starving
            bad += 250; // This place really sucks
            drm = 100; //starving is also unhealty
        }
    }

     /* now get power for nothing */
    if (commodityCount[STUFF_LOVOLT] >= POWER_RES_OVERHEAD + (POWER_USE_PER_PERSON * local_population))
    {
        consumeStuff(STUFF_LOVOLT, POWER_RES_OVERHEAD + (POWER_USE_PER_PERSON * local_population));
        flags |= FLAG_HAD_POWER;
        good += 10;
    }
    else
    {
        bad += 15;
        if ((flags & FLAG_HAD_POWER))
        {   bad += 50;}
    }

    /* now supply labor and buy goods if employed */
    const int& labor_cap =
      constructionGroup->commodityRuleCount[STUFF_LABOR].maxload;
    swing = JOB_SWING + (hc?HC_JOB_SWING:0) + (cc?CRICKET_JOB_SWING:0);
    int working_pop = WORKING_POP_PERCENT + (hc?HC_WORKING_POP:0) +
      (cc?CRICKET_WORKING_POP:0);
    int swing_amt = swing * (labor_cap - commodityCount[STUFF_LABOR]) / labor_cap;
    int job_prod = local_population * (working_pop + swing_amt) / 100;
    if (labor_cap - commodityCount[STUFF_LABOR] >= job_prod) {
        produceStuff(STUFF_LABOR, job_prod);
        flags |= FLAG_EMPLOYED; //enable births
        good += 20;
        if ((commodityCount[STUFF_GOODS] >= local_population/4)
        &&  (constructionGroup->commodityRuleCount[STUFF_WASTE].maxload-commodityCount[STUFF_WASTE] >= local_population/12))
        {
            consumeStuff(STUFF_GOODS, local_population/4);
            produceStuff(STUFF_WASTE, local_population/12);
            good += 10;
            if (commodityCount[STUFF_LOVOLT] >= local_population/2)
            {
                consumeStuff(STUFF_LOVOLT, local_population/2);
                good += 5;
                brm += 10;
                /*     buy more goods if got power for them */
                if ((commodityCount[STUFF_GOODS] >= local_population/4)
                &&  (constructionGroup->commodityRuleCount[STUFF_WASTE].maxload-commodityCount[STUFF_WASTE] >= local_population/12))
                {
                    consumeStuff(STUFF_GOODS, local_population/4);
                    produceStuff(STUFF_WASTE, local_population/12);
                    good += 5;
                }
            }
            else
            {   bad += 5;}
        }
    }
    else
    {
        flags &= ~(FLAG_EMPLOYED); //disable births
        world.stats.population.unemployed_m += local_population;
        world.stats.population.unemployed_days_t += local_population;
        world.stats.population.unemployed_history +=
          (double)local_population / NUMOF_DAYS_IN_YEAR;
        bad += 70;
    }

    switch (constructionGroup->group)
    {
        case GROUP_RESIDENCE_LL:
            drm += local_population * 7 * 50/24; //more people more deaths
            brm += RESIDENCE_LL_BRM + (extra_births?100:0); //slow down baby production
        break;
        case GROUP_RESIDENCE_ML:
            drm += local_population * 3 * 5/3; //more people more deaths
            brm += RESIDENCE_ML_BRM + (extra_births?50:0); //slow down baby production
        break;
        case GROUP_RESIDENCE_HL:
            drm += local_population * 1 * 6/3; //more people more deaths
            brm += RESIDENCE_HL_BRM + (extra_births?50:0); //slow down baby production
            good += 40;
        break;
        case GROUP_RESIDENCE_LH:
            drm += local_population * 3 * 7/3; //more people more deaths
            brm += RESIDENCE_LH_BRM + (extra_births?100:0); //slow down baby production
        break;
        case GROUP_RESIDENCE_MH:
            drm += local_population / 2 * 4/3; //more people more deaths
            brm += RESIDENCE_MH_BRM + (extra_births?50:0); //slow down baby production
        break;
        case GROUP_RESIDENCE_HH:
            drm += local_population * 4/3; //more people more deaths
            brm += RESIDENCE_HH_BRM + (extra_births?50:0); //slow down baby production
            good += 100;
        break;
    }
    drm += local_population / 4;
    brm += local_population / 4;
    if (drm > RESIDENCE_BASE_DR - 1)
    {   drm = RESIDENCE_BASE_DR - 1;}
    /* normal deaths + pollution deaths */
    po = ((world.map(point)->pollution / 16) + 1);
    pol_deaths = po>100?95:po-5>0?po-5:1;
    deaths = (RESIDENCE_BASE_DR - drm - 3*po);
    if (deaths < 1) deaths = 1;
    if (hc) deaths *= 4;
    r = rand() % deaths;
    if (local_population > 0 ) //somebody might die
    {
        if (r == 0) //one guy had bad luck
        {
            --local_population;
            ++world.stats.population.deaths_m;
            if(rand() % 100 < pol_deaths) // deadly pollution
            {
                world.stats.population.unnat_deaths_m++;
                world.stats.population.pollution_deaths_t++;
                world.stats.population.pollution_deaths_history += 1.0;
                bad += 100;
            }
        }
    }
    else //no death in hundred years
    {   deaths = 120000;}

    /* normal births FED and EMPLOYED */
    births = RESIDENCE_BASE_BR + brm;
    if (((flags & birth_flag) == birth_flag)
        && (local_population > 0))
    {
        if (rand() % births == 0)
        {
            ++local_population;
            ++world.stats.population.births_t;
            ++world.stats.population.births_m;
            good += 50;
        }
    }
    else //no baby in hundred years
    {   births = 120000;}

    /* people_pool stuff */
    //bad += local_population / 2;
    bad += world.map(point)->pollution / 20;
    good += world.people_pool / 27; //27
    desireability = good-bad;
    r = rand() % ((good + bad) * RESIDENCE_PPM);
    if (r < bad || local_population > max_population)
    {
        if (local_population > MIN_RES_POPULATION)
        {
            --local_population;
            ++world.people_pool;
        }
    } else if (world.people_pool > 0 && local_population < max_population
               && r > ((good + bad) * (RESIDENCE_PPM - 1) + bad))  /* r > (rmax - good) */
    {
        ++local_population;
        --world.people_pool;
    }
    /* XXX AL1: this is daily accumulator used stats.cpp, and maybe pop graph */
    world.stats.population.population_m += local_population;
    world.stats.population.housed_m += local_population;
    world.stats.population.housing_m += max_population;

    if(world.total_time % 100 == 99) {
      reset_prod_counters();
    }
}

void
Residence::torch() {
  // people die in the fire
  int casualities = local_population / 2;
  world.stats.population.unnat_deaths_m += casualities;
  world.stats.population.deaths_m += casualities;
  world.people_pool += local_population - casualities;
  local_population = 0; // so the deleter doesn't double-count

  Construction::torch();
}

void Residence::report(Mps& mps, bool production) const {
  mps.add_s(constructionGroup->name);
  mps.add_sddp(N_("Tenants"), local_population, max_population);
  mps.add_sd(N_("Desireability"), desireability);
  mps.add_sf(N_("Births p.a."), (float)1200/births);
  mps.add_sf(N_("Death p.a."), (float)1200/deaths);
  mps.add_sfp(N_("Unnat. mortality"), (float)pol_deaths);
  list_commodities(mps, production);
}

void Residence::save(xmlTextWriterPtr xmlWriter) const {
  xmlTextWriterWriteElement(xmlWriter, (xmlStr)"local_population", xmlFormat<int>(local_population));
  Construction::save(xmlWriter);
}

bool Residence::loadMember(xmlpp::TextReader& xmlReader, unsigned int ldsv_version) {
  std::string name = xmlReader.get_name();
  if(name == "local_population") local_population = xmlParse<int>(xmlReader.read_inner_xml());
  else return Construction::loadMember(xmlReader, ldsv_version);
  return true;
}

/** @file lincity/modules/residence.cpp */
