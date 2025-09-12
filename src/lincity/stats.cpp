/* ---------------------------------------------------------------------- *
 * src/lincity/stats.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 1995-1997 I J Peters
 * Copyright (C) 1997-2005 Greg Sharp
 * Copyright (C) 2000-2004 Corey Keasling
 * Copyright (C) 2025      David Bears <dbear4q@gmail.com>
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

#include "stats.hpp"

#include "commodities.hpp"  // for Commodity, operator++

Stats::Stats() {
  for(unsigned int& gc : groupCount) gc = 0;

  history.pop.resize(190);
  history.starve.resize(190);
  history.nojobs.resize(190);
  history.ppool.resize(190);
  history.tech.resize(12);
  history.money.resize(12);
  history.pollution.resize(2);
  for(auto& i : history.inventory)
    i.resize(2);
  history.births.resize(12);
  history.deaths.resize(12);
  history.unnat_deaths.resize(12);
  history.tenants.resize(12);
}

Stats::~Stats() { }

void
Stats::daily() {
}

void
Stats::monthly() {
  population.population_m.finalize();
  population.housed_m.finalize();
  population.housing_m.finalize();
  population.starving_m.finalize();
  population.unemployed_m.finalize();
  population.births_m.finalize();
  population.deaths_m.finalize();
  population.unnat_deaths_m.finalize();

  for(Commodity stuff = STUFF_INIT; stuff < STUFF_COUNT; stuff++) {
    inventory[stuff].amount.finalize();
    inventory[stuff].capacity.finalize();
    history.inventory[stuff].push_front({
      .amount = inventory[stuff].amount,
      .capacity = inventory[stuff].capacity
    });
    history.inventory[stuff].pop_back();
  }

  history.pop.push_front(population.population_m);
  history.starve.push_front(population.starving_m);
  history.nojobs.push_front(population.unemployed_m);
  history.ppool.push_front(population.population_m - population.housed_m);
  history.tech.push_front(tech_level);
  history.money.push_front(total_money);
  history.pollution.push_front(total_pollution);
  history.tenants.push_front({
    .amount = population.housed_m,
    .capacity = population.housing_m
  });
  history.births.push_front(population.births_m);
  history.deaths.push_front(population.deaths_m);
  history.unnat_deaths.push_front(population.unnat_deaths_m);
  history.pop.pop_back();
  history.starve.pop_back();
  history.nojobs.pop_back();
  history.ppool.pop_back();
  history.tech.pop_back();
  history.money.pop_back();
  history.pollution.pop_back();
  history.tenants.pop_back();
  history.births.pop_back();
  history.deaths.pop_back();
  history.unnat_deaths.pop_back();
}

void
Stats::yearly() {
  population.unemployed_history *= 0.99;
  population.pollution_deaths_history *= 0.99;
  population.starve_deaths_history *= 0.99;

  income.income_tax.finalize();
  income.coal_tax.finalize();
  income.goods_tax.finalize();
  income.export_tax.finalize();

  expenses.construction.finalize();
  expenses.coalSurvey.finalize();
  expenses.import.finalize();
  expenses.unemployment.finalize();
  expenses.transport.finalize();
  expenses.windmill.finalize();
  expenses.university.finalize();
  expenses.recycle.finalize();
  expenses.deaths.finalize();
  expenses.health.finalize();
  expenses.rockets.finalize();
  expenses.school.finalize();
  expenses.firestation.finalize();
  expenses.cricket.finalize();
  expenses.interest.finalize();
}

/** @file lincity/stats.cpp */
