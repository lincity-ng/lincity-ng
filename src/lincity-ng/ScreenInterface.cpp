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

#include "ScreenInterface.hpp"

#include <errno.h>                           // for errno
#include <stdio.h>                           // for snprintf
#include <cstdlib>                           // for abs, malloc
#include <exception>                         // for exception
#include <iomanip>                           // for operator<<, setfill, setw
#include <iostream>                          // for basic_ostream, operator<<
#include <sstream>                           // for basic_ostringstream
#include <string>                            // for basic_string, char_traits

#include "ButtonPanel.hpp"                   // for getButtonPanel, ButtonPanel
#include "Config.hpp"                        // for getConfig, Config
#include "Dialog.hpp"                        // for Dialog, MSG_DIALOG
#include "EconomyGraph.hpp"                  // for getEconomyGraph, Economy...
#include "GameView.hpp"                      // for getGameView, GameView
#include "Util.hpp"                          // for getParagraph
#include "gui/Component.hpp"                 // for Component
#include "gui/Paragraph.hpp"                 // for Paragraph
#include "gui_interface/mps.h"               // for mps_refresh
#include "gui_interface/screen_interface.h"  // for initialize_monthgraph
#include "lc_error.h"                        // for lc_error
#include "lincity/engglobs.h"                // for total_money, total_time
#include "lincity/lclib.h"                   // for current_year
#include "lincity/lintypes.h"                // for NUMOF_DAYS_IN_MONTH
#include "tinygettext/gettext.hpp"           // for _
#include "Game.hpp"

/* This is on in screen_full_refresh, used in *_refresh() */

/*
int ask_launch_rocket_now (int x, int y)
{
    new Dialog( ASK_LAUNCH_ROCKET, x, y );
    return 0;
}
*/
//void screen_full_refresh (void);
//void rotate_mini_screen (void);
//void advance_mps_style (void);
/*void update_main_screen (int full_refresh)
{
}*/
//void connect_transport_main_screen (void);

//void refresh_mps (void);
//void advance_monthgraph_style (void);
//void refresh_monthgraph (void);
//void draw_small_yellow_bezel (int x, int y, int h, int w);
//void mini_screen_help (void);
//void print_time_for_year (void);
//void rotate_main_screen (void);
//void screen_setup (void);


/* Miniscreen */
//void init_mini_map_mouse(void);
//void mini_map_handler(int x, int y, int button);
//void mini_aux_handler(int x, int y, int button);


/*
 * Update Message in Message Window
 */
// void updateMessageText( const std::string text )
// {
//     //Dialog Test
//     Component* root = getGameView();
//     if(!root) {
//         //happens while in menu.
//         std::cerr << "Root not found.\n";
//         return;
//     }
//     while( root->getParent() )
//         root = root->getParent();
//     Desktop* desktop = dynamic_cast<Desktop*> (root);
//     if(!desktop) {
//         std::cerr << "Root not a desktop!?!\n";
//         return;
//     }
//
//     try {
//         //test if message Windows is open
//         Component* messageTextComponent = 0;
//         messageTextComponent = root->findComponent( "messageText" );
//         if(messageTextComponent == 0) {
//             messageTextComponent = loadGUIFile("gui/messagearea.xml");
//             assert(messageTextComponent != 0);
//             desktop->addChildComponent(messageTextComponent);
//         }
//         Paragraph* messageText = getParagraph(*messageTextComponent, "messageText");
//
//         messageText->setText( text );
//     } catch(std::exception& e) {
//         std::cerr << "Couldn't display message '" << text << "': "
//             << e.what() << "\n";
//         return;
//     }
// }

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

void string_begadd_number(std::string &str, int number, bool fill) {
  std::ostringstream result;
  if (fill) result << std::setw(3) << std::setfill('0');
  result << number;
  str = result.str() + std::string(" ") + str;
};

void
Game::updateMoney() const {
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
