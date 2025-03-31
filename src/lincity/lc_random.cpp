/* ---------------------------------------------------------------------- *
 * src/lincity/lc_random.cpp
 * This file is part of Lincity-NG.
 *
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

#include "lc_random.hpp"

#include <time.h>     // for time
#include <algorithm>  // for generate
#include <random>     // for random_device, seed_seq
#include <vector>     // for vector

LcUrbg::LcUrbg() {
  std::vector<std::random_device::result_type> entropy(9);
  std::generate(entropy.begin(), entropy.begin() + 8, std::random_device());
  entropy[8] = time(nullptr);
  std::seed_seq seed(entropy.begin(), entropy.end());
  base_urbg.seed(seed);
}

LcUrbg::~LcUrbg() = default;

LcUrbg&
LcUrbg::get() {
  static thread_local LcUrbg instance;
  return instance;
}

LcUrbg::result_type
LcUrbg::operator()() {
  return base_urbg();
}
