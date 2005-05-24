/**
 *  Dialogs for Lincity-NG.
 *   
 *  April 2005, Wolfgang Becker <uafr@gmx.de>
 *
 */
#include <config.h>

#include "Dialog.hpp"

#include <iostream>
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

#include "gui_interface/shared_globals.h"

#include "gui/ComponentLoader.hpp"
#include "gui/Button.hpp"
#include "gui/callback/Callback.hpp"
#include "gui/Paragraph.hpp"

#include "tinygettext/gettext.hpp"

bool blockingDialogIsOpen = false;

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
            msg <<"Can't open Dialog type " << type << " without assitional parameters.";
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

void Dialog::askRocket(){
    if( !desktop ) {
        std::cerr << "No desktop found.\n";
        return;
    }
    try {
        myDialogComponent = loadGUIFile( "gui/launch_rocket_yn.xml" );
        assert( myDialogComponent != 0);
        desktop->addChildComponent( myDialogComponent );
        blockingDialogIsOpen = true;
        iAmBlocking = true;
    } catch(std::exception& e) {
        std::cerr << "Couldn't display message 'launch_rocket_yn': "
            << e.what() << "\n";
        return;
    }
    Paragraph* p = getParagraph( *myDialogComponent, "rocketTitle" );
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
    uint pos = filename.rfind( ".mes" );
    if( pos != std::string::npos ){
        filename.replace( pos, 4 ,".xml");
    }
    myDialogComponent = loadGUIFile( filename );
    assert( myDialogComponent != 0);
    desktop->addChildComponent( myDialogComponent );

    //set Extra-String
    getParagraph( *myDialogComponent, "extraString" )->setText( extraString );

    // connect signals
    Button* noButton = getButton( *myDialogComponent, "Okay" );
    noButton->clicked.connect( makeCallback( *this, &Dialog::closeDialogButtonClicked ) );
}

void Dialog::askBulldozeMonument() {
    if( !desktop ) {
        std::cerr << "No desktop found.\n";
        return;
    }
    try {
        myDialogComponent = loadGUIFile( "gui/bulldoze_monument_yn.xml" );
        assert( myDialogComponent != 0);
        desktop->addChildComponent( myDialogComponent );
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
        desktop->addChildComponent( myDialogComponent );
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
        desktop->addChildComponent( myDialogComponent );
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
        desktop->addChildComponent( myDialogComponent );
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

void Dialog::setParagraphN( std::string basename, int number, std::string text ){
    Paragraph* p;
    std::stringstream paragraphName;
    paragraphName << basename << number;
    p = getParagraph( *myDialogComponent, paragraphName.str() );
    p->setText( text );
}

void Dialog::gameStats(){
     if( !desktop ) {
        std::cerr << "No desktop found.\n";
        return;
    }
    try {
        myDialogComponent = loadGUIFile( "gui/gamestats.xml" );
        assert( myDialogComponent != 0);
        desktop->addChildComponent( myDialogComponent );
    } catch(std::exception& e) {
        std::cerr << "Couldn't display message 'gamestats': "
            << e.what() << "\n";
        return;
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
	     _(" Date  %s %04d   Money %8d   Tech-level %5.1f (%5.1f)"),
	     current_month(total_time), current_year(total_time), total_money,
	     (float) tech_level * 100.0 / MAX_TECH_LEVEL,
	     (float) highest_tech_level * 100.0 / MAX_TECH_LEVEL);
    setParagraphN( "statistic_text", line++, outf );
    snprintf (outf, maxlength,
	     _(" Deaths by starvation %7d   History %8.3f"),
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
    setParagraphN( "statistic_text", line++, "" );
    snprintf (outf, maxlength, _("    Residences %4d         Markets %4d            Farms %4d"),
	     group_count[GROUP_RESIDENCE_LL] + 
	     group_count[GROUP_RESIDENCE_ML] + 
	     group_count[GROUP_RESIDENCE_HL] + 
	     group_count[GROUP_RESIDENCE_LH] + 
	     group_count[GROUP_RESIDENCE_MH] + 
	     group_count[GROUP_RESIDENCE_HH],
	     group_count[GROUP_MARKET],
	     group_count[GROUP_ORGANIC_FARM]);
    setParagraphN( "statistic_text", line++, outf );
    snprintf (outf, maxlength, _("        Tracks %4d           Roads %4d             Rail %4d")
	     ,group_count[GROUP_TRACK], group_count[GROUP_ROAD]
	     ,group_count[GROUP_RAIL]);
    setParagraphN( "statistic_text", line++, outf );
    snprintf (outf, maxlength, _("     Potteries %4d     Blacksmiths %4d            Mills %4d")
	     ,group_count[GROUP_POTTERY], group_count[GROUP_BLACKSMITH]
	     ,group_count[GROUP_MILL]);
    setParagraphN( "statistic_text", line++, outf );
    snprintf (outf, maxlength, _("     Monuments %4d         Schools %4d     Universities %4d")
	     ,group_count[GROUP_MONUMENT], group_count[GROUP_SCHOOL]
	     ,group_count[GROUP_UNIVERSITY]);
    setParagraphN( "statistic_text", line++, outf );
    snprintf (outf, maxlength, _(" Fire stations %4d           Parks %4d     Cricket gnds %4d")
	     ,group_count[GROUP_FIRESTATION], group_count[GROUP_PARKLAND]
	     ,group_count[GROUP_CRICKET]);
    setParagraphN( "statistic_text", line++, outf );
    snprintf (outf, maxlength, _("    Coal mines %4d       Ore mines %4d         Communes %4d")
	     ,group_count[GROUP_COALMINE], group_count[GROUP_OREMINE]
	     ,group_count[GROUP_COMMUNE]);
    setParagraphN( "statistic_text", line++, outf );
    snprintf (outf, maxlength, _("     Windmills %4d     Coal powers %4d     Solar powers %4d"),
	     group_count[GROUP_WINDMILL],
	     group_count[GROUP_COAL_POWER],
	     group_count[GROUP_SOLAR_POWER]);
    setParagraphN( "statistic_text", line++, outf );
    snprintf (outf, maxlength, _("   Substations %4d     Power lines %4d            Ports %4d")
	     ,group_count[GROUP_SUBSTATION], group_count[GROUP_POWER_LINE]
	     ,group_count[GROUP_PORT]);
    setParagraphN( "statistic_text", line++, outf );
    snprintf (outf, maxlength, _("    Light inds %4d      Heavy inds %4d        Recyclers %4d")
	     ,group_count[GROUP_INDUSTRY_L], group_count[GROUP_INDUSTRY_H]
	     ,group_count[GROUP_RECYCLE]);
    setParagraphN( "statistic_text", line++, outf );
    snprintf (outf, maxlength, _("Health centres %4d            Tips %4d         Shanties %4d"),
	     group_count[GROUP_HEALTH], group_count[GROUP_TIP],
	     group_count[GROUP_SHANTY]);
    setParagraphN( "statistic_text", line++, outf );
    while( line <= 23 ){ //clear remaining lines
        setParagraphN( "statistic_text", line++, "" );
    }
    free( outf ); 
    // connect signals
    Button* noButton = getButton( *myDialogComponent, "Okay" );
    noButton->clicked.connect( makeCallback( *this, &Dialog::closeDialogButtonClicked ) );
}

void Dialog::editMarket(){
    if( !desktop ) {
        std::cerr << "No desktop found.\n";
        return;
    }
    try {
        myDialogComponent = loadGUIFile( "gui/tradedialog.xml" );
        assert( myDialogComponent != 0);
        desktop->addChildComponent( myDialogComponent );
        blockingDialogIsOpen = true;
        iAmBlocking = true;
    } catch(std::exception& e) {
        std::cerr << "Couldn't display dialog 'tradedialog.xml': "
            << e.what() << "\n";
        return;
    }
    // set Dialog to Market-Data
    Paragraph* p = getParagraph( *myDialogComponent, "tradeTitle" );
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
        desktop->addChildComponent( myDialogComponent );
        blockingDialogIsOpen = true;
        iAmBlocking = true;
    } catch(std::exception& e) {
        std::cerr << "Couldn't display dialog 'tradedialog.xml': "
            << e.what() << "\n";
        return;
    }
    // set Dialog to Port-Data
    Paragraph* p = getParagraph( *myDialogComponent, "tradeTitle" );
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
    delete( this );
}

void Dialog::okayLaunchRocketButtonClicked( Button* ){
    launch_rocket( pointX, pointY );
    desktop->remove( myDialogComponent );
    blockingDialogIsOpen = false;
    delete( this );
}


void Dialog::okayCoalSurveyButtonClicked( Button* ){
    do_coal_survey();    
    desktop->remove( myDialogComponent );
    blockingDialogIsOpen = false;
    delete( this );
}

void Dialog::okayBulldozeRiverButtonClicked( Button* ){
    river_bul_flag = 1;
    desktop->remove( myDialogComponent );
    check_bulldoze_area( pointX, pointY );
    blockingDialogIsOpen = false;
    delete( this );
}

void Dialog::okayBulldozeShantyButtonClicked( Button* ){
    shanty_bul_flag = 1;
    desktop->remove( myDialogComponent );
    check_bulldoze_area( pointX, pointY );
    blockingDialogIsOpen = false;
    delete( this );
}

void Dialog::okayBulldozeMonumentButtonClicked( Button* ){
    monument_bul_flag = 1;
    desktop->remove( myDialogComponent );
    check_bulldoze_area( pointX, pointY );
    blockingDialogIsOpen = false;
    delete( this );
}

void Dialog::gotoButtonClicked( Button* ){
    getGameView()->show( MapPoint( pointX, pointY ) );
}

void Dialog::closeDialogButtonClicked( Button* ){
    desktop->remove( myDialogComponent );
    if( iAmBlocking ){
        blockingDialogIsOpen = false;
    }
    delete( this );
}
