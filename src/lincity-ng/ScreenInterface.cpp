/* ---------------------------------------------------------------------- *
 * src/lincity-ng/ScreenInterface.cpp
 * This file is part of Lincity-NG.
 *
 * Copyright (C) 2005      David Kamphausen <david.kamphausen@web.de>
 * Copyright (C) 2024-2025 David Bears <dbear4q@gmail.com>
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

#include <assert.h>                 // for assert
#include <stdio.h>                  // for snprintf
#include <cstdlib>                  // for abs
#include <iomanip>                  // for operator<<, setfill, setw
#include <iostream>                 // for basic_ostream, operator<<, basic_...
#include <memory>                   // for unique_ptr
#include <sstream>                  // for basic_ostringstream
#include <string>                   // for basic_string, char_traits, allocator

#include "Game.hpp"                 // for Game
#include "gui/Component.hpp"        // for Component
#include "gui/Paragraph.hpp"        // for Paragraph
#include "lincity/util.hpp"          // for current_month, current_year
#include "lincity/lintypes.hpp"       // for NUMOF_DAYS_IN_MONTH
#include "lincity/world.hpp"          // for World
#include "util/gettextutil.hpp"

void
Game::updateDate() const {
  char dateText[20];
  snprintf(dateText, 20, "%02d. %.3s %d",
    world->total_time % NUMOF_DAYS_IN_MONTH * 30 / NUMOF_DAYS_IN_MONTH + 1,
    current_month(world->total_time),
    current_year(world->total_time)
  );

  Paragraph *dateParagraph = dynamic_cast<Paragraph *>(
    gui->findComponent("dateParagraph"));
  assert(dateParagraph);
  if(!dateParagraph) {
    std::cerr << "error: could not find dateParagraph" << '\n';
    return;
  }

  dateParagraph->setText(dateText);
}

static void string_begadd_number(std::string &str, int number, bool fill) {
  std::ostringstream result;
  if (fill) result << std::setw(3) << std::setfill('0');
  result << number;
  str = result.str() + std::string(" ") + str;
};

void
Game::updateMoney() const {
  if(!world->isUpdated(World::Updatable::MONEY)) return;
  std::ostringstream moneyText;
  std::string        postfix ="";
  std::string        moneystr="";
  int money = world->total_money;

  /*   */if ( abs(money) > 100000000 ) {
    money/=1000000;
    postfix=_("M") + std::string(" ");
  } else if ( abs(money) > 100000    ) {
    money/=1000;
    postfix=_("k") + std::string(" ");
  }

  do {
    int tmpmoney;
    if (abs(money)<1000) {
      string_begadd_number(moneystr, money, false);
      money=0;
    } else {
      tmpmoney=abs(money)%1000;
      money/=1000;
      string_begadd_number(moneystr, tmpmoney, true);
    }
  } while (abs(money)>0);

  moneyText << moneystr << postfix << _("$");

  Paragraph *moneyPar = dynamic_cast<Paragraph *>(
    gui->findComponent("moneyParagraph"));
  assert(moneyPar);
  if(!moneyPar) return;

  moneyPar->setText(moneyText.str());

  world->clearUpdated(World::Updatable::MONEY);
}

/*
 *  A DialogBox with a Progressbar.
 *  see oldgui/screen.cpp: prog_box (char *title, int percent)
 *  is used to open a Dialog with given Title an a Progressbar,
 *  showing percent completed, but is also used to update
 *  the current Progressbar.
 */

void prog_box (const char *title, int percent)
{
    (void) title;
    (void) percent;
#ifdef DEBUG
    std::ostringstream text;
    text << "prog_box:'" << title << "' " << percent << "%\n";
    std::cout << text.str();
#endif
}

/*
void refresh_main_screen()
{
}
*/
/*
void screen_full_refresh ()
{
    updateDate();
    print_total_money();
    update_main_screen (true);
}
*/

/*
void update_main_screen_normal (int full_refresh)
{
    (void) full_refresh;
    return;
}*/
/*
void update_main_screen (int full_refresh)
{
    update_main_screen_normal (full_refresh);
}*/
/*
void print_time_for_year ()
{
}*/
/*
int getMainWindowWidth()
{
  //is only used by old code. 0 is the value that hurts least when the
  //load_city does some "corrections" to main_screen_originx
  return 0;
}
int getMainWindowHeight()
{
  return 0;
}
*/
/*
int yn_dial_box (const char * s1, const char * s2, const char * s3,
        const char *s4)
{
    std::cerr << "DIALOG MISSING:\n";
    std::cerr << "---[yn_dial_box]---------\n";
    std::cerr << s1 << "\n";
    std::cerr << s2 << "\n";
    std::cerr << s3 << "\n";
    std::cerr << s4 << "\n";
    std::cerr << "-------------------------\n";
    return 0;
}
*/
/** @file lincity-ng/ScreenInterface.cpp */
