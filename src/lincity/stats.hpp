/* ---------------------------------------------------------------------- *
 * src/lincity/stats.hpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 1995-1997 I J Peters
 * Copyright (C) 1997-2005 Greg Sharp
 * Copyright (C) 2000-2004 Corey Keasling
 * Copyright (C) 2025      David Bears
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

#ifndef __stats_h__
#define __stats_h__

#include <array>
#include <deque>  // for deque

#include "commodities.hpp"
#include "groups.hpp"

#define NUM_PBARS 15
#define PPOP    0
#define PTECH   1
#define PMONEY  2
#define PFOOD   3
#define PLABOR  4
#define PGOODS  5
#define PCOAL   6
#define PORE    7
#define PSTEEL  8
#define PPOL     9
#define PLOVOLT 10
#define PHIVOLT 11
#define PWATER  12
#define PWASTE  13
#define PHOUSE  14
#define PBAR_DATA_SIZE 1

template<typename T = int, T zero = 0>
class Stat {
public:

  operator T() { return stat; }
  void operator+=(const T& rhs) { acc += rhs; }
  void operator-=(const T& rhs) { acc -= rhs; }
  void operator++(int) { acc++; }
  void operator++() { ++acc; }
  void operator--(int) { acc--; }
  void operator--() { --acc; }
  T finalize() { stat = acc; acc = zero; return stat; }

  T stat = zero;
  T acc = zero;
};

class Stats {
public:
  Stats();
  ~Stats();

  struct {
    // monthly
    Stat<int> population_m;
    Stat<int> housed_m;
    Stat<int> starving_m;
    Stat<int> unemployed_m;
    Stat<int> housing_m;
    Stat<int> births_m;
    Stat<int> deaths_m;
    Stat<int> unnat_deaths_m;

    // totals
    int evacuated_t = 0;
    int births_t = 0;
    int pollution_deaths_t = 0;
    int starve_deaths_t = 0;
    int unemployed_days_t = 0;

    // decaying counters
    float unemployed_history = 0;
    float pollution_deaths_history = 0;
    float starve_deaths_history = 0;

    // maximum
    int max_pop_ever = 0;
  } population;

  struct {
    Stat<int> income_tax;
    Stat<int> coal_tax;
    Stat<int> goods_tax;
    Stat<int> export_tax;
  } income;

  struct {
    Stat<int> construction;
    Stat<int> coalSurvey;
    Stat<int> import;
    Stat<int> unemployment;
    Stat<int> transport;
    Stat<int> windmill;
    Stat<int> university;
    Stat<int> recycle;
    Stat<int> deaths;
    Stat<int> health;
    Stat<int> rockets;
    Stat<int> school;
    Stat<int> firestation;
    Stat<int> cricket;
    Stat<int> interest;
  } expenses;

  struct {
    bool mining_flag = true;
    int mining_years = 0;

    bool trade_flag = true;
    int trade_years = 0;

    int old_money = 0;
    int money_years = 0;

    int old_population = 0;
    int population_years = 0;

    int old_tech = 0;
    int tech_years = 0;

    int fire_years = 0;

    bool sustainable = false;
  } sustainability;

  template<typename I = int>
  struct Inventory {
    I amount;
    I capacity;

    template<typename O>
    Inventory<I>& operator+=(const Inventory<O>& rhs) {
      amount += rhs.amount;
      capacity += rhs.capacity;
      return *this;
    }
    template<typename O>
    Inventory<I>& operator-=(const Inventory<O>& rhs) {
      amount -= rhs.amount;
      capacity -= rhs.capacity;
      return *this;
    }

    int percent() {
      if(capacity == 0) return 0;
      return amount * 1000L / capacity;
    }
  };

  std::array<Inventory<Stat<>>, STUFF_COUNT> inventory;

  std::array<unsigned int, NUM_OF_GROUPS> groupCount;

  struct {
    // previously "monthgraph"
    std::deque<int> pop;
    std::deque<int> starve;
    std::deque<int> nojobs;
    std::deque<int> ppool;
    // previously "pbar"
    std::deque<int> tech;
    std::deque<int> money;
    std::deque<int> pollution;
    std::array<std::deque<Inventory<>>, STUFF_COUNT> inventory;
    std::deque<Inventory<>> tenants;
    // previously "q"
    std::deque<int> births;
    std::deque<int> deaths;
    std::deque<int> unnat_deaths;
  } history;

  int highest_tech_level = 0;
  int total_pollution = 0;

  // NOTE: These are NOT the official counters.
  int tech_level = 0;
  int total_money = 0;

  void daily();
  void monthly();
  void yearly();
};

#endif

/** @file lincity/stats.h */
