/* ---------------------------------------------------------------------- *
 * src/lincity-ng/Mps.hpp
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

#ifndef __lc_mps_h__
#define __lc_mps_h__

#include <string>             // for string
#include <vector>             // for vector

#include "gui/Component.hpp"  // for Component

class Game;
class MapTile;
class Paragraph;
namespace xmlpp {
class TextReader;
}  // namespace xmlpp

/**
 * The MPS component is more or less a text component with 13 lines that
 * is used to display status information about the game/engine.
 */
class Mps : public Component {
public:
  Mps();
  ~Mps();

  void parse(xmlpp::TextReader& reader);

  void reset();
  void clear();
  void addText(const std::string &s);
  void addBlank();
  void fillBlank();
  bool isFull() const;

  void add_s(const std::string &s);
  void add_fp(int i, double f);
  void add_f(double f);
  void add_sf(const std::string& s, double fl);
  void add_d(int d);
  void add_ss(const std::string& s1, const std::string& s2);
  void add_sd(const std::string& s, int d);
  void add_ssd(const std::string& s1, const std::string& s2, int d);
  void add_sfp(const std::string& s, double fl);
  void add_sdd(const std::string& s, int d1, int d2);
  void add_sddp(const std::string& s, int d, int max);
  void add_tsddp(const std::string& t1, const std::string& s2, int d, int max);
  void add_ttt(const std::string& s1, const std::string& s2,
    const std::string& s3);

  static const int PARAGRAPH_COUNT = 13;

private:
  std::vector<Paragraph *> paragraphs;
  int i = 0;
};

class MpsMap : public Mps {
public:
  enum class Page {
    INVENTORY, PRODUCTION, GROUND
  };

  MpsMap() { }
  ~MpsMap() { }

  void refresh();
  void setTile(MapTile *tile);
  void scroll();
  void query(MapTile *tile);

  Page page = Page::INVENTORY;
  MapTile *tile = nullptr;

private:

  void refreshInvProd(bool production);
  void refreshGround();
};

class MpsFinance : public Mps {
public:
  enum class Page {
    CASH_FLOW, COSTS, POPULATION
  };

  MpsFinance() { }
  ~MpsFinance() { }

  void refresh();
  void setGame(Game *game) { this->game = game; }

  Page page = Page::CASH_FLOW;

private:
  Game *game = nullptr;

  void refreshCashFlow();
  void refreshCosts();
  void refreshPopulation();
};

#endif

/** @file lincity-ng/Mps.hpp */
