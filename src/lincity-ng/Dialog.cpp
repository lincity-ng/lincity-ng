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

#include "gui/ComponentLoader.hpp"
#include "gui/Button.hpp"
#include "gui/callback/Callback.hpp"
#include "gui/Paragraph.hpp"

bool blockingDialogIsOpen = false;

Dialog::Dialog( int type ){
    initDialog();   
    switch( type ){
        case ASK_COAL_SURVEY:
            coalSurvey();
            break;
        default:
            std::stringstream msg;
            msg <<"Can't open Dialog type " << type << " without coordinates.";
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
        default:
            std::stringstream msg;
            msg <<"Unknown Dialog type " << type << ".";
            throw std::runtime_error(msg.str());
    }
}

void Dialog::initDialog( int x /*= -1*/, int y /*= -1*/ ){
    Component* root = getGameView();
    desktop = 0;
    myDialogComponent = 0;
    pointX = x;
    pointY = y;
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
    } catch(std::exception& e) {
        std::cerr << "Couldn't display dialog 'tradedialog.xml': "
            << e.what() << "\n";
        return;
    }
    // set Dialog to Market-Data
    Paragraph* p = getParagraph( *myDialogComponent, "tradeTitle" );
    std::stringstream title;
	title << "Market ( " << pointX <<" , " << pointY << " )";
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
    } catch(std::exception& e) {
        std::cerr << "Couldn't display dialog 'tradedialog.xml': "
            << e.what() << "\n";
        return;
    }
    // set Dialog to Port-Data
    Paragraph* p = getParagraph( *myDialogComponent, "tradeTitle" );
    std::stringstream title;
	title << "Port ( " << pointX <<" , " << pointY << " )";
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

void Dialog::closeDialogButtonClicked( Button* ){
    desktop->remove( myDialogComponent );
    blockingDialogIsOpen = false;
    delete( this );
}
