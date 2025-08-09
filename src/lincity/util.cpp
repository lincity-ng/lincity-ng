/* ---------------------------------------------------------------------- *
 * src/lincity/util.cpp
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

#include "util.hpp"

#include <stdio.h>               // for snprintf
#include <string.h>              // for strdup
#include <algorithm>             // for min
#include <cstdlib>               // for abs
#include <string>                // for basic_string, string

#include "lintypes.hpp"          // for NUMOF_DAYS_IN_YEAR, NUMOF_DAYS_IN_MONTH
#include "util/gettextutil.hpp"  // for N_, _

// TODO: move to NG
const char *current_month(int current_time) {
  static const char *months[] = { (char*)N_("Jan"), (char*)N_("Feb"), (char*)N_("Mar"), (char*)N_("Apr"),
      (char*)N_("May"), (char*)N_("Jun"), (char*)N_("Jul"), (char*)N_("Aug"),
      (char*)N_("Sep"), (char*)N_("Oct"), (char*)N_("Nov"), (char*)N_("Dec")
  };
  static bool doinit = true;
  if (doinit) {
    doinit = false;
    for (int i = 0; i < 12; ++i) {
      months[i] = strdup(_(months[i]));
    }
  }
  return months[(current_time % NUMOF_DAYS_IN_YEAR) / NUMOF_DAYS_IN_MONTH];
}

int current_year(int current_time) {
  return current_time / NUMOF_DAYS_IN_YEAR;
}

std::string num_to_ansi(long num) {
  int triplets = 0;
  float numf = (float)num;
  if(abs(numf) > 9999) do {
    numf /= 1000;
    triplets++;
  } while(abs(numf) > 999);

  switch(triplets) {
  case 0:
    triplets = ' ';
    break;
  case 1:
    triplets = 'k';
    break;                  // kilo
  case 2:
    triplets = 'M';
    break;                  // mega
  case 3:
    triplets = 'G';
    break;                  // giga
  case 4:
    triplets = 'T';
    break;                  // tera
  case 5:
    triplets = 'P';
    break;                  // peta
  default:
    triplets = '?';
    break;
  }

  std::string str;
  str.resize(7);
  int size_old = str.size();
  int size_new;
  if(triplets == ' ') {
    size_new = snprintf(str.data(), str.size()+1, "%4.0f", numf);
  } else {
    size_new = snprintf(str.data(), str.size()+1, "%3.1f%c", numf, triplets);
  }
  str.resize(std::min(size_old, size_new));
  return str;
}
