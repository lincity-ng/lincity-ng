/*
Copyright (C) 2005 Wolfgang Becker <uafr@gmx.de>
Copyright (C) 2024 David Bears <dbear4q@gmail.com>

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

#include "Dialog.hpp"

#include <assert.h>                        // for assert
#include <config.h>                        // for PACKAGE_VERSION
#include <stdio.h>                         // for snprintf
#include <stdlib.h>                        // for free, malloc
#include <string.h>                        // for strlen
#include <array>                           // for array
#include <exception>                       // for exception
#include <filesystem>                      // for path, operator/
#include <fstream>                         // for basic_ostream, operator<<
#include <functional>                      // for bind, _1, function
#include <iostream>                        // for cerr
#include <memory>                          // for unique_ptr
#include <sstream>                         // for basic_stringstream
#include <stdexcept>                       // for runtime_error
#include <vector>                          // for vector

#include "Config.hpp"                      // for getConfig, Config
#include "GameView.hpp"                    // for getGameView, GameView
#include "MapEdit.hpp"                     // for check_bulldoze_area, monum...
#include "MapPoint.hpp"                    // for MapPoint
#include "Util.hpp"                        // for getCheckButton, getButton
#include "gui/Button.hpp"                  // for Button
#include "gui/CheckButton.hpp"             // for CheckButton
#include "gui/Component.hpp"               // for Component
#include "gui/ComponentLoader.hpp"         // for loadGUIFile
#include "gui/Paragraph.hpp"               // for Paragraph
#include "gui/Signal.hpp"                  // for Signal
#include "gui/SwitchComponent.hpp"         // for SwitchComponent
#include "gui/Window.hpp"                  // for Window
#include "gui/WindowManager.hpp"           // for WindowManager
#include "gui_interface/mps.h"             // for mps_refresh
#include "gui_interface/shared_globals.h"  // for cheat_flag
#include "lincity/commodities.hpp"         // for CommodityRule, Commodity
#include "lincity/engglobs.h"              // for world, given_scene, people...
#include "lincity/engine.h"                // for do_coal_survey
#include "lincity/lclib.h"                 // for current_year, current_month
#include "lincity/lin-city.h"              // for MAX_TECH_LEVEL, RESULTS_FI...
#include "lincity/lintypes.h"              // for Construction, NUMOF_DAYS_I...
#include "lincity/modules/all_modules.h"   // for Market, ResidenceConstruct...
#include "lincity/world.h"                 // for MapTile, World
#include "tinygettext/gettext.hpp"         // for _

using namespace std::placeholders;

bool blockingDialogIsOpen = false;

std::vector<Dialog*> dialogVector;

void closeAllDialogs(){
    std::vector<Dialog*>::iterator iter;
    while( !dialogVector.empty() ){
        iter = dialogVector.begin();
        (*iter)->closeDialog();
    }
}

Dialog::Dialog( int type ){
    initDialog();
    switch( type ){
        case ASK_COAL_SURVEY:
            coalSurvey();
            break;
        case GAME_STATS:
            gameStats();
            break;
        default:
            std::stringstream msg;
            msg <<"Can't open Dialog type " << type << " without additional parameters.";
            throw std::runtime_error(msg.str());
    }
}

Dialog::Dialog( int type, std::string message, std::string extraString){
    initDialog();
    switch( type ){
        case MSG_DIALOG:
            msgDialog( message, extraString );
            break;
        default:
            std::stringstream msg;
            msg <<"Can't open Dialog type " << type << " with String parameters.";
            throw std::runtime_error(msg.str());
    }
}

Dialog::Dialog( int type, int x, int y ){
    initDialog( x, y );
    switch( type ) {
        case BULLDOZE_MONUMENT:
            askBulldozeMonument();
            break;
        case BULLDOZE_RIVER:
            askBulldozeRiver();
            break;
        case BULLDOZE_SHANTY:
            askBulldozeShanty();
            break;
        case EDIT_MARKET:
            editMarket();
            break;
        case EDIT_PORT:
            editPort();
            break;
        case ASK_LAUNCH_ROCKET:
            askRocket();
            break;
        default:
            std::stringstream msg;
            msg <<"Can't open Dialog type " << type << " with coordinates.";
            throw std::runtime_error(msg.str());
    }
}

void Dialog::initDialog( int x /*= -1*/, int y /*= -1*/ ){
    Component* root = getGameView();
    windowManager = 0;
    myDialogComponent = 0;
    pointX = x;
    pointY = y;
    iAmBlocking = false;
    if( root ) {
        while( root->getParent() )
            root = root->getParent();
        windowManager = dynamic_cast<WindowManager*>(
          root->findComponent("windowManager"));
        if(!windowManager)
            std::cerr << "Root not a window manager!?!\n";
    } else {
        std::cerr << "Dialog: Root not found.\n";
    }
}

Dialog::~Dialog(){
}

void Dialog::registerDialog(){
    dialogVector.push_back( this );
    windowManager->addWindow(static_cast<Window *>(myDialogComponent));
}

void Dialog::unRegisterDialog(){
    std::vector<Dialog*>::iterator iter = dialogVector.begin();
    while( iter != dialogVector.end() ){
        if ( *iter == this ){
            iter = dialogVector.erase( iter );
        } else {
            iter++;
        }
    }

    delete( this );
}

void Dialog::askRocket(){
    if( !windowManager ) {
        std::cerr << "No window manager found.\n";
        return;
    }
    try {
        myDialogComponent = dynamic_cast<Window *>(
          loadGUIFile( "gui/dialogs/launch_rocket_yn.xml" ));
        assert( myDialogComponent != 0);
        registerDialog();
        blockingDialogIsOpen = true;
        iAmBlocking = true;
    } catch(std::exception& e) {
        std::cerr << "Couldn't display message 'launch_rocket_yn': "
            << e.what() << "\n";
        return;
    }
    Paragraph* p = getParagraph( *myDialogComponent, "DialogTitle" );
    std::stringstream title;
    title << _("Launchsite") << " ( " << pointX <<" , " << pointY << " )";
    p->setText( title.str() );
    // connect signals
    Button* yesButton = getButton( *myDialogComponent, "Yes" );
    yesButton->clicked.connect(
      std::bind(&Dialog::okayLaunchRocketButtonClicked, this, _1));
    Button* noButton = getButton( *myDialogComponent, "No" );
    noButton->clicked.connect(
      std::bind(&Dialog::closeDialogButtonClicked, this, _1));
    Button* gotoButton = getButton( *myDialogComponent, "goto" );
    gotoButton->clicked.connect(
      std::bind(&Dialog::gotoButtonClicked, this, _1));
}

//no Signals caught here, so ScreenInterface has to catch them.
void Dialog::msgDialog( std::string message, std::string extraString){
    if( !windowManager ) {
        std::cerr << "No window manager found.\n";
        return;
    }
    //generate filename. foo.mes => gui/dialogs/foo.xml
    std::string filename = "gui/dialogs/";
    filename += message;
    std::string::size_type pos = filename.rfind( ".mes" );
    if( pos != std::string::npos ){
        filename.replace( pos, 4 ,".xml");
    }
    std::unique_ptr<Window> myDialogComponent (dynamic_cast<Window *>(
      loadGUIFile( filename )));

    //set Extra-String
    getParagraph( *myDialogComponent, "ExtraText" )->setText( extraString );

    // connect signals
    Button* noButton = getButton( *myDialogComponent, "Ok" );
    noButton->clicked.connect(
      std::bind(&Dialog::closeDialogButtonClicked, this, _1));

    this->myDialogComponent = myDialogComponent.release();
    registerDialog();
}

void Dialog::askBulldozeMonument() {
    if( !windowManager ) {
        std::cerr << "No window manager found.\n";
        return;
    }
    try {
        myDialogComponent = dynamic_cast<Window *>(
          loadGUIFile( "gui/dialogs/bulldoze_monument_yn.xml" ));
        assert( myDialogComponent != 0);
        registerDialog();
        blockingDialogIsOpen = true;
        iAmBlocking = true;
    } catch(std::exception& e) {
        std::cerr << "Couldn't display message 'bulldoze_monument_yn': "
            << e.what() << "\n";
        return;
    }
    // connect signals
    Button* yesButton = getButton( *myDialogComponent, "Yes" );
    yesButton->clicked.connect(
      std::bind(&Dialog::okayBulldozeMonumentButtonClicked, this, _1));
    Button* noButton = getButton( *myDialogComponent, "No" );
    noButton->clicked.connect(
      std::bind(&Dialog::closeDialogButtonClicked, this, _1));
}

void Dialog::askBulldozeRiver() {
    if( !windowManager ) {
        std::cerr << "No window manager found.\n";
        return;
    }
    try {
        myDialogComponent = dynamic_cast<Window *>(
          loadGUIFile( "gui/dialogs/bulldoze_river_yn.xml" ));
        assert( myDialogComponent != 0);
        registerDialog();
        blockingDialogIsOpen = true;
        iAmBlocking = true;
    } catch(std::exception& e) {
        std::cerr << "Couldn't display message 'bulldoze_river_yn.xml': "
            << e.what() << "\n";
        return;
    }
    // connect signals
    Button* yesButton = getButton( *myDialogComponent, "Yes" );
    yesButton->clicked.connect(
      std::bind(&Dialog::okayBulldozeRiverButtonClicked, this, _1));
    Button* noButton = getButton( *myDialogComponent, "No" );
    noButton->clicked.connect(
      std::bind(&Dialog::closeDialogButtonClicked, this, _1));
}

void Dialog::askBulldozeShanty() {
    if( !windowManager ) {
        std::cerr << "No window manager found.\n";
        return;
    }
    try {
        myDialogComponent = dynamic_cast<Window *>(
          loadGUIFile( "gui/dialogs/bulldoze_shanty_yn.xml" ));
        assert( myDialogComponent != 0);
        registerDialog();
        blockingDialogIsOpen = true;
        iAmBlocking = true;
    } catch(std::exception& e) {
        std::cerr << "Couldn't display message 'bulldoze_shanty_yn': "
            << e.what() << "\n";
        return;
    }
    // connect signals
    Button* yesButton = getButton( *myDialogComponent, "Yes" );
    yesButton->clicked.connect(
      std::bind(&Dialog::okayBulldozeShantyButtonClicked, this, _1));
    Button* noButton = getButton( *myDialogComponent, "No" );
    noButton->clicked.connect(
      std::bind(&Dialog::closeDialogButtonClicked, this, _1));
}

void Dialog::coalSurvey(){
    if( !windowManager ) {
        std::cerr << "No window manager found.\n";
        return;
    }
    try {
        myDialogComponent = dynamic_cast<Window *>(
          loadGUIFile( "gui/dialogs/coal_survey_yn.xml" ));
        assert( myDialogComponent != 0);
        registerDialog();
        blockingDialogIsOpen = true;
        iAmBlocking = true;
    } catch(std::exception& e) {
        std::cerr << "Couldn't display message 'coal_survey_yn': "
            << e.what() << "\n";
        return;
    }
    // connect signals
    Button* yesButton = getButton( *myDialogComponent, "Yes" );
    yesButton->clicked.connect(
      std::bind(&Dialog::okayCoalSurveyButtonClicked, this, _1));
    Button* noButton = getButton( *myDialogComponent, "No" );
    noButton->clicked.connect(
      std::bind(&Dialog::closeDialogButtonClicked, this, _1));
}

void Dialog::setParagraphN( const std::string basename, const int number, const std::string text ){
    std::stringstream paragraphName;
    paragraphName << basename << number;
    setParagraph( paragraphName.str(), text );
}

template<typename T> void Dialog::setTableRC( const std::string basename, const int row, const int column, const std::string text, T value ){
    std::stringstream paragraphName;
    paragraphName << basename << "_text_" << row << "_" << column;
    setParagraph( paragraphName.str(), text );
    paragraphName.str("");
    paragraphName << basename << "_value_" << row << "_" << column;
    std::stringstream stringvalue;
    stringvalue << value;
    setParagraph( paragraphName.str(), stringvalue.str() );
}

void Dialog::setParagraph( const std::string paragraphName, const std::string text ){
    Paragraph* p;
    try{
        p = getParagraph( *myDialogComponent, paragraphName );
        p->setText( text );
    } catch(std::exception& e) {
        std::cerr << "Couldn't set " << paragraphName << " to '" << text << "': "
            << e.what() << "\n";
    }
}

/*
 * Show game statistics in dialog and write them to RESULTS_FILENAME.
 *
 * gamestats.xml has 11 lines
 * statistic_text1
 * [...]
 * statistic_text11
 * and 15 lines in a Table with 6 columns
 * statistic_text_1_1  statistic_number_1_1  statistic_text_1_2  statistic_number_1_2  statistic_text_1_3  statistic_number_1_3
 * [...]
//  * statistic_text_15_1 statistic_number_15_1 statistic_text_15_2 statistic_number_15_2 statistic_text_15_3 statistic_number_15_3
 */
void Dialog::gameStats(){
     saveGameStats();
     if( !windowManager ) {
        std::cerr << "No window manager found.\n";
        return;
    }
    bool useExisting = false;
    myDialogComponent = dynamic_cast<Window *>(
      windowManager->findComponent("GameStats"));
    if( myDialogComponent == 0){
        try {
            myDialogComponent = dynamic_cast<Window *>(
              loadGUIFile( "gui/dialogs/gamestats.xml" ));
            assert( myDialogComponent != 0);
            registerDialog();
        } catch(std::exception& e) {
            std::cerr << "Couldn't display message 'gamestats': "
                << e.what() << "\n";
            return;
        }
    } else {
        useExisting = true;
    }

    // Fill in Fields.
    int line = 1;
    int maxlength = 567;
    char* outf = (char *) malloc ( maxlength );
    if (cheat_flag){
        setParagraphN( "statistic_text", line++, _("----- IN TEST MODE -------") );
    }
    snprintf (outf, maxlength,"%s %s", _("Game statistics from LinCity-NG Version"), PACKAGE_VERSION);
    setParagraphN( "statistic_text", line++, outf );
    if (strlen (given_scene) > 3){
        snprintf (outf, maxlength,"%s - %s", _("Initial loaded scene"), given_scene);
        setParagraphN( "statistic_text", line++, outf );
    }
    if (sustain_flag){
        snprintf (outf, maxlength, "%s", _("Economy is sustainable"));
        setParagraphN( "statistic_text", line++, outf );
    }
    snprintf (outf, maxlength, "%s %d %s %d %s.",
        _("Population"), population + people_pool, _("of which"),  people_pool, _("are not housed"));
    setParagraphN( "statistic_text", line++, outf );
    snprintf (outf, maxlength, "%s %d %s %d %s %d",
        _("Max population"), max_pop_ever,  _("Number evacuated"), total_evacuated, _("Total births"), total_births);
    setParagraphN( "statistic_text", line++, outf );
    snprintf (outf, maxlength, "%s %s %04d %s %8d %s %5.1f (%5.1f)",
        _("Date"),  current_month(total_time), current_year(total_time),
        _("Money"), total_money, _("Tech-level"),
        (float) tech_level * 100.0 / MAX_TECH_LEVEL,
        (float) highest_tech_level * 100.0 / MAX_TECH_LEVEL);
    setParagraphN( "statistic_text", line++, outf );
    snprintf (outf, maxlength, "%s %7d %s %8.3f",
        _("Deaths by starvation"), total_starve_deaths,
        _("History"), starve_deaths_history);
    setParagraphN( "statistic_text", line++, outf );
    snprintf (outf, maxlength, "%s %7d   %s %8.3f",
        _("Deaths from pollution"), total_pollution_deaths,
        _("History"), pollution_deaths_history);
    setParagraphN( "statistic_text", line++, outf );
    snprintf (outf, maxlength, "%s %7d   %s %8.3f",
        _("Years of unemployment"), total_unemployed_years,
        _("History"), unemployed_history);
    setParagraphN( "statistic_text", line++, outf );
    snprintf (outf, maxlength, "%s %2d  %s %2d",
        _("Rockets launched"), rockets_launched,
        _("Successful launches"), rockets_launched_success);
    setParagraphN( "statistic_text", line++, outf );

    while( line <= 11 ){ //clear remaining lines
        setParagraphN( "statistic_text", line++, "" );
    }
    free( outf );

    setTableRC("statistic", 1, 1, _("Residences"),
      residenceLLConstructionGroup.count +
      residenceMLConstructionGroup.count +
      residenceHLConstructionGroup.count +
      residenceLHConstructionGroup.count +
      residenceMHConstructionGroup.count +
      residenceHHConstructionGroup.count);
    setTableRC("statistic", 1, 2, _("Markets"), marketConstructionGroup.count);
    setTableRC("statistic", 1, 3, _("Farms"), organic_farmConstructionGroup.count);

    setTableRC("statistic", 2, 1, _("Water wells"), waterwellConstructionGroup.count);
    setTableRC("statistic", 2, 2, _("Windmills"), windmillConstructionGroup.count);
    setTableRC("statistic", 2, 3, "", "");

    setTableRC("statistic", 3, 1, "", "");
    setTableRC("statistic", 3, 2, "", "");
    setTableRC("statistic", 3, 3, "", "");

    setTableRC("statistic", 4, 1, _("Monuments"), monumentConstructionGroup.count);
    setTableRC("statistic", 4, 2, _("Schools"), schoolConstructionGroup.count);
    setTableRC("statistic", 4, 3, _("Universities"), universityConstructionGroup.count);

    setTableRC("statistic", 5, 1, _("Fire stations"), fireStationConstructionGroup.count);
    setTableRC("statistic", 5, 2, _("Parks"),
      parklandConstructionGroup.count +
      parkpondConstructionGroup.count);
    setTableRC("statistic", 5, 3, _("Sports fields"), cricketConstructionGroup.count);

    setTableRC("statistic", 6, 1, _("Health centres"), healthCentreConstructionGroup.count);
    setTableRC("statistic", 6, 2, _("Tips"), tipConstructionGroup.count);
    setTableRC("statistic", 6, 3, _("Shanties"), shantyConstructionGroup.count);

    setTableRC("statistic", 7, 1, "", "");
    setTableRC("statistic", 7, 2, "", "");
    setTableRC("statistic", 7, 3, "", "");

    setTableRC("statistic", 8, 1, _("Wind powers"), windpowerConstructionGroup.count);
    setTableRC("statistic", 8, 2, _("Coal powers"), coal_powerConstructionGroup.count);
    setTableRC("statistic", 8, 3, _("Solar powers"), solarPowerConstructionGroup.count);

    setTableRC("statistic", 9, 1, _("Substations"), substationConstructionGroup.count);
    setTableRC("statistic", 9, 2, _("Power lines"), powerlineConstructionGroup.count);
    setTableRC("statistic", 9, 3, _("Ports"), portConstructionGroup.count);

    setTableRC("statistic", 10, 1, _("Tracks"),
      trackConstructionGroup.count +
      trackbridgeConstructionGroup.count);
    setTableRC("statistic", 10, 2, _("Roads"),
      roadConstructionGroup.count +
      roadbridgeConstructionGroup.count);
    setTableRC("statistic", 10, 3, _("Rail"),
      railConstructionGroup.count +
      railbridgeConstructionGroup.count);

    setTableRC("statistic", 11, 1, "", "");
    setTableRC("statistic", 11, 2, "", "");
    setTableRC("statistic", 11, 3, "", "");

    setTableRC("statistic", 12, 1, _("Potteries"), potteryConstructionGroup.count);
    setTableRC("statistic", 12, 2, _("Blacksmiths"), blacksmithConstructionGroup.count);
    setTableRC("statistic", 12, 3, _("Mills"), millConstructionGroup.count);

    setTableRC("statistic", 13, 1, _("Light inds"), industryLightConstructionGroup.count);
    setTableRC("statistic", 13, 2, _("Heavy inds"), industryHeavyConstructionGroup.count);
    setTableRC("statistic", 13, 3, _("Recyclers"), recycleConstructionGroup.count);

    setTableRC("statistic", 14, 1, _("Coal mines"), coalmineConstructionGroup.count);
    setTableRC("statistic", 14, 2, _("Ore mines"), oremineConstructionGroup.count);
    setTableRC("statistic", 14, 3, _("Forests"), communeConstructionGroup.count);

    setTableRC("statistic", 15, 1, "", "");
    setTableRC("statistic", 15, 2, "", "");
    setTableRC("statistic", 15, 3, "", "");

    if( !useExisting ){
        // connect signals
        Button* noButton = getButton( *myDialogComponent, "Okay" );
        noButton->clicked.connect(
          std::bind(&Dialog::closeDialogButtonClicked, this, _1));
    }
}

/*
 * Save game statistics to RESULTS_FILENAME. Do NOT translate text here!
 * That way the files can be used for highscores.
 */
void Dialog::saveGameStats(){
    std::ofstream results(getConfig()->userDataDir / RESULTS_FILENAME);

    // Fill in Fields.
    int maxlength = 567;
    char* outf = (char *) malloc ( maxlength );
    //int group_count[NUM_OF_GROUPS];
    //count_all_groups (group_count);
    if (cheat_flag){
        results << "----- IN TEST MODE -------"  << std::endl;
    }
    snprintf (outf, maxlength, "Game statistics from LinCity-NG Version %s", PACKAGE_VERSION);
    results << outf << std::endl;
    if (strlen (given_scene) > 3){
        #pragma GCC diagnostic push
        #pragma GCC diagnostic ignored "-Wformat-truncation"
        snprintf (outf, maxlength, "Initial loaded scene - %s", given_scene);
        #pragma GCC diagnostic pop
        results << outf << std::endl;
    }
    if (sustain_flag){
        snprintf (outf, maxlength, "%s", "Economy is sustainable");
        results << outf << std::endl;
    }
    snprintf (outf, maxlength, "Population  %d  of which  %d  are not housed."
         ,population + people_pool, people_pool);
    results << outf << std::endl;
    snprintf (outf, maxlength,
         "Max population %d  Number evacuated %d Total births %d"
         ,max_pop_ever, total_evacuated, total_births);
    results << outf << std::endl;
    snprintf (outf, maxlength,
         "Date %02d/%04d  Money %8d   Tech-level %5.1f (%5.1f)",
         1 + ((total_time % NUMOF_DAYS_IN_YEAR) / NUMOF_DAYS_IN_MONTH), current_year(total_time), total_money,
         (float) tech_level * 100.0 / MAX_TECH_LEVEL,
         (float) highest_tech_level * 100.0 / MAX_TECH_LEVEL);
    results << outf << std::endl;
    snprintf (outf, maxlength,
         " Deaths by starvation %7d   History %8.3f",
         total_starve_deaths, starve_deaths_history);
    results << outf << std::endl;
    snprintf (outf, maxlength,
         "Deaths from pollution %7d   History %8.3f",
         total_pollution_deaths, pollution_deaths_history);
    results << outf << std::endl;
    snprintf (outf, maxlength, "Years of unemployment %7d   History %8.3f",
         total_unemployed_years, unemployed_history);
    results << outf << std::endl;
    snprintf (outf, maxlength, "Rockets launched %2d  Successful launches %2d",
         rockets_launched, rockets_launched_success);
    results << outf << std::endl;
    results << "" << std::endl;

    snprintf(outf, maxlength, "    Residences %4d         Markets %4d            Farms %4d",
      residenceLLConstructionGroup.count
        + residenceMLConstructionGroup.count
        + residenceHLConstructionGroup.count
        + residenceLHConstructionGroup.count
        + residenceMHConstructionGroup.count
        + residenceHHConstructionGroup.count,
      marketConstructionGroup.count,
      organic_farmConstructionGroup.count);
    results << outf << std::endl;
    snprintf(outf, maxlength, "   Water wells %4d     Wind powers %4d",
      waterwellConstructionGroup.count,
      windpowerConstructionGroup.count);
    results << outf << std::endl;
    snprintf(outf, maxlength, "     Monuments %4d         Schools %4d     Universities %4d",
      monumentConstructionGroup.count,
      schoolConstructionGroup.count,
      universityConstructionGroup.count);
    results << outf << std::endl;
    snprintf(outf, maxlength, " Fire stations %4d           Parks %4d    Sports fields %4d",
      fireStationConstructionGroup.count,
      parklandConstructionGroup.count + parkpondConstructionGroup.count,
      cricketConstructionGroup.count);
    results << outf << std::endl;
    snprintf(outf, maxlength, "Health centres %4d            Tips %4d         Shanties %4d",
      healthCentreConstructionGroup.count,
      tipConstructionGroup.count,
      shantyConstructionGroup.count);
    results << outf << std::endl;
    results << "" << std::endl;

    snprintf(outf, maxlength, "     Windmills %4d     Coal powers %4d     Solar powers %4d",
      windmillConstructionGroup.count,
      coal_powerConstructionGroup.count,
      solarPowerConstructionGroup.count);
    results << outf << std::endl;
    snprintf(outf, maxlength, "   Substations %4d     Power lines %4d            Ports %4d",
      substationConstructionGroup.count,
      powerlineConstructionGroup.count,
      portConstructionGroup.count);
    results << outf << std::endl;
    snprintf(outf, maxlength, "        Tracks %4d           Roads %4d             Rail %4d",
      trackConstructionGroup.count + trackbridgeConstructionGroup.count,
      roadConstructionGroup.count + roadbridgeConstructionGroup.count,
      railConstructionGroup.count + railbridgeConstructionGroup.count);
    results << outf << std::endl;
    results << "" << std::endl;

    snprintf(outf, maxlength, "     Potteries %4d     Blacksmiths %4d            Mills %4d",
      potteryConstructionGroup.count,
      blacksmithConstructionGroup.count,
      millConstructionGroup.count);
    results << outf << std::endl;
    snprintf(outf, maxlength, "    Light inds %4d      Heavy inds %4d        Recyclers %4d",
      industryLightConstructionGroup.count,
      industryHeavyConstructionGroup.count,
      recycleConstructionGroup.count);
    results << outf << std::endl;
    snprintf(outf, maxlength, "    Coal mines %4d       Ore mines %4d         Communes %4d",
      coalmineConstructionGroup.count,
      oremineConstructionGroup.count,
      communeConstructionGroup.count);
    results << outf << std::endl;
    results << "" << std::endl;

    free( outf );
}

void Dialog::editMarket(){
    if( !windowManager ) {
        std::cerr << "No window manager found.\n";
        return;
    }
    try {
        myDialogComponent = dynamic_cast<Window *>(
          loadGUIFile( "gui/dialogs/tradedialog.xml" ));
        assert( myDialogComponent != 0);
        registerDialog();
        blockingDialogIsOpen = true;
        iAmBlocking = true;
    } catch(std::exception& e) {
        std::cerr << "Couldn't display dialog 'tradedialog.xml': "
            << e.what() << "\n";
        return;
    }
    // set Dialog to Market-Data
    Paragraph* p = getParagraph( *myDialogComponent, "DialogTitle" );
    std::stringstream title;
    title << _("Market") << " ( " << pointX <<" , " << pointY << " )";
    p->setText( title.str() );
    Market * market = static_cast <Market *> (world(pointX, pointY)->reportingConstruction);
    CheckButton* cb;
    cb = getCheckButton( *myDialogComponent, "BuyLabor" );
    if( market->commodityRuleCount[STUFF_LABOR].take ) cb->check(); else cb->uncheck();
    cb = getCheckButton( *myDialogComponent, "BuyLabor" );
    if( market->commodityRuleCount[STUFF_LABOR].give ) cb->check(); else cb->uncheck();
    cb = getCheckButton( *myDialogComponent, "BuyFood" );
    if( market->commodityRuleCount[STUFF_FOOD].take ) cb->check(); else cb->uncheck();
    cb = getCheckButton( *myDialogComponent, "SellFood" );
    if( market->commodityRuleCount[STUFF_FOOD].give ) cb->check(); else cb->uncheck();
    cb = getCheckButton( *myDialogComponent, "BuyCoal" );
    if( market->commodityRuleCount[STUFF_COAL].take ) cb->check(); else cb->uncheck();
    cb = getCheckButton( *myDialogComponent, "SellCoal" );
    if( market->commodityRuleCount[STUFF_COAL].give ) cb->check(); else cb->uncheck();
    cb = getCheckButton( *myDialogComponent, "BuyOre" );
    if( market->commodityRuleCount[STUFF_ORE].take ) cb->check(); else cb->uncheck();
    cb = getCheckButton( *myDialogComponent, "SellOre" );
    if( market->commodityRuleCount[STUFF_ORE].give ) cb->check(); else cb->uncheck();
    cb = getCheckButton( *myDialogComponent, "BuyGoods" );
    if( market->commodityRuleCount[STUFF_GOODS].take ) cb->check(); else cb->uncheck();
    cb = getCheckButton( *myDialogComponent, "SellGoods" );
    if( market->commodityRuleCount[STUFF_GOODS].give ) cb->check(); else cb->uncheck();
    cb = getCheckButton( *myDialogComponent, "BuySteel" );
    if( market->commodityRuleCount[STUFF_STEEL].take ) cb->check(); else cb->uncheck();
    cb = getCheckButton( *myDialogComponent, "SellSteel" );
    if( market->commodityRuleCount[STUFF_STEEL].give ) cb->check(); else cb->uncheck();
    cb = getCheckButton( *myDialogComponent, "BuyWaste" );
    if( market->commodityRuleCount[STUFF_WASTE].take ) cb->check(); else cb->uncheck();
    cb = getCheckButton( *myDialogComponent, "SellWaste" );
    if( market->commodityRuleCount[STUFF_WASTE].give ) cb->check(); else cb->uncheck();
    cb = getCheckButton( *myDialogComponent, "BuyWater" );
    if( market->commodityRuleCount[STUFF_WATER].take ) cb->check(); else cb->uncheck();
    cb = getCheckButton( *myDialogComponent, "SellWater" );
    if( market->commodityRuleCount[STUFF_WATER].give ) cb->check(); else cb->uncheck();
    // connect signals
    Button* applyButton = getButton( *myDialogComponent, "Apply" );
    applyButton->clicked.connect(
      std::bind(&Dialog::applyMarketButtonClicked, this, _1));
    Button* gotoButton = getButton( *myDialogComponent, "goto" );
    gotoButton->clicked.connect(
      std::bind(&Dialog::gotoButtonClicked, this, _1));
}

void Dialog::editPort(){
    if( !windowManager ) {
        std::cerr << "No window manager found.\n";
        return;
    }
    try {
        myDialogComponent = dynamic_cast<Window *>(
          loadGUIFile( "gui/dialogs/portdialog.xml" ));
        assert( myDialogComponent != 0);
        registerDialog();
        blockingDialogIsOpen = true;
        iAmBlocking = true;
    } catch(std::exception& e) {
        std::cerr << "Couldn't display dialog 'tradedialog.xml': "
            << e.what() << "\n";
        return;
    }
    // set Dialog to Port-Data
    //int port_flags = world(pointX, pointY)->reportingConstruction->flags;
    Port *port = dynamic_cast<Port *>(world(pointX, pointY)->reportingConstruction);
    Paragraph* p = getParagraph( *myDialogComponent, "DialogTitle" );
    std::stringstream title;
    title << _("Port") << " ( " << pointX <<" , " << pointY << " )";
    p->setText( title.str() );

    for(Commodity c = STUFF_INIT; c != STUFF_COUNT; c++) {
      if(!port->commodityRuleCount[c].maxload) continue;

      for(int isGive = 0; isGive < 2; isGive++) {
        SwitchComponent *sc = dynamic_cast<SwitchComponent *>(
          myDialogComponent->findComponent(
            std::string(isGive?"Sell":"Buy")+commodityNames[c]));
        if(!sc) {
          std::cerr << "error: could not get "
            << (std::string(isGive?"Sell":"Buy")+commodityNames[c])
            << " SwitchComponent" << std::endl;
          continue;
        }
        const CommodityRule &rule = portConstructionGroup.tradeRule[c];
        sc->switchComponent((isGive ? rule.give : rule.take) ? "green" : "red");
        CheckButton *cb = dynamic_cast<CheckButton *>(sc->getActiveComponent());
        if(!cb) {
          std::cerr << "error: could not get "
            << ((isGive ? rule.give : rule.take) ? "green" : "red")
            << " CheckButton for "
            << (std::string(isGive?"Sell":"Buy")+commodityNames[c])
            << std::endl;
          continue;
        }

        CommodityRule& crc = port->commodityRuleCount[c];
        if(isGive ? crc.give : crc.take) cb->check(); else cb->uncheck();
      }
    }

    // connect signals
    Button* applyButton = getButton( *myDialogComponent, "Apply" );
    applyButton->clicked.connect(
      std::bind(&Dialog::applyPortButtonClicked, this, _1));
    Button* gotoButton = getButton( *myDialogComponent, "goto" );
    gotoButton->clicked.connect(
      std::bind(&Dialog::gotoButtonClicked, this, _1));
}

void Dialog::applyMarketButtonClicked( Button* ){
    CheckButton* cb;
    Market * market = static_cast <Market *> (world(pointX, pointY)->construction);
    cb = getCheckButton( *myDialogComponent, "BuyLabor" );
    if(cb->isChecked()) {
        market->commodityRuleCount[STUFF_LABOR].take = true;
    } else {
        market->commodityRuleCount[STUFF_LABOR].take = false;
    }
    cb = getCheckButton( *myDialogComponent, "BuyLabor" );
    if(cb->isChecked()) {
        market->commodityRuleCount[STUFF_LABOR].give = true;
    } else {
        market->commodityRuleCount[STUFF_LABOR].give = false;
    }
    cb = getCheckButton( *myDialogComponent, "BuyFood" );
    if(cb->isChecked()) {
        market->commodityRuleCount[STUFF_FOOD].take = true;
    } else {
        market->commodityRuleCount[STUFF_FOOD].take = false;
    }
    cb = getCheckButton( *myDialogComponent, "SellFood" );
    if(cb->isChecked()) {
        market->commodityRuleCount[STUFF_FOOD].give = true;
    } else {
        market->commodityRuleCount[STUFF_FOOD].give = false;
    }
    cb = getCheckButton( *myDialogComponent, "BuyCoal" );
    if(cb->isChecked()) {
        market->commodityRuleCount[STUFF_COAL].take = true;
    } else {
        market->commodityRuleCount[STUFF_COAL].take = false;
    }
    cb = getCheckButton( *myDialogComponent, "SellCoal" );
    if(cb->isChecked()) {
        market->commodityRuleCount[STUFF_COAL].give = true;
    } else {
        market->commodityRuleCount[STUFF_COAL].give = false;
    }
    cb = getCheckButton( *myDialogComponent, "BuyOre" );
    if(cb->isChecked()) {
        market->commodityRuleCount[STUFF_ORE].take = true;
    } else {
        market->commodityRuleCount[STUFF_ORE].take = false;
    }
    cb = getCheckButton( *myDialogComponent, "SellOre" );
    if(cb->isChecked()) {
        market->commodityRuleCount[STUFF_ORE].give = true;
    } else {
        market->commodityRuleCount[STUFF_ORE].give = false;
    }
    cb = getCheckButton( *myDialogComponent, "BuyGoods" );
    if(cb->isChecked()) {
        market->commodityRuleCount[STUFF_GOODS].take = true;
    } else {
        market->commodityRuleCount[STUFF_GOODS].take = false;
    }
    cb = getCheckButton( *myDialogComponent, "SellGoods" );
    if(cb->isChecked()) {
        market->commodityRuleCount[STUFF_GOODS].give = true;
    } else {
        market->commodityRuleCount[STUFF_GOODS].give = false;
    }
    cb = getCheckButton( *myDialogComponent, "BuySteel" );
    if(cb->isChecked()) {
        market->commodityRuleCount[STUFF_STEEL].take = true;
    } else {
        market->commodityRuleCount[STUFF_STEEL].take = false;
    }
    cb = getCheckButton( *myDialogComponent, "SellSteel" );
    if(cb->isChecked()) {
        market->commodityRuleCount[STUFF_STEEL].give = true;
    } else {
        market->commodityRuleCount[STUFF_STEEL].give = false;
    }
    cb = getCheckButton( *myDialogComponent, "BuyWaste" );
    if(cb->isChecked()) {
        market->commodityRuleCount[STUFF_WASTE].take = true;
    } else {
        market->commodityRuleCount[STUFF_WASTE].take = false;
    }
    cb = getCheckButton( *myDialogComponent, "SellWaste" );
    if(cb->isChecked()) {
        market->commodityRuleCount[STUFF_WASTE].give = true;
    } else {
        market->commodityRuleCount[STUFF_WASTE].give = false;
    }
    cb = getCheckButton( *myDialogComponent, "BuyWater" );
    if(cb->isChecked()) {
        market->commodityRuleCount[STUFF_WATER].take = true;
    } else {
        market->commodityRuleCount[STUFF_WATER].take = false;
    }
    cb = getCheckButton( *myDialogComponent, "SellWater" );
    if(cb->isChecked()) {
        market->commodityRuleCount[STUFF_WATER].give = true;
    } else {
        market->commodityRuleCount[STUFF_WATER].give = false;
    }
    mps_refresh();
    windowManager->removeWindow( myDialogComponent );
    blockingDialogIsOpen = false;
    unRegisterDialog();
}

void Dialog::applyPortButtonClicked( Button* ){
    Port *port = dynamic_cast<Port *>(world(pointX, pointY)->reportingConstruction);

    for(Commodity c = STUFF_INIT; c != STUFF_COUNT; c++) {
      if(!port->commodityRuleCount[c].maxload) continue;

      for(int isGive = 0; isGive < 2; isGive++) {
        SwitchComponent *sc = dynamic_cast<SwitchComponent *>(
          myDialogComponent->findComponent(
            std::string(isGive?"Sell":"Buy")+commodityNames[c]));
        if(!sc) {
          std::cerr << "error: could not find "
            << (std::string(isGive?"Sell":"Buy")+commodityNames[c])
            << " SwitchComponent" << std::endl;
          continue;
        }
        CheckButton *cb = dynamic_cast<CheckButton *>(sc->getActiveComponent());
        if(!cb) {
          std::cerr << "error: could not find CheckButton for "
            << (std::string(isGive?"Sell":"Buy")+commodityNames[c])
            << std::endl;
          continue;
        }

        CommodityRule& crc = port->commodityRuleCount[c];
        (isGive ? crc.give : crc.take) = cb->isChecked();
      }

      if(port->commodityRuleCount[c].take && port->commodityRuleCount[c].give) {
        port->commodityRuleCount[c].take = false;
        port->commodityRuleCount[c].give = false;
      }
    }

    windowManager->removeWindow( myDialogComponent );
    blockingDialogIsOpen = false;
    unRegisterDialog();
}

void Dialog::okayLaunchRocketButtonClicked( Button* )
{
    static_cast<RocketPad*> (world(pointX, pointY)->reportingConstruction)-> launch_rocket();
    windowManager->removeWindow( myDialogComponent );
    blockingDialogIsOpen = false;
    unRegisterDialog();
}


void Dialog::okayCoalSurveyButtonClicked( Button* ){
    do_coal_survey();
    windowManager->removeWindow( myDialogComponent );
    blockingDialogIsOpen = false;
    unRegisterDialog();
}

void Dialog::okayBulldozeRiverButtonClicked( Button* ){
    river_bul_flag = 1;
    windowManager->removeWindow( myDialogComponent );
    check_bulldoze_area( pointX, pointY );
    blockingDialogIsOpen = false;
    unRegisterDialog();
}

void Dialog::okayBulldozeShantyButtonClicked( Button* ){
    shanty_bul_flag = 1;
    windowManager->removeWindow( myDialogComponent );
    check_bulldoze_area( pointX, pointY );
    blockingDialogIsOpen = false;
    unRegisterDialog();
}

void Dialog::okayBulldozeMonumentButtonClicked( Button* ){
    monument_bul_flag = 1;
    windowManager->removeWindow( myDialogComponent );
    check_bulldoze_area( pointX, pointY );
    blockingDialogIsOpen = false;
    unRegisterDialog();
}

void Dialog::gotoButtonClicked( Button* ){
    getGameView()->show( MapPoint( pointX, pointY ) );
}

void Dialog::closeDialogButtonClicked( Button* ){
    closeDialog();
}

void Dialog::closeDialog(){
    windowManager->removeWindow( myDialogComponent );
    if( iAmBlocking ){
        blockingDialogIsOpen = false;
    }
    unRegisterDialog();
}

/** @file lincity-ng/Dialog.cpp */
