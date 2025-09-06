/* ---------------------------------------------------------------------- *
 * src/lincity-ng/PBar.hpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2005      David Kamphausen <david.kamphausen@web.de>
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

#ifndef __lc_pbar_h__
#define __lc_pbar_h__

#include <vector>             // for vector

#include "gui/Component.hpp"  // for Component

class Game;
namespace xmlpp {
class TextReader;
}  // namespace xmlpp

class LCPBar : public Component
{
public:
  LCPBar();
  ~LCPBar();

  void parse(xmlpp::TextReader& reader);

  void refresh();
  void setGame(Game *game);

private:
  enum class Bar {
    POP, TECH, MONEY, FOOD, LABOR, GOODS, COAL, ORE, STEEL, POL, LOVOLT, HIVOLT,
    WATER, WASTE, HOUSE
  };

  void setValue(Bar bar, int value, int diff);
  std::vector<Bar> bars;
  Game *game = nullptr;
};

class BarView : public Component
{
public:
  BarView();
  ~BarView();

  void parse(xmlpp::TextReader& reader);

  void setValue(float v);
  virtual void draw(Painter &painter);
private:
  float value;
  bool dir;
  bool bad;
};

#endif

/** @file lincity-ng/PBar.hpp */
