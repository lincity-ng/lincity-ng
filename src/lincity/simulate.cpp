/* ---------------------------------------------------------------------- *
 * src/lincity/simulate.cpp
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

#include <array>                           // for array
#include <cstdlib>                         // for rand, abs
#include <iterator>                        // for advance
#include <list>                            // for list, _List_iterator, oper...
#include <numeric>
#include <algorithm>

#include "Vehicles.h"                      // for Vehicle
#include "all_buildings.h"                 // for DAYS_BETWEEN_COVER, DAYS_P...
#include "commodities.hpp"                 // for CommodityRule, Commodity
#include "engglobs.h"                      // for tech_level, total_money
#include "groups.h"                        // for GROUP_FIRE, GROUP_MONUMENT
#include "gui_interface/pbar_interface.h"  // for update_pbars_monthly
#include "lin-city.h"                      // for MAX_TECH_LEVEL, TECH_LEVEL...
#include "lintypes.h"                      // for Construction, NUMOF_DAYS_I...
#include "modules/all_modules.h"           // for IMPORT_EXPORT_DISABLE_PERIOD
#include "modules/modules_interfaces.h"    // for update_shanty
#include "stats.h"                         // for export_tax, goods_tax, ly_...
#include "sustainable.h"                   // for SUST_MIN_TECH_LEVEL, SUST_...
#include "tinygettext/gettext.hpp"         // for _
#include "world.h"                         // for Map, MapTile
#include "lc_random.hpp"

/* extern resources */
extern void print_total_money(void);
void setSimulationDelay( int speed );
extern void ok_dial_box(const char *, int, const char *);

/* AL1: they are all in engine.cpp */
extern void do_daily_ecology(void);
extern void do_pollution(void);
extern void do_fire_health_cricket_power_cover(void);

/* Flag to warn users that they have 10 years to put waterwell everywhere */
int flag_warning = false;

/* ---------------------------------------------------------------------- *
 * Public Functions
 * ---------------------------------------------------------------------- */
void
World::do_time_step(void) {
    /* Increment game time */
    ++total_time;

    // update stats
    stats.daily();
    if(total_time % NUMOF_DAYS_IN_MONTH == 0)
      stats.monthly();
    if(total_time % NUMOF_DAYS_IN_YEAR == 0)
      stats.yearly();

    /* Run through simulation equations for each farm, residence, etc. */
    simulate_mappoints();

    /* Now do the stuff that happens once a year, once a month, etc. */
    do_periodic_events();
}

void
World::do_animate(unsigned long real_time) {
  Construction *construction;
  for(Construction *cst : map.constructions) {
    if(cst->isDead()) continue;
    cst->animate(real_time);
  }
  for(std::list<Vehicle*>::iterator it = vehicleList.begin();
    it != vehicleList.end();
  ) {
    (*(it++))->update(real_time);
  }
}

/* ---------------------------------------------------------------------- *
 * Private Functions
 * ---------------------------------------------------------------------- */

void
World::do_periodic_events(void) {
  do_daily_ecology();

  if(total_time % NUMOF_DAYS_IN_YEAR == 0)
    sustainability_test();

  if(total_time % DAYS_PER_POLLUTION == 3)
    do_pollution();

  int fire_rate = DAYS_BETWEEN_FIRES*100/map.len()*100/map.len();
  int fire_tech = GROUP_FIRESTATION_TECH * MAX_TECH_LEVEL / 1000;
  if(total_time % fire_rate == 9 && tech_level > fire_tech)
    do_random_fire();

  if(total_time % DAYS_BETWEEN_COVER == 75)
    do_fire_health_cricket_power_cover();

  if(total_time % DAYS_BETWEEN_SHANTY == 15
    && tech_level > GROUP_HEALTH_TECH * MAX_TECH_LEVEL / 1000
  )
    update_shanty(*this);

  if(total_time % NUMOF_DAYS_IN_MONTH == NUMOF_DAYS_IN_MONTH - 1)
    end_of_month_update();

  if(total_time % NUMOF_DAYS_IN_YEAR == NUMOF_DAYS_IN_YEAR - 1)
    end_of_year_update();
}

void
World::end_of_month_update(void) {
  //update queque of polluted tiles
  scan_pollution();

  if(people_pool > 100) {
    if(rand() % 1000 < people_pool)
      people_pool -= 10;
  }
  if(people_pool < 0) {
    assert(false);
    people_pool = 0;
  }
  stats.population.population_m += people_pool * NUMOF_DAYS_IN_MONTH;

  if(tech_level > TECH_LEVEL_LOSS_START) {
    tech_level -= (int)(tech_level * (1. / TECH_LEVEL_LOSS)
      * (1 + (stats.population.population_m.acc
        * (1. / NUMOF_DAYS_IN_MONTH / 120 / (TECH_LEVEL_LOSS - 200)))));
  }
  else
    tech_level += TECH_LEVEL_UNAIDED;
  /* we can go over 100, but it's even more difficult */
  if(tech_level > MAX_TECH_LEVEL) {
    tech_level -= (int)((tech_level - MAX_TECH_LEVEL) * (1. / TECH_LEVEL_LOSS)
      * (1 + (stats.population.population_m.acc
        * (1. / NUMOF_DAYS_IN_MONTH / 120 / (TECH_LEVEL_LOSS - 100)))));
  }
  if(tech_level > stats.highest_tech_level) {
    stats.highest_tech_level = tech_level;
  }
  stats.tech_level = tech_level;

  try {
    expense(stats.population.unnat_deaths_m.acc * UNNAT_DEATHS_COST,
      stats.expenses.deaths);
  } catch(const OutOfMoneyMessage::Exception& ex) {
    // TODO
  }

  try {
    expense(stats.population.unemployed_m.acc * money_rates.dole / 100,
      stats.expenses.unemployment);
  } catch(const OutOfMoneyMessage::Exception& ex) {
    // TODO
  }

  for(Construction *cst : map.constructions) {
    if(cst->isDead()) continue;
    cst->report_commodities();
  }

  stats.total_money = total_money;

  static bool noPeople = false;
  if(stats.population.population_m.acc == 0 && !noPeople) {
    pushMessage(NoPeopleLeftMessage::create(total_time));
    noPeople = true;
  }
}

void
World::end_of_year_update(void) {
  income(taxable.labor * money_rates.income_tax / 100,
    stats.income.income_tax);
  taxable.labor = 0;

  income(taxable.coal * money_rates.coal_tax / 10,
    stats.income.coal_tax);
  taxable.coal = 0;

  int goods_tax = (taxable.goods * money_rates.goods_tax) / 100;
  goods_tax += (int)((float)(goods_tax * money_rates.goods_tax)
    * (float)tech_level / 2000000.0);
  income(goods_tax, stats.income.goods_tax);
  taxable.goods = 0;

  /* The price of exports on the world market drops as you export more.
     The exporters have to discount there wares, therefore the
     tax take is less.
   */
  if(taxable.trade_ex > ex_tax_dis[0]) {
    int discount, disi;
    discount = 0;
    for(disi = 0;
      disi < NUMOF_DISCOUNT_TRIGGERS
        && taxable.trade_ex > ex_tax_dis[disi];
      disi++
    )
      discount += (taxable.trade_ex - ex_tax_dis[disi]) / 10;
    taxable.trade_ex -= discount;
  }
  income(taxable.trade_ex, stats.income.export_tax);
  taxable.trade_ex = 0;

  try {
    if(total_money < 0)
      expense(std::min((-total_money / 1000) * INTEREST_RATE, 1000000),
        stats.expenses.interest);
  } catch(const OutOfMoneyMessage::Exception& ex) {
    // TODO
  }

  if(total_money > 2000000000)
    total_money = 2000000000;
  else if(total_money < -2000000000)
    total_money = -2000000000;

  setUpdated(Updatable::MONEY);

  // change import/export ability
  for(Commodity s = STUFF_INIT; s < STUFF_COUNT; s++) {
    tradeRule[s].take ^= !(rand() % (tradeRule[s].take
      ? IMPORT_EXPORT_DISABLE_PERIOD : IMPORT_EXPORT_ENABLE_PERIOD));
    tradeRule[s].give ^= !(rand() % (tradeRule[s].give
      ? IMPORT_EXPORT_DISABLE_PERIOD : IMPORT_EXPORT_ENABLE_PERIOD));
  }
}

void
World::simulate_mappoints(void) {
  // We could directly shuffle constructions, but the swaps could be more
  // expensive if we decide down the line to store Construction's directly
  // instead of using pointers.
  std::vector<decltype(map.constructions)::iterator> ordering(
    map.constructions.size());
  std::iota(ordering.begin(), ordering.end(), map.constructions.begin());
  std::shuffle(ordering.begin(), ordering.end(), LcUrbg::get());
  for(auto cstIt : ordering) {
    Construction *cst = *cstIt;
    if(cst->isDead()) {
      // TODO: Consider move pruning to its own function.
      map.constructions.erase(cstIt);
      delete cst;
      continue;
    }
    cst->trade();
    cst->update();
  }
}

void
World::sustainability_test(void) {
  int i;
  if(!stats.sustainability.mining_flag) {
    stats.sustainability.mining_flag = true;
    stats.sustainability.mining_years = 0;
  } else
    stats.sustainability.mining_years++;

  if(!stats.sustainability.trade_flag) {
    stats.sustainability.trade_flag = true;
    stats.sustainability.trade_years = 0;
  } else
    stats.sustainability.trade_years++;

  /* Money must be going up or the same. (ie can't build.) */
  if(stats.sustainability.old_money > total_money)
    stats.sustainability.money_years = 0;
  else
    stats.sustainability.money_years++;
  stats.sustainability.old_money = total_money;

  /* population must be withing 2% of when it started. */
  i = stats.population.population_m - stats.sustainability.old_population;
  if(abs(i) > stats.sustainability.old_population / 40     /* 2.5%  */
    || stats.population.population_m < SUST_MIN_POPULATION * NUMOF_DAYS_IN_MONTH
  ) {
    stats.sustainability.old_population = stats.population.population_m;
    stats.sustainability.population_years = 0;
  } else
    stats.sustainability.population_years++;

  /* tech level must be going up or not fall more than 0.5% from it's
     highest during the sus count
   */
  i = tech_level - stats.sustainability.old_tech;
  if(-i > stats.sustainability.old_tech / 100 ||
    tech_level < SUST_MIN_TECH_LEVEL
  ) {
    stats.sustainability.tech_years = 0;
    stats.sustainability.old_tech = tech_level;
  }
  else {
    stats.sustainability.tech_years++;
    if(i > 0)
      stats.sustainability.old_tech = tech_level;
  }

  /* check fire cover only every three years */
  if(total_time % (NUMOF_DAYS_IN_YEAR * 3) == 0) {
    if(sust_fire_cover())
      stats.sustainability.fire_years += 3;
    else
      stats.sustainability.fire_years = 0;
  }

  if(stats.sustainability.mining_years >= SUST_ORE_COAL_YEARS_NEEDED
    && stats.sustainability.trade_years >= SUST_PORT_YEARS_NEEDED
    && stats.sustainability.money_years >= SUST_MONEY_YEARS_NEEDED
    && stats.sustainability.population_years >= SUST_POP_YEARS_NEEDED
    && stats.sustainability.tech_years >= SUST_TECH_YEARS_NEEDED
    && stats.sustainability.fire_years >= SUST_FIRE_YEARS_NEEDED
  ) {
    if(!stats.sustainability.sustainable) {
      pushMessage(SustainableEconomyMessage::create(total_time));
    }
    stats.sustainability.sustainable = true;
  }
}

bool
World::sust_fire_cover(void) {
  for(Construction *cst : map.constructions) {
    if(cst->isDead()) continue;
    if(cst->flags & FLAG_IS_TRANSPORT)
      continue;
    unsigned short grp = cst->constructionGroup->group;
    if(grp == GROUP_MONUMENT
      || grp == GROUP_OREMINE
      || grp == GROUP_ROCKET
      || grp == GROUP_FIRE
      || grp == GROUP_POWER_LINE
    )
      continue;
    if(!(map(cst->point)->flags & FLAG_FIRE_COVER))
      return false;
  }
  return true;
}

/** @file lincity/simulate.cpp */
