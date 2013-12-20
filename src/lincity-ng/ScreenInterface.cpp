/*
Copyright (C) 2005 David Kamphausen <david.kamphausen@web.de>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include <config.h>
#include <iostream>
#include <iomanip>
#include <stdarg.h>

#include <sys/stat.h>

#include "ScreenInterface.hpp"
#include "gui_interface/screen_interface.h"
#include "gui_interface/shared_globals.h"
#include "gui_interface/pbar_interface.h"
#include "gui_interface/mps.h"

#include "lincity/engglobs.h"
#include "lincity/lclib.h"
#include "lincity/fileutil.h"

#include "gui/Component.hpp"
#include "gui/ComponentLoader.hpp"
#include "gui/Paragraph.hpp"
#include "gui/Desktop.hpp"
#include "tinygettext/gettext.hpp"

#include "GameView.hpp"
#include "Util.hpp"
#include "Config.hpp"
#include "ButtonPanel.hpp"
#include "Dialog.hpp"
#include "EconomyGraph.hpp"

int selected_module_cost; // this must be changed, when module (or celltype-button) is changed


std::string lastDateText = "";
int lastMoney = -123456789;

/* This is on in screen_full_refresh, used in *_refresh() */

const char* current_month (int current_time);
void draw_cb_box (int row, int col, int checked);
/*
int ask_launch_rocket_now (int x, int y)
{
    new Dialog( ASK_LAUNCH_ROCKET, x, y );
    return 0;
}
*/
//void screen_full_refresh (void);
void initialize_monthgraph (void){
    int i;
    monthgraph_size = getConfig()->monthgraphW;

    monthgraph_pop = (int*) malloc (sizeof(int) * monthgraph_size);
    if (monthgraph_pop == 0) {
    malloc_failure ();
    }
    monthgraph_starve = (int*) malloc (sizeof(int) * monthgraph_size);
    if (monthgraph_starve == 0) {
    malloc_failure ();
    }
    monthgraph_nojobs = (int*) malloc (sizeof(int) * monthgraph_size);
    if (monthgraph_nojobs == 0) {
    malloc_failure ();
    }
    monthgraph_ppool = (int*) malloc (sizeof(int) * monthgraph_size);
    if (monthgraph_ppool == 0) {
    malloc_failure ();
    }
    for (i = 0; i < monthgraph_size; i++) {
    monthgraph_pop[i] = 0;
    monthgraph_starve[i] = 0;
    monthgraph_nojobs[i] = 0;
    monthgraph_ppool[i] = 0;
    }
}
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
 * Display some Text in a Dialog Box with an OK Button.
 *
 * see oldgui/screen.cpp: ok_dial_box (char *fn, int good_bad, char *xs)
 *
 * in oldgui good_bad influences the color:
 *                                GOOD, RESULTS in green
 *                                BAD in red
 *                                default white
 * in NG good_bad is reseted to void !!!
 *
 * fn is the FileName of the message
 *   if good_bad is RESULTS the fn is an absolute filename else
 *   it is a File in message_path.
 *
 * xs is some additional Text, that is shown after the Message
 *    from the File. ( XtraString )
 */
void ok_dial_box (const char *fn, int good_bad, const char *xs)
{
    (void) good_bad;
    try{
        new Dialog( MSG_DIALOG, std::string( fn ), std::string( xs ? xs : "" ) );
    } catch(std::exception& e) {
        std::cerr << "Problem with ok_dial_box: " << e.what() << "\n";
        std::ostringstream text;
        text << "Problem with ok_dial_box: '" << fn << "' + \"" << (xs ? xs : "") << "\"\n";
        updateMessageText( text.str() );
    }
}

/*
 * Update Message in Message Window
 */
void updateMessageText( const std::string text )
{
    //Dialog Test
    Component* root = getGameView();
    if(!root) {
        //happens while in menu.
        std::cerr << "Root not found.\n";
        return;
    }
    while( root->getParent() )
        root = root->getParent();
    Desktop* desktop = dynamic_cast<Desktop*> (root);
    if(!desktop) {
        std::cerr << "Root not a desktop!?!\n";
        return;
    }

    try {
        //test if message Windows is open
        Component* messageTextComponent = 0;
        messageTextComponent = root->findComponent( "messageText" );
        if(messageTextComponent == 0) {
            messageTextComponent = loadGUIFile("gui/messagearea.xml");
            assert(messageTextComponent != 0);
            desktop->addChildComponent(messageTextComponent);
        }
        Paragraph* messageText = getParagraph(*messageTextComponent, "messageText");

        messageText->setText( text );
    } catch(std::exception& e) {
        std::cerr << "Couldn't display message '" << text << "': "
            << e.what() << "\n";
        return;
    }
}

void updateDate()
{
    std::ostringstream dateText;

    int day = total_time % NUMOF_DAYS_IN_MONTH +1; // 1..NUMOF_DAYS_IN_MONTH
    day = 1 + ( 29 * day / NUMOF_DAYS_IN_MONTH ); // 1..30
    dateText << day << ". ";

    dateText << current_month( total_time );
    dateText << " "<< current_year( total_time );

    Component* root = getGameView();
    if( !root ) return;
    while( root->getParent() )
        root = root->getParent();

    if( dateText.str() == lastDateText ){
        return;
    }
    Component* dateParagraphComponent = 0;
    dateParagraphComponent = root->findComponent( "dateParagraph" );
    if( dateParagraphComponent == 0 )
    {   return;}
    Paragraph* dateParagraph = getParagraph( *root, "dateParagraph");
    if( !dateParagraph )
    {   return;}
    dateParagraph->setText( dateText.str() );
    lastDateText = dateText.str();
}

void updateMoney() {
    if( lastMoney == total_money ){
        return;
    }
    //Prevent overflow
    if (total_money > 2000000000)
    total_money = 2000000000;
    else if (total_money < -2000000000)
    total_money = -2000000000;

    std::ostringstream moneyText;
    int money = total_money;

    if(  abs(money) > 100000000 ){
       moneyText << money/1000000 << _("M");
    } else {
        if( abs(money) > 1000000 ){
            moneyText << money/1000000 << " ";
            money %= 1000000;
            money = abs(money);
            moneyText << std::setw(6);
            moneyText << std::setfill('0');
        }
        moneyText << money;
    }
    moneyText << _("$");

    Component* root = getGameView();
    if( !root ) return;
    while( root->getParent() )
        root = root->getParent();
    Component* moneyParagraphComponent = 0;
    moneyParagraphComponent = root->findComponent( "moneyParagraph" );
    if( moneyParagraphComponent == 0 ) {
        return;
    }
    Paragraph* moneyParagraph = getParagraph( *root, "moneyParagraph");
    if( !moneyParagraph ) return;

    moneyParagraph->setText( moneyText.str() );
    lastMoney = total_money;
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

void print_total_money (void)
{
    updateMoney();
}
/*
void refresh_population_text (void)
{
    update_pbar (PPOP, housed_population + people_pool, 0);
}
*/
void update_avail_modules (int popup)
{
    //tell ButtonPanel to check for tech change.
    ButtonPanel* bp = getButtonPanel();
    if( bp ){
        bp->checkTech( popup );
    }
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

void print_stats ()
{
    // this show update the financy window or mps
    if (total_time % NUMOF_DAYS_IN_MONTH == (NUMOF_DAYS_IN_MONTH - 1))
    {
        update_pbars_monthly();
        mps_refresh();
        getEconomyGraph()->updateData();
    }

    if (total_time % (NUMOF_DAYS_IN_MONTH/5) == NUMOF_DAYS_IN_MONTH / 5 - 1)
    {   mps_refresh();}

    //check for new tech
    update_avail_modules (1);

}

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

