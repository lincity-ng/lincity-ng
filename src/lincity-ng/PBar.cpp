/* ---------------------------------------------------------------------- *
 * src/lincity-ng/PBar.cpp
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

#include "PBar.hpp"

#include <libxml++/parsers/textreader.h>  // for TextReader
#include <libxml++/ustring.h>             // for ustring
#include <algorithm>                      // for max
#include <array>                          // for array
#include <cassert>                        // for assert
#include <cmath>                          // for sqrt
#include <deque>                          // for deque
#include <iomanip>                        // for operator<<, setprecision
#include <iostream>                       // for basic_ostream, operator<<
#include <memory>                         // for unique_ptr
#include <sstream>                        // for basic_ostringstream
#include <stdexcept>                      // for runtime_error
#include <string>                         // for char_traits, basic_string
#include <utility>                        // for move

#include "Game.hpp"                       // for Game
#include "Util.hpp"                       // for getParagraph
#include "gui/Color.hpp"                  // for Color
#include "gui/ComponentFactory.hpp"       // for IMPLEMENT_COMPONENT_FACTORY
#include "gui/ComponentLoader.hpp"        // for parseEmbeddedComponent
#include "gui/Painter.hpp"                // for Painter
#include "gui/Paragraph.hpp"              // for Paragraph
#include "gui/Rect2D.hpp"                 // for Rect2D
#include "lincity/commodities.hpp"        // for Commodity
#include "lincity/lintypes.hpp"           // for NUMOF_DAYS_IN_MONTH
#include "lincity/stats.hpp"              // for Stats
#include "lincity/util.hpp"               // for num_to_ansi
#include "lincity/world.hpp"              // for World
#include "util/xmlutil.hpp"               // for xmlParse, unexpectedXmlAttr...


LCPBar::LCPBar() { }

LCPBar::~LCPBar() { }

void
LCPBar::parse(xmlpp::TextReader& reader) {
  while(reader.move_to_next_attribute()) {
    xmlpp::ustring name = reader.get_name();
    xmlpp::ustring value = reader.get_value();
    if(parseAttribute(reader));
    else
      unexpectedXmlAttribute(reader);
  }
  reader.move_to_element();

  if(getName() == "PBar")
    bars = {Bar::POP, Bar::TECH, Bar::MONEY,
      Bar::FOOD, Bar::LABOR, Bar::GOODS, Bar::COAL, Bar::ORE, Bar::STEEL};
  else if(getName() == "PBar2nd")
    bars = {Bar::POP, Bar::TECH, Bar::MONEY,
      Bar::POL, Bar::LOVOLT, Bar::HIVOLT, Bar::WATER, Bar::WASTE, Bar::HOUSE};
  else {
    std::cerr << "Unknown LCBar component '" << getName() << "' found.\n";
    assert(false);
  }

  std::unique_ptr<Component> component = parseEmbeddedComponent(reader);
  width = component->getWidth();
  height = component->getHeight();
  addChild(std::move(component));
}

void
LCPBar::refresh() {
  Stats& stats = game->getWorld().stats;
  for(int b = 0; b < bars.size(); b++) {
    Bar bar = bars[b];

    int val, diff, cap;
    switch(bar) {
    case Bar::POP:
      val = stats.history.pop.at(0) / NUMOF_DAYS_IN_MONTH;
      diff = val - stats.history.pop.at(11) / NUMOF_DAYS_IN_MONTH;
      cap = 0;
      break;
    case Bar::TECH:
      val = stats.history.tech.at(0);
      diff = val - stats.history.tech.at(11);
      cap = 0;
      break;
    case Bar::MONEY:
      val = stats.history.money.at(0);
      diff = val - stats.history.money.at(11);
      cap = 0;
      break;
    case Bar::FOOD:
      val = stats.history.inventory[STUFF_FOOD].at(0).amount;
      diff = val - stats.history.inventory[STUFF_FOOD].at(1).amount;
      cap = stats.history.inventory[STUFF_FOOD].at(0).capacity;
      break;
    case Bar::LABOR:
      val = stats.history.inventory[STUFF_LABOR].at(0).amount;
      diff = val - stats.history.inventory[STUFF_LABOR].at(1).amount;
      cap = stats.history.inventory[STUFF_LABOR].at(0).capacity;
      break;
    case Bar::GOODS:
      val = stats.history.inventory[STUFF_GOODS].at(0).amount;
      diff = val - stats.history.inventory[STUFF_GOODS].at(1).amount;
      cap = stats.history.inventory[STUFF_GOODS].at(0).capacity;
      break;
    case Bar::COAL:
      val = stats.history.inventory[STUFF_COAL].at(0).amount;
      diff = val - stats.history.inventory[STUFF_COAL].at(1).amount;
      cap = stats.history.inventory[STUFF_COAL].at(0).capacity;
      break;
    case Bar::ORE:
      val = stats.history.inventory[STUFF_ORE].at(0).amount;
      diff = val - stats.history.inventory[STUFF_ORE].at(1).amount;
      cap = stats.history.inventory[STUFF_ORE].at(0).capacity;
      break;
    case Bar::STEEL:
      val = stats.history.inventory[STUFF_STEEL].at(0).amount;
      diff = val - stats.history.inventory[STUFF_STEEL].at(1).amount;
      cap = stats.history.inventory[STUFF_STEEL].at(0).capacity;
      break;
    case Bar::POL:
      val = stats.history.pollution.at(0);
      diff = val - stats.history.pollution.at(1);
      cap = 0;
      break;
    case Bar::LOVOLT:
      val = stats.history.inventory[STUFF_LOVOLT].at(0).amount;
      diff = val - stats.history.inventory[STUFF_LOVOLT].at(1).amount;
      cap = stats.history.inventory[STUFF_LOVOLT].at(0).capacity;
      break;
    case Bar::HIVOLT:
      val = stats.history.inventory[STUFF_HIVOLT].at(0).amount;
      diff = val - stats.history.inventory[STUFF_HIVOLT].at(1).amount;
      cap = stats.history.inventory[STUFF_HIVOLT].at(0).capacity;
      break;
    case Bar::WATER:
      val = stats.history.inventory[STUFF_WATER].at(0).amount;
      diff = val - stats.history.inventory[STUFF_WATER].at(1).amount;
      cap = stats.history.inventory[STUFF_WATER].at(0).capacity;
      break;
    case Bar::WASTE:
      val = stats.history.inventory[STUFF_WASTE].at(0).amount;
      diff = val - stats.history.inventory[STUFF_WASTE].at(1).amount;
      cap = stats.history.inventory[STUFF_WASTE].at(0).capacity;
      break;
    case Bar::HOUSE:
      val = stats.history.tenants.at(0).amount;
      diff = val - stats.history.tenants.at(11).amount;
      cap = stats.history.tenants.at(0).capacity;
      break;
    default:
      assert(false);
    }

    float diffNorm;
    switch(bar)
    {
    case Bar::POP:
      diffNorm = diff / std::max(std::sqrt((float)val), 1.0f);
      break;
    case Bar::TECH:
      diffNorm = diff / (diff >= 0 ? 4.0f : 2.0f);
      break;
    case Bar::POL:
      diffNorm = diff / std::max(std::sqrt((float)val), 1.0f);
      break;
    case Bar::MONEY:
      diffNorm = diff / (diff >= 0 ? 100000.0f : 50000.0f);
      break;
    default:
      diffNorm = diff * 100.f / std::max(cap, 1);
      break;
    };
    if(diffNorm > 1.0f)
      diffNorm = 1.0f;
    if(diffNorm < -1.0f)
      diffNorm = -1.0f;

    std::ostringstream valStr;
    valStr << std::fixed << std::setprecision(1);
    switch(bar) {
    case Bar::TECH:
      valStr << val / 10000.0f;
      break;

    case Bar::MONEY:
    case Bar::POP:
    case Bar::POL:
      valStr << num_to_ansi(val);
      break;

    case Bar::FOOD:
    case Bar::LABOR:
    case Bar::GOODS:
    case Bar::COAL:
    case Bar::ORE:
    case Bar::STEEL:
    case Bar::LOVOLT:
    case Bar::HIVOLT:
    case Bar::WATER:
    case Bar::WASTE:
    case Bar::HOUSE:
      valStr << ((float)val / std::max(cap, 1) * 100) << "%";
      break;

    default:
      assert(false);
    }

    Paragraph *valPar = getParagraph(*this,
      "pbar_text" + std::to_string(b + 1));
    BarView *barview1 = dynamic_cast<BarView *>(findComponent(
      "pbar_barview" + std::to_string(b + 1) + "a"));
    BarView *barview2 = dynamic_cast<BarView *>(findComponent(
      "pbar_barview" + std::to_string(b + 1) + "b"));

    valPar->setText(valStr.str());
    barview1->setValue(diffNorm);
    barview2->setValue(diffNorm);
  }
}

void
LCPBar::setGame(Game *game) {
  this->game = game;
}

//////////////////////////////////////////////////////////////////////////////
// BarView
//////////////////////////////////////////////////////////////////////////////

BarView::BarView()
{
}

BarView::~BarView()
{
}

void
BarView::parse(xmlpp::TextReader& reader) {
  dir=true;
  bad=false;

  while(reader.move_to_next_attribute()) {
    xmlpp::ustring name = reader.get_name();
    xmlpp::ustring value = reader.get_value();
    if(parseAttribute(reader));
    else if(name == "width")
      width = xmlParse<float>(value);
    else if(name == "height")
      height = xmlParse<float>(value);
    else if(name == "dir") {
      dir = xmlParse<int>(value) >= 0;
    }
    else if(name == "bad")
      bad = xmlParse<bool>(value);
    else
      unexpectedXmlAttribute(reader);
  }
  reader.move_to_element();

  if(width <= 0 || height <= 0)
      throw std::runtime_error("Width or Height invalid");
  value=0.7;
}

void BarView::setValue(float v)
{
    if(v>=-1.0 && v<=1.0)
        value=v;
}

void BarView::draw(Painter &painter)
{
    if(((int)(width*value)>0 && dir))
    {
        painter.setFillColor(bad?Color(0xFF,0,0,255):Color(0,0xAA,0,255));
        painter.fillRectangle(Rect2D(0,0,width*value,height));
    }
    else if(((int)(width*value)<0 && !dir))
    {
        painter.setFillColor(bad?Color(0,0xAA,0,255):Color(0xFF,0,0,255));
        painter.fillRectangle(Rect2D(width-1+width*value,0,width-1,height));
    }
}

IMPLEMENT_COMPONENT_FACTORY(LCPBar)
IMPLEMENT_COMPONENT_FACTORY(BarView)

/** @file lincity-ng/PBar.cpp */
