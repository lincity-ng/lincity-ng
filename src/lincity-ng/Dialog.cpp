/*
Copyright (C) 2005 Wolfgang Becker <uafr@gmx.de>

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

#include "Dialog.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "GameView.hpp"
#include "Util.hpp"
#include "MapEdit.hpp"
#include "CheckButton.hpp"
#include "lincity/engine.h"
#include "lincity/fileutil.h"
#include "lincity/simulate.h"
#include "lincity/lclib.h"
#include "lincity/loadsave.h"

#include "gui_interface/shared_globals.h"

#include "gui/ComponentLoader.hpp"
#include "gui/Button.hpp"
#include "gui/callback/Callback.hpp"
#include "gui/Paragraph.hpp"

#include "tinygettext/gettext.hpp"

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
    desktop = 0;
    myDialogComponent = 0;
    pointX = x;
    pointY = y;
    iAmBlocking = false;
    if( root ) {
        while( root->getParent() )
            root = root->getParent();
        desktop = dynamic_cast<Desktop*> (root);
        if(!desktop) 
            std::cerr << "Root not a desktop!?!\n";
    } else {
        std::cerr << "Dialog: Root not found.\n";
    }
}

Dialog::~Dialog(){
}

void Dialog::registerDialog(){
    dialogVector.push_back( this );
    desktop->addChildComponent( myDialogComponent );
}

void Dialog::unRegisterDialog(){
    std::vector<Dialog*>::iterator iter = dialogVector.begin(); 
    std::vector<Dialog*>::iterator del; 
    while( iter <= dialogVector.end() ){
        if ( *iter == this ){
            del = iter;
            iter++;
            dialogVector.erase( del );
        } else {
            iter++;
        }
    }
    delete( this );
}

void Dialog::askRocket(){
    if( !desktop ) {
        std::cerr << "No desktop found.\n";
        return;
    }
    try {
        myDialogComponent = loadGUIFile( "gui/launch_rocket_yn.xml" );
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
    yesButton->clicked.connect( makeCallback(*this, &Dialog::okayLaunchRocketButtonClicked ) );
    Button* noButton = getButton( *myDialogComponent, "No" );
    noButton->clicked.connect( makeCallback( *this, &Dialog::closeDialogButtonClicked ) );
    Button* gotoButton = getButton( *myDialogComponent, "goto" );
    gotoButton->clicked.connect( makeCallback( *this, &Dialog::gotoButtonClicked ) );
}

//no Signals caught here, so ScreenInterface has to catch them.
void Dialog::msgDialog( std::string message, std::string extraString){
    if( !desktop ) {
        std::cerr << "No desktop found.\n";
        return;
    }
    //generate filename. foo.mes => gui/foo.xml
    std::string filename = "gui/";
    filename += message;
    std::string::size_type pos = filename.rfind( ".mes" );
    if( pos != std::string::npos ){
        filename.replace( pos, 4 ,".xml");
    }
    std::auto_ptr<Component> myDialogComponent (loadGUIFile( filename ));

    //set Extra-String
    getParagraph( *myDialogComponent, "ExtraText" )->setText( extraString );

    // connect signals
    Button* noButton = getButton( *myDialogComponent, "Ok" );
    noButton->clicked.connect( makeCallback( *this, &Dialog::closeDialogButtonClicked ) );

    this->myDialogComponent = myDialogComponent.release();
    registerDialog();
}

void Dialog::askBulldozeMonument() {
    if( !desktop ) {
        std::cerr << "No desktop found.\n";
        return;
    }
    try {
        myDialogComponent = loadGUIFile( "gui/bulldoze_monument_yn.xml" );
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
    yesButton->clicked.connect( makeCallback(*this, &Dialog::okayBulldozeMonumentButtonClicked ) );
    Button* noButton = getButton( *myDialogComponent, "No" );
    noButton->clicked.connect( makeCallback( *this, &Dialog::closeDialogButtonClicked ) );
}

void Dialog::askBulldozeRiver() {
    if( !desktop ) {
        std::cerr << "No desktop found.\n";
        return;
    }
    try {
        myDialogComponent = loadGUIFile( "gui/bulldoze_river_yn.xml" );
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
    yesButton->clicked.connect( makeCallback(*this, &Dialog::okayBulldozeRiverButtonClicked ) );
    Button* noButton = getButton( *myDialogComponent, "No" );
    noButton->clicked.connect( makeCallback( *this, &Dialog::closeDialogButtonClicked ) );
}

void Dialog::askBulldozeShanty() {
    if( !desktop ) {
        std::cerr << "No desktop found.\n";
        return;
    }
    try {
        myDialogComponent = loadGUIFile( "gui/bulldoze_shanty_yn.xml" );
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
    yesButton->clicked.connect( makeCallback(*this, &Dialog::okayBulldozeShantyButtonClicked ) );
    Button* noButton = getButton( *myDialogComponent, "No" );
    noButton->clicked.connect( makeCallback( *this, &Dialog::closeDialogButtonClicked ) );
}

void Dialog::coalSurvey(){
    if( !desktop ) {
        std::cerr << "No desktop found.\n";
        return;
    }
    try {
        myDialogComponent = loadGUIFile( "gui/coal_survey_yn.xml" );
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
    yesButton->clicked.connect( makeCallback(*this, &Dialog::okayCoalSurveyButtonClicked ) );
    Button* noButton = getButton( *myDialogComponent, "No" );
    noButton->clicked.connect( makeCallback( *this, &Dialog::closeDialogButtonClicked ) );
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
 * and 12 lines in a Table with 6 columns
 * statistic_text_1_1  statistic_number_1_1  statistic_text_1_2  statistic_number_1_2  statistic_text_1_3  statistic_number_1_3
 * [...]
 * statistic_text_12_1 statistic_number_12_1 statistic_text_12_2 statistic_number_12_2 statistic_text_12_3 statistic_number_12_3
 */
void Dialog::gameStats(){
     saveGameStats();
     if( !desktop ) {
        std::cerr << "No desktop found.\n";
        return;
    }
    bool useExisting = false;
    myDialogComponent = desktop->findComponent("GameStats");
    if( myDialogComponent == 0){
        try {
            myDialogComponent = loadGUIFile( "gui/gamestats.xml" );
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
    int group_count[NUM_OF_GROUPS];
    count_all_groups (group_count);
    if (cheat_flag){
        setParagraphN( "statistic_text", line++, _("----- IN TEST MODE -------") );
    }
    snprintf (outf, maxlength, _("Game statistics from LinCity-NG Version %s"), PACKAGE_VERSION);
    setParagraphN( "statistic_text", line++, outf );
    if (strlen (given_scene) > 3){
	    snprintf (outf, maxlength, _("Initial loaded scene - %s"), given_scene);
        setParagraphN( "statistic_text", line++, outf );
    }
    if (sustain_flag){
	    snprintf (outf, maxlength, _("Economy is sustainable"));
        setParagraphN( "statistic_text", line++, outf );
    }
    snprintf (outf, maxlength, _("Population  %d  of which  %d  are not housed.")
	     ,housed_population + people_pool, people_pool);
    setParagraphN( "statistic_text", line++, outf );
    snprintf (outf, maxlength,
	     _("Max population %d  Number evacuated %d Total births %d")
	     ,max_pop_ever, total_evacuated, total_births);
    setParagraphN( "statistic_text", line++, outf );
    snprintf (outf, maxlength,
	     _("Date %s %04d  Money %8d   Tech-level %5.1f (%5.1f)"),
	     current_month(total_time), current_year(total_time), total_money,
	     (float) tech_level * 100.0 / MAX_TECH_LEVEL,
	     (float) highest_tech_level * 100.0 / MAX_TECH_LEVEL);
    setParagraphN( "statistic_text", line++, outf );
    snprintf (outf, maxlength,
	     _("Deaths by starvation %7d   History %8.3f"),
	     total_starve_deaths, starve_deaths_history);
    setParagraphN( "statistic_text", line++, outf );
    snprintf (outf, maxlength,
	     _("Deaths from pollution %7d   History %8.3f"),
	     total_pollution_deaths, pollution_deaths_history);
    setParagraphN( "statistic_text", line++, outf );
    snprintf (outf, maxlength, _("Years of unemployment %7d   History %8.3f"),
	     total_unemployed_years, unemployed_history);
    setParagraphN( "statistic_text", line++, outf );
    snprintf (outf, maxlength, _("Rockets launched %2d  Successful launches %2d"),
	     rockets_launched, rockets_launched_success);
    setParagraphN( "statistic_text", line++, outf );

    while( line <= 11 ){ //clear remaining lines
        setParagraphN( "statistic_text", line++, "" );
    }
    free( outf );

    setTableRC("statistic", 1, 1, _("Residences"),
	     group_count[GROUP_RESIDENCE_LL] + 
	     group_count[GROUP_RESIDENCE_ML] + 
	     group_count[GROUP_RESIDENCE_HL] + 
	     group_count[GROUP_RESIDENCE_LH] + 
	     group_count[GROUP_RESIDENCE_MH] + 
	     group_count[GROUP_RESIDENCE_HH]);
    setTableRC("statistic", 1, 2, _("Markets"), group_count[GROUP_MARKET] );
    setTableRC("statistic", 1, 3, _("Farms"), group_count[GROUP_ORGANIC_FARM]);
    
    setTableRC("statistic", 2, 1, _("Water wells"), group_count[GROUP_WATERWELL]);
    setTableRC("statistic", 2, 2, "", "");
    setTableRC("statistic", 2, 3, "", "");

    setTableRC("statistic", 3, 1, "", "");
    setTableRC("statistic", 3, 2, "", "");
    setTableRC("statistic", 3, 3, "", "");

    setTableRC("statistic", 4, 1, _("Monuments"), group_count[GROUP_MONUMENT]); 
    setTableRC("statistic", 4, 2, _("Schools"),  group_count[GROUP_SCHOOL]);
    setTableRC("statistic", 4, 3, _("Universities"), group_count[GROUP_UNIVERSITY]);

    setTableRC("statistic", 5, 1, _("Fire stations"), group_count[GROUP_FIRESTATION]);
    setTableRC("statistic", 5, 2, _("Parks"), group_count[GROUP_PARKLAND]);
    setTableRC("statistic", 5, 3, _("Sports fields"), group_count[GROUP_CRICKET]);

    setTableRC("statistic", 6, 1, _("Health centres"), group_count[GROUP_HEALTH]);
    setTableRC("statistic", 6, 2, _("Tips"), group_count[GROUP_TIP]);
    setTableRC("statistic", 6, 3, _("Shanties"), group_count[GROUP_SHANTY]);

    setTableRC("statistic", 7, 1, "", "");
    setTableRC("statistic", 7, 2, "", "");
    setTableRC("statistic", 7, 3, "", "");

    setTableRC("statistic", 8, 1, _("Windmills"), group_count[GROUP_WINDMILL]);
    setTableRC("statistic", 8, 2, _("Coal powers"), group_count[GROUP_COAL_POWER]);
    setTableRC("statistic", 8, 3, _("Solar powers"), group_count[GROUP_SOLAR_POWER]);

    setTableRC("statistic", 9, 1, _("Substations"), group_count[GROUP_SUBSTATION]);
    setTableRC("statistic", 9, 2, _("Power lines"), group_count[GROUP_POWER_LINE]);
    setTableRC("statistic", 9, 3, _("Ports"), group_count[GROUP_PORT]);

    setTableRC("statistic", 10, 1, _("Tracks"), group_count[GROUP_TRACK]);
    setTableRC("statistic", 10, 2, _("Roads"), group_count[GROUP_ROAD]);
    setTableRC("statistic", 10, 3, _("Rail"), group_count[GROUP_RAIL]);

    setTableRC("statistic", 11, 1, "", "");
    setTableRC("statistic", 11, 2, "", "");
    setTableRC("statistic", 11, 3, "", "");

    setTableRC("statistic", 12, 1, _("Potteries"), group_count[GROUP_POTTERY]);
    setTableRC("statistic", 12, 2, _("Blacksmiths"), group_count[GROUP_BLACKSMITH]);
    setTableRC("statistic", 12, 3, _("Mills"), group_count[GROUP_MILL]);

    setTableRC("statistic", 13, 1, _("Light inds"), group_count[GROUP_INDUSTRY_L]);
    setTableRC("statistic", 13, 2, _("Heavy inds"), group_count[GROUP_INDUSTRY_H]);
    setTableRC("statistic", 13, 3, _("Recyclers"), group_count[GROUP_RECYCLE]);

    setTableRC("statistic", 14, 1, _("Coal mines"), group_count[GROUP_COALMINE]);
    setTableRC("statistic", 14, 2, _("Ore mines"), group_count[GROUP_OREMINE]);
    setTableRC("statistic", 14, 3, _("Communes"), group_count[GROUP_COMMUNE]);

    setTableRC("statistic", 15, 1, "", "");
    setTableRC("statistic", 15, 2, "", "");
    setTableRC("statistic", 15, 3, "", "");
   
    if( !useExisting ){
        // connect signals
        Button* noButton = getButton( *myDialogComponent, "Okay" );
        noButton->clicked.connect( makeCallback( *this, &Dialog::closeDialogButtonClicked ) );
    }
}

/* 
 * Save game statistics to RESULTS_FILENAME
 */
void Dialog::saveGameStats(){
    //open File
    char *s;
    if ((s = (char *) malloc (lc_save_dir_len + strlen (LC_SAVE_DIR)
			      + strlen (RESULTS_FILENAME) + 64)) == 0)
	malloc_failure ();
    sprintf (s, "%s%c%s", lc_save_dir, PATH_SLASH, RESULTS_FILENAME);

    std::ofstream results( s );
    free( s );
    
    // Fill in Fields.
    int maxlength = 567;
    char* outf = (char *) malloc ( maxlength );
    int group_count[NUM_OF_GROUPS];
    count_all_groups (group_count);
    if (cheat_flag){
        results << "----- IN TEST MODE -------"  << std::endl;
    }
    snprintf (outf, maxlength, "Game statistics from LinCity-NG Version %s", PACKAGE_VERSION);
    results << outf << std::endl;
    if (strlen (given_scene) > 3){
	    snprintf (outf, maxlength, "Initial loaded scene - %s", given_scene);
        results << outf << std::endl;
    }
    if (sustain_flag){
	    snprintf (outf, maxlength, "Economy is sustainable");
        results << outf << std::endl;
    }
    snprintf (outf, maxlength, "Population  %d  of which  %d  are not housed."
	     ,housed_population + people_pool, people_pool);
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

    snprintf (outf, maxlength, "    Residences %4d         Markets %4d            Farms %4d",
	     group_count[GROUP_RESIDENCE_LL] + 
	     group_count[GROUP_RESIDENCE_ML] + 
	     group_count[GROUP_RESIDENCE_HL] + 
	     group_count[GROUP_RESIDENCE_LH] + 
	     group_count[GROUP_RESIDENCE_MH] + 
	     group_count[GROUP_RESIDENCE_HH],
	     group_count[GROUP_MARKET],
	     group_count[GROUP_ORGANIC_FARM]);
    results << outf << std::endl;
    snprintf (outf, maxlength, "   Water wells %4d", group_count[GROUP_WATERWELL]);
    results << outf << std::endl;
    snprintf (outf, maxlength, "     Monuments %4d         Schools %4d     Universities %4d"
	     ,group_count[GROUP_MONUMENT], group_count[GROUP_SCHOOL]
	     ,group_count[GROUP_UNIVERSITY]);
    results << outf << std::endl;
    snprintf (outf, maxlength, " Fire stations %4d           Parks %4d    Sports fields %4d"
	     ,group_count[GROUP_FIRESTATION], group_count[GROUP_PARKLAND]
	     ,group_count[GROUP_CRICKET]);
    results << outf << std::endl;
    snprintf (outf, maxlength, "Health centres %4d            Tips %4d         Shanties %4d",
	     group_count[GROUP_HEALTH], group_count[GROUP_TIP],
	     group_count[GROUP_SHANTY]);
    results << outf << std::endl;
    results << "" << std::endl;

    snprintf (outf, maxlength, "     Windmills %4d     Coal powers %4d     Solar powers %4d",
	     group_count[GROUP_WINDMILL],
	     group_count[GROUP_COAL_POWER],
	     group_count[GROUP_SOLAR_POWER]);
    results << outf << std::endl;
    snprintf (outf, maxlength, "   Substations %4d     Power lines %4d            Ports %4d"
	     ,group_count[GROUP_SUBSTATION], group_count[GROUP_POWER_LINE]
	     ,group_count[GROUP_PORT]);
    results << outf << std::endl;
    snprintf (outf, maxlength, "        Tracks %4d           Roads %4d             Rail %4d"
	     ,group_count[GROUP_TRACK], group_count[GROUP_ROAD]
	     ,group_count[GROUP_RAIL]);
    results << outf << std::endl;
    results << "" << std::endl;

    snprintf (outf, maxlength, "     Potteries %4d     Blacksmiths %4d            Mills %4d"
	     ,group_count[GROUP_POTTERY], group_count[GROUP_BLACKSMITH]
	     ,group_count[GROUP_MILL]);
    results << outf << std::endl;
    snprintf (outf, maxlength, "    Light inds %4d      Heavy inds %4d        Recyclers %4d"
	     ,group_count[GROUP_INDUSTRY_L], group_count[GROUP_INDUSTRY_H]
	     ,group_count[GROUP_RECYCLE]);
    results << outf << std::endl;
    snprintf (outf, maxlength, "    Coal mines %4d       Ore mines %4d         Communes %4d"
	     ,group_count[GROUP_COALMINE], group_count[GROUP_OREMINE]
	     ,group_count[GROUP_COMMUNE]);
    results << outf << std::endl;
    results << "" << std::endl;
    
    //close File
    results.close();
        
    free( outf );
}

void Dialog::editMarket(){
    if( !desktop ) {
        std::cerr << "No desktop found.\n";
        return;
    }
    try {
        myDialogComponent = loadGUIFile( "gui/tradedialog.xml" );
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

    CheckButton* cb;
    cb = getCheckButton( *myDialogComponent, "BuyJobs" );
    if( MP_INFO( pointX,pointY ).flags & FLAG_MB_JOBS ) cb->check(); else cb->uncheck();
    cb = getCheckButton( *myDialogComponent, "SellJobs" );
    if( MP_INFO( pointX,pointY ).flags & FLAG_MS_JOBS ) cb->check(); else cb->uncheck();
    cb = getCheckButton( *myDialogComponent, "BuyFood" );
    if( MP_INFO( pointX,pointY ).flags & FLAG_MB_FOOD) cb->check(); else cb->uncheck();
    cb = getCheckButton( *myDialogComponent, "SellFood" );
    if( MP_INFO( pointX,pointY ).flags & FLAG_MS_FOOD) cb->check(); else cb->uncheck();
    cb = getCheckButton( *myDialogComponent, "BuyCoal" );
    if( MP_INFO( pointX,pointY ).flags & FLAG_MB_COAL) cb->check(); else cb->uncheck();
    cb = getCheckButton( *myDialogComponent, "SellCoal" );
    if( MP_INFO( pointX,pointY ).flags & FLAG_MS_COAL) cb->check(); else cb->uncheck();
    cb = getCheckButton( *myDialogComponent, "BuyOre" );
    if( MP_INFO( pointX,pointY ).flags & FLAG_MB_ORE) cb->check(); else cb->uncheck();
    cb = getCheckButton( *myDialogComponent, "SellOre" );
    if( MP_INFO( pointX,pointY ).flags & FLAG_MS_ORE) cb->check(); else cb->uncheck();
    cb = getCheckButton( *myDialogComponent, "BuyGoods" );
    if( MP_INFO( pointX,pointY ).flags & FLAG_MB_GOODS) cb->check(); else cb->uncheck();
    cb = getCheckButton( *myDialogComponent, "SellGoods" );
    if( MP_INFO( pointX,pointY ).flags & FLAG_MS_GOODS) cb->check(); else cb->uncheck();
    cb = getCheckButton( *myDialogComponent, "BuySteel" );
    if( MP_INFO( pointX,pointY ).flags & FLAG_MB_STEEL) cb->check(); else cb->uncheck();
    cb = getCheckButton( *myDialogComponent, "SellSteel" );
    if( MP_INFO( pointX,pointY ).flags & FLAG_MS_STEEL) cb->check(); else cb->uncheck();
    // connect signals
    Button* applyButton = getButton( *myDialogComponent, "Apply" );
    applyButton->clicked.connect( makeCallback(*this, &Dialog::applyMarketButtonClicked ) );
    Button* gotoButton = getButton( *myDialogComponent, "goto" );
    gotoButton->clicked.connect( makeCallback( *this, &Dialog::gotoButtonClicked ) );
}

void Dialog::editPort(){
    if( !desktop ) {
        std::cerr << "No desktop found.\n";
        return;
    }
    try {
        myDialogComponent = loadGUIFile( "gui/portdialog.xml" );
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
    Paragraph* p = getParagraph( *myDialogComponent, "DialogTitle" );
    std::stringstream title;
	title << _("Port") << " ( " << pointX <<" , " << pointY << " )";
    p->setText( title.str() );

    CheckButton* cb;
    cb = getCheckButton( *myDialogComponent, "BuyFood" );
    if( MP_INFO( pointX,pointY ).flags & FLAG_MB_FOOD) cb->check(); else cb->uncheck();
    cb = getCheckButton( *myDialogComponent, "SellFood" );
    if( MP_INFO( pointX,pointY ).flags & FLAG_MS_FOOD) cb->check(); else cb->uncheck();
    cb = getCheckButton( *myDialogComponent, "BuyCoal" );
    if( MP_INFO( pointX,pointY ).flags & FLAG_MB_COAL) cb->check(); else cb->uncheck();
    cb = getCheckButton( *myDialogComponent, "SellCoal" );
    if( MP_INFO( pointX,pointY ).flags & FLAG_MS_COAL) cb->check(); else cb->uncheck();
    cb = getCheckButton( *myDialogComponent, "BuyOre" );
    if( MP_INFO( pointX,pointY ).flags & FLAG_MB_ORE) cb->check(); else cb->uncheck();
    cb = getCheckButton( *myDialogComponent, "SellOre" );
    if( MP_INFO( pointX,pointY ).flags & FLAG_MS_ORE) cb->check(); else cb->uncheck();
    cb = getCheckButton( *myDialogComponent, "BuyGoods" );
    if( MP_INFO( pointX,pointY ).flags & FLAG_MB_GOODS) cb->check(); else cb->uncheck();
    cb = getCheckButton( *myDialogComponent, "SellGoods" );
    if( MP_INFO( pointX,pointY ).flags & FLAG_MS_GOODS) cb->check(); else cb->uncheck();
    cb = getCheckButton( *myDialogComponent, "BuySteel" );
    if( MP_INFO( pointX,pointY ).flags & FLAG_MB_STEEL) cb->check(); else cb->uncheck();
    cb = getCheckButton( *myDialogComponent, "SellSteel" );
    if( MP_INFO( pointX,pointY ).flags & FLAG_MS_STEEL) cb->check(); else cb->uncheck();
    // connect signals
    Button* applyButton = getButton( *myDialogComponent, "Apply" );
    applyButton->clicked.connect( makeCallback(*this, &Dialog::applyPortButtonClicked ) );
    Button* gotoButton = getButton( *myDialogComponent, "goto" );
    gotoButton->clicked.connect( makeCallback( *this, &Dialog::gotoButtonClicked ) );
}

void Dialog::applyMarketButtonClicked( Button* b ){
    CheckButton* cb;
    cb = getCheckButton( *myDialogComponent, "BuyJobs" );
    if( cb->state == CheckButton::STATE_CHECKED ){  
        MP_INFO( pointX,pointY ).flags |= FLAG_MB_JOBS; 
    } else {
        MP_INFO( pointX,pointY ).flags &= ~FLAG_MB_JOBS; 
    }
    cb = getCheckButton( *myDialogComponent, "SellJobs" );
    if( cb->state == CheckButton::STATE_CHECKED ){  
        MP_INFO( pointX,pointY ).flags |= FLAG_MS_JOBS; 
    } else {
        MP_INFO( pointX,pointY ).flags &= ~FLAG_MS_JOBS; 
    }
    applyPortButtonClicked( b );
}
    
void Dialog::applyPortButtonClicked( Button* ){
    CheckButton* cb;
    cb = getCheckButton( *myDialogComponent, "BuyFood" );
    if( cb->state == CheckButton::STATE_CHECKED ){  
        MP_INFO( pointX,pointY ).flags |= FLAG_MB_FOOD; 
    } else {
        MP_INFO( pointX,pointY ).flags &= ~FLAG_MB_FOOD; 
    }
    cb = getCheckButton( *myDialogComponent, "SellFood" );
    if( cb->state == CheckButton::STATE_CHECKED ){  
        MP_INFO( pointX,pointY ).flags |= FLAG_MS_FOOD; 
    } else {
        MP_INFO( pointX,pointY ).flags &= ~FLAG_MS_FOOD; 
    }
    cb = getCheckButton( *myDialogComponent, "BuyCoal" );
    if( cb->state == CheckButton::STATE_CHECKED ){  
        MP_INFO( pointX,pointY ).flags |= FLAG_MB_COAL; 
    } else {
        MP_INFO( pointX,pointY ).flags &= ~FLAG_MB_COAL; 
    }
    cb = getCheckButton( *myDialogComponent, "SellCoal" );
    if( cb->state == CheckButton::STATE_CHECKED ){  
        MP_INFO( pointX,pointY ).flags |= FLAG_MS_COAL; 
    } else {
        MP_INFO( pointX,pointY ).flags &= ~FLAG_MS_COAL; 
    }
    cb = getCheckButton( *myDialogComponent, "BuyOre" );
    if( cb->state == CheckButton::STATE_CHECKED ){  
        MP_INFO( pointX,pointY ).flags |= FLAG_MB_ORE; 
    } else {
        MP_INFO( pointX,pointY ).flags &= ~FLAG_MB_ORE; 
    }
    cb = getCheckButton( *myDialogComponent, "SellOre" );
    if( cb->state == CheckButton::STATE_CHECKED ){  
        MP_INFO( pointX,pointY ).flags |= FLAG_MS_ORE; 
    } else {
        MP_INFO( pointX,pointY ).flags &= ~FLAG_MS_ORE; 
    }
    cb = getCheckButton( *myDialogComponent, "BuyGoods" );
    if( cb->state == CheckButton::STATE_CHECKED ){  
        MP_INFO( pointX,pointY ).flags |= FLAG_MB_GOODS; 
    } else {
        MP_INFO( pointX,pointY ).flags &= ~FLAG_MB_GOODS; 
    }
    cb = getCheckButton( *myDialogComponent, "SellGoods" );
    if( cb->state == CheckButton::STATE_CHECKED ){  
        MP_INFO( pointX,pointY ).flags |= FLAG_MS_GOODS; 
    } else {
        MP_INFO( pointX,pointY ).flags &= ~FLAG_MS_GOODS; 
    }
    cb = getCheckButton( *myDialogComponent, "BuySteel" );
    if( cb->state == CheckButton::STATE_CHECKED ){  
        MP_INFO( pointX,pointY ).flags |= FLAG_MB_STEEL; 
    } else {
        MP_INFO( pointX,pointY ).flags &= ~FLAG_MB_STEEL; 
    }
    cb = getCheckButton( *myDialogComponent, "SellSteel" );
    if( cb->state == CheckButton::STATE_CHECKED ){  
        MP_INFO( pointX,pointY ).flags |= FLAG_MS_STEEL; 
    } else {
        MP_INFO( pointX,pointY ).flags &= ~FLAG_MS_STEEL; 
    }
    desktop->remove( myDialogComponent );
    blockingDialogIsOpen = false;
    unRegisterDialog();
}

void Dialog::okayLaunchRocketButtonClicked( Button* ){
    launch_rocket( pointX, pointY );
    desktop->remove( myDialogComponent );
    blockingDialogIsOpen = false;
    unRegisterDialog();
}


void Dialog::okayCoalSurveyButtonClicked( Button* ){
    do_coal_survey();    
    desktop->remove( myDialogComponent );
    blockingDialogIsOpen = false;
    unRegisterDialog();
}

void Dialog::okayBulldozeRiverButtonClicked( Button* ){
    river_bul_flag = 1;
    desktop->remove( myDialogComponent );
    check_bulldoze_area( pointX, pointY );
    blockingDialogIsOpen = false;
    unRegisterDialog();
}

void Dialog::okayBulldozeShantyButtonClicked( Button* ){
    shanty_bul_flag = 1;
    desktop->remove( myDialogComponent );
    check_bulldoze_area( pointX, pointY );
    blockingDialogIsOpen = false;
    unRegisterDialog();
}

void Dialog::okayBulldozeMonumentButtonClicked( Button* ){
    monument_bul_flag = 1;
    desktop->remove( myDialogComponent );
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
    desktop->remove( myDialogComponent );
    if( iAmBlocking ){
        blockingDialogIsOpen = false;
    }
    unRegisterDialog();
}
